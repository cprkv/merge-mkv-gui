#include "process-mkv.hpp"
#include "string-utils.hpp"
#include "thread-pool.hpp"
#include "mkv-merge-tool.hpp"


#define mErrorIf( cond, message )                                             \
  if( cond )                                                                  \
  {                                                                           \
    wxMessageBox( message, "error process", wxICON_ERROR | wxOK | wxCENTRE ); \
    return std::nullopt;                                                      \
  }


namespace
{
  using PathDiff = std::map<std::u8string, fs::path>;


  std::optional<PathDiff> diffPath( const std::vector<fs::path>& paths )
  {
    std::vector<std::u8string> pathStrings( paths.size() );
    std::transform( paths.begin(), paths.end(), pathStrings.begin(), []( auto& p ) { return p.u8string(); } );

    auto commonPrefix = commonPathPrefix( pathStrings );
    auto commonSuffix = commonPathSuffix( pathStrings );

    PathDiff result;

    for( size_t i = 0; i < pathStrings.size(); ++i )
    {
      auto& pathStr    = pathStrings[i];
      auto  differPart = pathStr.substr( commonPrefix.length(),
                                         pathStr.length() - commonPrefix.length() - commonSuffix.length() );

      if( result.contains( differPart ) )
        return std::nullopt;

      result[differPart] = paths[i];
    }

    return result;
  }


  bool hasSmthInCommon( const PathDiff& a, const PathDiff& b )
  {
    return std::any_of( a.begin(), a.end(), [&]( auto& p ) { return b.contains( p.first ); } );
  }


  void logTasks( const MkvCombineTasks& tasks )
  {
    for( const auto& task: tasks )
    {
      wxLogInfo( "mkv combine task: %s + %s + %s",
                 toWxString( task.mkvFile.filename() ),
                 task.subFile ? toWxString( task.subFile->filename() ) : wxString{ "<none>" },
                 task.audioFile ? toWxString( task.audioFile->filename() ) : wxString{ "<none>" } );
    }
  }


  std::optional<fs::path> makeMergedDirectory( const fs::path& parent )
  {
    for( uint32_t i = 0; i < 100; ++i )
    {
      char folderName[64] = { 0 };
      sprintf( folderName, "merged-%02u", i );
      auto path = parent / folderName;

      if( std::error_code ec; !fs::exists( path, ec ) && !ec )
      {
        if( fs::create_directory( path, ec ); !ec )
          return path;
      }
    }

    return std::nullopt;
  }
} // namespace


std::optional<MkvCombineTasks> makeMkvCombineTasks( ProcessMkvInput input )
{
  wxLogInfo( "processMkv: mkv directory: %s", toWxString( input.mkvsDirectory ) );
  wxLogInfo( "processMkv: mkv files count: %d", ( int ) input.mkvs.size() );
  wxLogInfo( "processMkv: sub files count: %d", ( int ) input.subs.size() );
  wxLogInfo( "processMkv: audio files count: %d", ( int ) input.audios.size() );

  mErrorIf( input.mkvsDirectory.empty() || input.mkvs.empty(), "no mkv directory selected" );
  mErrorIf( input.subs.empty() && input.audios.empty(), "nothing to merge" );

  auto mkvDiffs = diffPath( input.mkvs );
  mErrorIf( !mkvDiffs, "can't determine different parts in mkv files" );

  auto subDiffs   = std::optional<PathDiff>{};
  auto audioDiffs = std::optional<PathDiff>{};

  if( !input.subs.empty() )
  {
    subDiffs = diffPath( input.subs );
    mErrorIf( !subDiffs, "can't determine different parts in sub files" );
    mErrorIf( !hasSmthInCommon( *subDiffs, *mkvDiffs ), "can't find common subs for mkvs" );
  }

  if( !input.audios.empty() )
  {
    audioDiffs = diffPath( input.audios );
    mErrorIf( !audioDiffs, "can't determine different parts in audio files" );
    mErrorIf( !hasSmthInCommon( *audioDiffs, *mkvDiffs ), "can't find common audios for mkvs" );
  }

  auto destination = makeMergedDirectory( input.mkvsDirectory );
  mErrorIf( !destination, "can't create merged folder" );

  auto tasks = MkvCombineTasks{};

  for( const auto& [mkvDiff, mkvPath]: *mkvDiffs )
  {
    auto task = MkvCombineTask{ .mkvFile = mkvPath, .destination = *destination / mkvPath.filename() };

    if( subDiffs )
      if( auto it = subDiffs->find( mkvDiff ); it != subDiffs->end() )
        task.subFile = it->second;

    if( audioDiffs )
      if( auto it = audioDiffs->find( mkvDiff ); it != audioDiffs->end() )
        task.audioFile = it->second;

    tasks.emplace_back( std::move( task ) );
  }

  logTasks( tasks );

  return tasks;
}


void runMkvCombine( const fs::path&                                  mkvToolnixPath,
                    const MkvCombineTasks&                           tasks,
                    const std::function<void( uint32_t, uint32_t )>& notifyRemaining )
{
  wxLogInfo( "runMkvCombine(%d);", ( int ) tasks.size() );

  struct TaskResult
  {
    std::optional<const char*> error;
  };

  auto taskFuncs = std::vector<std::function<TaskResult()>>( tasks.size() );

  for( size_t i = 0; i < tasks.size(); ++i )
  {
    auto opts = RunMkvMergeOptions{
        .mkvToolnixPath = mkvToolnixPath,
        .outputPath     = tasks[i].destination,
        .mkvPath        = tasks[i].mkvFile,
        .subtitlePath   = tasks[i].subFile,
        .audioPath      = tasks[i].audioFile,
    };

    taskFuncs[i] = [opts]() -> TaskResult {
      auto r = runMkvMergeTool( opts );
      return { .error = r.error };
    };
  }

  auto pool = ThreadPool<TaskResult>{ taskFuncs };
  pool.start();

  while( true )
  {
    uint32_t jobRemaining = pool.getJobRemaining();
    notifyRemaining( jobRemaining, ( uint32_t ) tasks.size() );

    if( jobRemaining == 0 )
      break;

    std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );
  }

  pool.waitAndExit();
  auto results = pool.getResults();

  for( const auto& result: results )
  {
    wxLogInfo( "task status: %s", result.error ? *result.error : "<OK>" );
  }
}
