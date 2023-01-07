#include "drop-box.hpp"

DropBox::DropBox( std::vector<IDropHandler*> sizers )
    : sizers_{ sizers }
{}

bool DropBox::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
  for( auto* sizer: sizers_ )
  {
    if( !sizer->getBounds().Contains( x, y ) )
      continue;

    sizer->resetDrop();

    for( const auto& filename: filenames )
      handleDrop( sizer, filename );
  }

  return false;
}

void DropBox::handleDrop( IDropHandler* handler, const wxString& string )
{
  auto path = fs::path{ string.fn_str() };

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
