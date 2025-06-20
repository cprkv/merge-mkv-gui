#include "drop-box.hpp"

namespace
{
  void handleDrop( IDropHandler* handler, const wxString& string )
  {
    auto path = fs::path{ string.fn_str().data() };

    auto ec     = std::error_code{};
    auto status = fs::status( path, ec );
    if( ec )
    {
      wxLogError( "error getting file status: %s", ec.message().c_str() );
      return;
    }

    if( status.type() == fs::file_type::directory )
      handler->handleDirectory( path );
    else if( status.type() == fs::file_type::regular )
      handler->handleFile( path );
    else
      wxLogError( "unknown file status: %d", ( int ) status.type() );
  }
} // namespace


DropBox::DropBox( std::vector<IDropHandler*> dropHandlers )
    : dropHandlers_{ std::move( dropHandlers ) }
{}

bool DropBox::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
  for( auto* dropHandler: dropHandlers_ )
  {
    if( !dropHandler->getBounds().Contains( x, y ) )
      continue;

    dropHandler->resetDrop();

    for( const auto& filename: filenames )
      handleDrop( dropHandler, filename );
  }

  return false;
}
