#pragma once
#include "pch.hpp"


struct RunMkvMergeOptions
{
  fs::path                mkvToolnixPath;
  fs::path                outputPath;
  fs::path                mkvPath;
  std::optional<fs::path> subtitlePath;
  std::optional<fs::path> audioPath;
};

struct MkvMergeResult
{
  std::optional<const char*> error;
};

MkvMergeResult runMkvMergeTool( const RunMkvMergeOptions& options );
