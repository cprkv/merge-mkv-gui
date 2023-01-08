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
  std::vector<IDropHandler*> dropHandlers_;

public:
  explicit DropBox( std::vector<IDropHandler*> dropHandlers );

  bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames ) override;
};
