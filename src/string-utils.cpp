#include "string-utils.hpp"

std::string commonPathPrefix( const std::vector<std::string>& strings )
{
  auto commonPrefix = strings.empty() ? std::string{} : strings[0];

  for( const auto& path: strings )
  {
    auto s = path;

    auto potentialMatchLength = std::min( s.length(), commonPrefix.length() );

    if( potentialMatchLength < commonPrefix.length() )
      commonPrefix = commonPrefix.substr( 0, potentialMatchLength );

    for( size_t i = 0; i < potentialMatchLength; ++i )
    {
      if( s[i] != commonPrefix[i] )
      {
        commonPrefix = commonPrefix.substr( 0, i );
        break;
      }
    }
  }

  return commonPrefix;
}

std::string commonPathSuffix( const std::vector<std::string>& strings )
{
  auto commonSuffix = strings.empty() ? std::string{} : strings[0];

  for( const auto& s: strings )
  {
    auto potentialMatchLength = std::min( s.length(), commonSuffix.length() );

    if( potentialMatchLength < commonSuffix.length() )
      commonSuffix = commonSuffix.substr( commonSuffix.length() - potentialMatchLength, potentialMatchLength );

    for( size_t i = 0; i < potentialMatchLength; ++i )
    {
      if( s[s.length() - i - 1] != commonSuffix[commonSuffix.length() - i - 1] )
      {
        commonSuffix = commonSuffix.substr( commonSuffix.length() - i, i );
        break;
      }
    }
  }

  return commonSuffix;
}
