#include "main-frame.hpp"
#include "process-mkv.hpp"


namespace
{
  class ButtonDisableScope
  {
    wxButton* button_;

  public:
    explicit ButtonDisableScope( wxButton* button )
        : button_{ button }
    {
      button_->Disable();
    }

    ~ButtonDisableScope()
    {
      button_->Enable( true );
    }
  };
} // namespace


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

    Bind( wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello );
    Bind( wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT );
    Bind( wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT );
  }

  // STATUS BAR
  {
    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );
  }

  // LAYOUT
  {
    mkvFolderInput   = new FolderInput{ this, "mkv dir:", ".mkv" };
    subsFolderInput  = new FolderInput{ this, "subs dir:", ".ass", FolderInputFlags::AskExtension };
    audioFolderInput = new FolderInput{ this, "audio dir:", ".mka", FolderInputFlags::AskExtension };
    SetDropTarget( new DropBox{ { mkvFolderInput, subsFolderInput, audioFolderInput } } );

    constexpr int rows = 1;
    constexpr int cols = 3;
    const auto    gap  = wxSize{ 20, 0 };

    auto* grid = new wxGridSizer{ rows, cols, gap };
    grid->Add( mkvFolderInput, 1, wxEXPAND );
    grid->Add( subsFolderInput, 1, wxEXPAND );
    grid->Add( audioFolderInput, 1, wxEXPAND );

    processButton_ = new wxButton{ this, ID_Process, wxT( "process" ) };
    Bind( wxEVT_BUTTON, &MainFrame::OnProcess, this, ID_Process );

    auto* topBox = new wxBoxSizer{ wxVERTICAL };
    topBox->Add( grid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10 );
    topBox->Add( processButton_, 0, wxGROW | wxALL, 15 );
    SetSizer( topBox );

    SetMinSize( wxSize{ 1400, 800 } );
    SetBackgroundColour( wxColour{ "#ffffff" } );
  }

  wxLogInfo( "main frame created" );
}


void MainFrame::OnExit( wxCommandEvent& )
{
  constexpr bool force = true;
  Close( force );
}


void MainFrame::OnAbout( wxCommandEvent& )
{
  wxMessageBox( "This is a wxWidgets Hello World example",
                "About Hello World", wxOK | wxICON_INFORMATION );
}


void MainFrame::OnHello( wxCommandEvent& )
{
  wxLogInfo( "Hello world from wxWidgets!" );
}


void MainFrame::OnProcess( wxCommandEvent& )
{
  if( processUpdate_.isUpdateInProgress() )
    return;

  auto buttonDisabler = ButtonDisableScope{ processButton_ };
  auto processLock    = UpdateInProgress{ processUpdate_ };

  auto tasks = makeMkvCombineTasks( ProcessMkvInput{
      .mkvsDirectory = mkvFolderInput->getBaseDirectory(),
      .mkvs          = std::move( mkvFolderInput->getFilteredFileList() ),
      .subs          = std::move( subsFolderInput->getFilteredFileList() ),
      .audios        = std::move( audioFolderInput->getFilteredFileList() ),
  } );

  if( !tasks )
    return;

  // TODO: make this persistent setting in app
  auto mkvToolnixPath = fs::path{ "C:\\Program Files\\MKVToolNix" };

  runMkvCombine( mkvToolnixPath, *tasks, [this]( uint32_t taskRemaining, uint32_t taskCount ) {
    auto tasksDone = taskCount - taskRemaining;
    SetStatusText( wxString::Format( "processing... %u/%u", tasksDone, taskCount ) );
    wxWindow::Update();
  } );

  SetStatusText( "processing done!" );
}
