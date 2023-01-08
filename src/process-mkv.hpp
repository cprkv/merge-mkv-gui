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
  fs::path                destination;
  std::optional<fs::path> subFile;
  std::optional<fs::path> audioFile;
};

using MkvCombineTasks = std::vector<MkvCombineTask>;


std::optional<MkvCombineTasks> makeMkvCombineTasks( ProcessMkvInput input );

// notifyRemaining: (uint32_t taskRemaining, uint32_t taskCount) -> void
void runMkvCombine( const fs::path&                                  mkvToolnixPath,
                    const MkvCombineTasks&                           tasks,
                    const std::function<void( uint32_t, uint32_t )>& notifyRemaining );
