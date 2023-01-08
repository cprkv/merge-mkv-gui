#pragma once
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#  include <wx/wx.h>
#endif

#include <wx/log.h>
#include <wx/file.h>
#include <wx/dnd.h>
#include <wx/aboutdlg.h>
#include <wx/dynarray.h>

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <array>
#include <list>
#include <span>
#include <string>
#include <optional>
#include <filesystem>
#include <queue>
#include <stack>

#include <memory>
#include <algorithm>
#include <utility>
#include <type_traits>

#include <cinttypes>
#include <cstdlib>
#include <cassert>

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <barrier>
#include <latch>
#include <atomic>

namespace fs = std::filesystem;

template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, bool> = true>
constexpr auto operator*( TEnum s ) -> std::underlying_type_t<TEnum>
{
  return static_cast<std::underlying_type_t<TEnum>>( s );
}

template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, bool> = true>
constexpr bool operator!( TEnum s )
{
  return !static_cast<std::underlying_type_t<TEnum>>( s );
}

template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, bool> = true>
constexpr auto operator|( TEnum a, TEnum b ) -> TEnum
{
  return static_cast<TEnum>( *a | *b );
}

template<typename TEnum, std::enable_if_t<std::is_enum_v<TEnum>, bool> = true>
constexpr auto operator&( TEnum a, TEnum b ) -> TEnum
{
  return static_cast<TEnum>( *a & *b );
}
