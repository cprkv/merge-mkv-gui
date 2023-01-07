#include "main-frame.hpp"


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
  DropBox( std::vector<IDropHandler*> sizers )
      : sizers_{ sizers }
  {}

  bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames ) override
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

private:
  void handleDrop( IDropHandler* handler, const wxString& string )
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
};


enum class FolderInputFlags
{
  None,
  AskExtension,
};


class FolderInput : public wxBoxSizer, public IDropHandler
{
  wxString    extension_;
  wxTextCtrl* extensionInput_ = nullptr;

  fs::path    baseDirectory_;
  wxTextCtrl* baseDirectoryInput_ = nullptr;

  std::vector<fs::path> allFiles_;
  wxListBox*            allFilesList_ = nullptr;

public:
  FolderInput( wxWindow* parent, const wxString& title, const wxString& extension,
               FolderInputFlags flags = FolderInputFlags::None )
      : wxBoxSizer{ wxVERTICAL }
      , extension_{ extension }
  {
    auto* text          = new wxStaticText{ parent, wxID_ANY, title };
    baseDirectoryInput_ = new wxTextCtrl{ parent, wxID_ANY };
    this->Add( text, 0, wxEXPAND | wxLEFT | wxRIGHT, 5 );
    this->Add( baseDirectoryInput_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    if( !!( flags & FolderInputFlags::AskExtension ) )
    {
      auto* extText   = new wxStaticText{ parent, wxID_ANY, wxT( "extension:" ) };
      extensionInput_ = new wxTextCtrl{ parent, wxID_ANY, extension };
      this->Add( extText, 0, wxEXPAND | wxLEFT | wxRIGHT, 5 );
      this->Add( extensionInput_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
    }

    auto* listText = new wxStaticText{ parent, wxID_ANY, wxT( "files:" ) };
    allFilesList_  = new wxListBox{ parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                   wxLB_HSCROLL };
    this->Add( listText, 0, wxEXPAND | wxLEFT | wxRIGHT, 5 );
    this->Add( allFilesList_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
  }

  void handleFile( const fs::path& path ) override
  {
    wxLogInfo( "file input: %s", path.string().c_str() );
    baseDirectory_ = path.parent_path();
    allFiles_.push_back( path );
    updateControl();
  }

  void handleDirectory( const fs::path& path ) override
  {
    wxLogInfo( "directory input: %s", path.string().c_str() );
    baseDirectory_ = path;

    auto ec       = std::error_code{};
    auto iterator = fs::directory_iterator{ path, ec };

    if( ec )
    {
      wxLogError( "can't get directory status: %s", ec.message().c_str() );
      return;
    }

    for( const auto& entry: iterator )
    {
      bool isRegularFile = entry.is_regular_file( ec );

      if( ec )
      {
        wxLogError( "can't get file status: %s", ec.message().c_str() );
        continue;
      }

      if( !isRegularFile )
        continue;

      allFiles_.push_back( entry.path() );
    }

    updateControl();
  }

  void resetDrop() override
  {
    wxLogInfo( "resetting drop" );
    baseDirectory_ = fs::path{};
    allFiles_.clear();
  }

  wxRect getBounds() const override { return { GetPosition(), GetSize() }; }

private:
  void updateControl()
  {
    baseDirectoryInput_->ChangeValue( baseDirectory_.string().c_str() );
    allFilesList_->Clear();

    if( !allFiles_.empty() )
    {
      auto stringArr = wxArrayString{};
      stringArr.Alloc( allFiles_.size() );
      for( const auto& path: allFiles_ )
      {
        stringArr.Add( path.string().c_str() );
      }

      allFilesList_->InsertItems( stringArr, 0 );
    }
  }
};


MainFrame::MainFrame()
    : wxFrame{ nullptr, wxID_ANY, "merge mkv" }
{
  // MENU
  {
    auto* menuFile = new wxMenu{};
    menuFile->Append( ID_Hello, "&Hello...\tCtrl-H",
                      "Help string shown in status bar for this menu item" );
    menuFile->AppendSeparator();
    menuFile->Append( wxID_EXIT );

    auto* menuHelp = new wxMenu{};
    menuHelp->Append( wxID_ABOUT );

    auto* menuBar = new wxMenuBar{};
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );

    SetMenuBar( menuBar );
  }

  // STATUS BAR
  {
    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );
  }

  FolderInput* mkvPanel;
  FolderInput* subsPanel;
  FolderInput* audioPanel;

  // LAYOUT
  {
    auto* topBox = new wxBoxSizer{ wxVERTICAL };

    constexpr int rows = 1;
    constexpr int cols = 3;
    const auto    gap  = wxSize{ 20, 0 };

    auto* grid = new wxGridSizer{ rows, cols, gap };
    topBox->Add( grid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10 );

    mkvPanel   = new FolderInput{ this, "mkv dir:", ".mkv" };
    subsPanel  = new FolderInput{ this, "subs dir:", ".ass", FolderInputFlags::AskExtension };
    audioPanel = new FolderInput{ this, "audio dir:", ".mka", FolderInputFlags::AskExtension };

    grid->Add( mkvPanel, 1, wxEXPAND );
    grid->Add( subsPanel, 1, wxEXPAND );
    grid->Add( audioPanel, 1, wxEXPAND );

    auto* button = new wxButton{ this, ID_Process, wxT( "process" ) };
    topBox->Add( button, 0, wxGROW | wxALL, 15 );

    SetSizer( topBox );
    SetMinSize( wxSize{ 1400, 800 } );
    SetBackgroundColour( wxColour{ "#ffffff" } );

    wxLogInfo( "main frame created" );
    wxLogError( "main frame created 3" );
  }

  // EVENTS
  {
    SetDropTarget( new DropBox{ { mkvPanel, subsPanel, audioPanel } } );

    Bind( wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello );
    Bind( wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT );
    Bind( wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT );
    Bind( wxEVT_BUTTON, &MainFrame::OnProcess, this, ID_Process );
  }
}

void MainFrame::OnExit( wxCommandEvent& event )
{
  constexpr bool force = true;
  Close( force );
}

void MainFrame::OnAbout( wxCommandEvent& event )
{
  wxMessageBox( "This is a wxWidgets Hello World example",
                "About Hello World", wxOK | wxICON_INFORMATION );
}

void MainFrame::OnHello( wxCommandEvent& event )
{
  wxLogInfo( "Hello world from wxWidgets!" );
}

void MainFrame::OnProcess( wxCommandEvent& event )
{
  wxLogInfo( "OnProcess();" );
}
