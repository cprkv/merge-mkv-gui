#include "string-utils.hpp"

wxString toWxString( const fs::path& path )
{
  auto str = path.u8string();
  return wxString::FromUTF8Unchecked( ( const char* ) str.c_str() );
}
