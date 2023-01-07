#pragma once
#include "pch.hpp"

struct ProcessMkvInput
{
  fs::path              mkvsDirectory;
  std::vector<fs::path> mkvs;
  std::vector<fs::path> subs;
  std::vector<fs::path> audios;
};

struct MkvCombineTask
{
  fs::path                mkvFile;
  std::optional<fs::path> subFile;
  std::optional<fs::path> audioFile;
};

using MkvCombineTasks = std::vector<MkvCombineTask>;

std::optional<MkvCombineTasks> makeMkvCombineTasks( ProcessMkvInput input );
