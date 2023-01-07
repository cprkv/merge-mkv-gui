#include "process-mkv.hpp"
#include "string-utils.hpp"

#define mErrorIf( cond, message )                                             \
  if( cond )                                                                  \
  {                                                                           \
    wxMessageBox( message, "error process", wxICON_ERROR | wxOK | wxCENTRE ); \
    return std::nullopt;                                                      \
  }

namespace
{
  using PathDiff = std::map<std::string, fs::path>;

  std::optional<PathDiff> diffPath( const std::vector<fs::path>& paths )
  {
    std::vector<std::string> pathStrings( paths.size() );
    std::transform( paths.begin(), paths.end(), pathStrings.begin(), []( auto& p ) { return p.string(); } );

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

  //  void logDiffs( const char* desc, const PathDiff& a )
  //  {
  //    for( const auto& [key, _]: a )
  //    {
  //      wxLogInfo( "%s diffs: %s", desc, key.c_str() );
  //    }
  //  }

  void logTasks( const MkvCombineTasks& tasks )
  {
    for( const auto& task: tasks )
    {
      wxLogInfo( "mkv combine task: %s + %s + %s",
                 task.mkvFile.filename().string().c_str(),
                 task.subFile ? task.subFile->filename().string().c_str() : "<none>",
                 task.audioFile ? task.audioFile->filename().string().c_str() : "<none>" );
    }
  }
} // namespace

std::optional<MkvCombineTasks> makeMkvCombineTasks( ProcessMkvInput input )
{
  wxLogInfo( "processMkv: mkv directory: %s", input.mkvsDirectory.string().c_str() );
  wxLogInfo( "processMkv: mkv files count: %d", ( int ) input.mkvs.size() );
  wxLogInfo( "processMkv: sub files count: %d", ( int ) input.subs.size() );
  wxLogInfo( "processMkv: audio files count: %d", ( int ) input.audios.size() );

  mErrorIf( input.mkvsDirectory.empty() || input.mkvs.empty(), "no mkv directory selected" );
  mErrorIf( input.subs.empty() && input.audios.empty(), "nothing to merge" );

  auto mkvDiffs = diffPath( input.mkvs );
  mErrorIf( !mkvDiffs, "can't determine different parts in mkv files" );
  //  logDiffs( "mkv", *mkvDiffs );

  auto subDiffs   = std::optional<PathDiff>{};
  auto audioDiffs = std::optional<PathDiff>{};

  if( !input.subs.empty() )
  {
    subDiffs = diffPath( input.subs );
    mErrorIf( !subDiffs, "can't determine different parts in sub files" );
    //    logDiffs( "sub", *subDiffs );
    mErrorIf( !hasSmthInCommon( *subDiffs, *mkvDiffs ), "can't find common subs for mkvs" );
  }

  if( !input.audios.empty() )
  {
    audioDiffs = diffPath( input.audios );
    mErrorIf( !audioDiffs, "can't determine different parts in audio files" );
    //    logDiffs( "audio", *audioDiffs );
    mErrorIf( !hasSmthInCommon( *audioDiffs, *mkvDiffs ), "can't find common audios for mkvs" );
  }

  auto tasks = MkvCombineTasks{};

  for( const auto& [mkvDiff, mkvPath]: *mkvDiffs )
  {
    auto task = MkvCombineTask{ .mkvFile = mkvPath };

    if( subDiffs )
    {
      if( auto it = subDiffs->find( mkvDiff ); it != subDiffs->end() )
      {
        task.subFile = it->second;
      }
    }

    if( audioDiffs )
    {
      if( auto it = audioDiffs->find( mkvDiff ); it != audioDiffs->end() )
      {
        task.audioFile = it->second;
      }
    }

    tasks.emplace_back( std::move( task ) );
  }

  logTasks( tasks );

  return std::nullopt;
}
