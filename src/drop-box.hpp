#pragma once
#include "pch.hpp"

struct IDropHandler
{
  virtual ~IDropHandler() = default;

  virtual void   handleDirectory( const fs::path& ) = 0;
  virtual void   handleFile( const fs::path& )      = 0;
  virtual wxRect getBounds() const                  = 0;
  virtual void   resetDrop()                        = 0;
};

class DropBox : public wxFileDropTarget
{
  const char*                name_;
  std::vector<IDropHandler*> sizers_;

public:
  DropBox( std::vector<IDropHandler*> sizers );

  bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames );

private:
  void handleDrop( IDropHandler* handler, const wxString& string );
};
