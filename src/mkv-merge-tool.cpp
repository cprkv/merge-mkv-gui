#include "mkv-merge-tool.hpp"
#include <subprocess.h>

namespace
{
  std::optional<int> runSubprocess( std::vector<std::string> arguments )
  {
    std::vector<const char*> cmd( arguments.size() + 1, nullptr );
    std::transform( arguments.begin(), arguments.end(), cmd.begin(),
                    []( const auto& s ) { return s.c_str(); } );

    auto subprocess = subprocess_s{};
    if( int result = subprocess_create( cmd.data(), 0, &subprocess ); result != 0 )
      return std::nullopt;

    //  FILE* processStdOut = subprocess_stdout( &subprocess );
    //  FILE* processStdErr = subprocess_stderr( &subprocess );

    int processReturn;
    if( int result = subprocess_join( &subprocess, &processReturn ); result != 0 )
      return std::nullopt;

    if( int result = subprocess_destroy( &subprocess ); result != 0 )
      return std::nullopt;

    return processReturn;
  }
} // namespace


MkvMergeResult runMkvMergeTool( const RunMkvMergeOptions& options )
{
  if( !options.subtitlePath && !options.audioPath )
    return { .error = "nothing to merge" };

  auto mkvMergePath = options.mkvToolnixPath / "mkvmerge.exe";
  if( !fs::exists( mkvMergePath ) )
    return { .error = "mkv toolnix path is invalid" };

  auto arguments = std::vector<std::string>{
      mkvMergePath.string(),
      "-o",
      options.outputPath.string(),
      options.mkvPath.string(),
  };

  if( options.subtitlePath )
    arguments.emplace_back( options.subtitlePath->string() );

  if( options.audioPath )
    arguments.emplace_back( options.audioPath->string() );

  auto status = runSubprocess( std::move( arguments ) );

  if( !status )
    return { .error = "error running mkvmerge" };

  if( *status != 0 )
    return { .error = "bad mkvmerge exit code" };

  return { .error = std::nullopt };
}
