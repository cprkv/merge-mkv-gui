#include "pch.hpp"
#include "main-frame.hpp"
#include <wx/app.h>

class MyApp : public wxApp
{
public:
  virtual bool OnInit()
  {
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
};

wxIMPLEMENT_APP( MyApp );
