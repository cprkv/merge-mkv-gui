#include "pch.hpp"
#include "main-frame.hpp"
#include <wx/app.h>

class MyApp : public wxApp
{
public:
  bool OnInit() override
  {
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );

    // LOGGING
    {
      constexpr wxWindow* parent    = nullptr;
      constexpr bool      show      = true;
      constexpr bool      passToOld = false;

      auto* logWindow = new wxLogWindow{ parent, "logger", show, passToOld };
      wxLog::SetActiveTarget( logWindow );
    }

    // APP FRAMES
    {
      auto* frame = new MainFrame{};
      frame->Show( true );
    }

    return true;
  }

  bool OnExceptionInMainLoop() override
  {
    auto exception = std::current_exception();
    if( exception )
    {
      try
      {
        std::rethrow_exception( exception );
      }
      catch( std::exception& ex )
      {
        wxLogError( "exception: %s", ex.what() );
      }
      catch( ... )
      {
        wxLogError( "unknown exception" );
      }
    }
    std::terminate();
  }
};

wxIMPLEMENT_APP( MyApp );
