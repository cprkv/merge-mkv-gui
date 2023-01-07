#include "main-frame.hpp"
#include "process-mkv.hpp"

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

  auto tasks = makeMkvCombineTasks( ProcessMkvInput{
      .mkvsDirectory = mkvPanel->getBaseDirectory(),
      .mkvs          = std::move( mkvPanel->getFilteredFileList() ),
      .subs          = std::move( subsPanel->getFilteredFileList() ),
      .audios        = std::move( audioPanel->getFilteredFileList() ),
  } );

  if( !tasks )
    return;
}
