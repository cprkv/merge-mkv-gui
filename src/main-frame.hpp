#pragma once
#include "pch.hpp"
#include "folder-input.hpp"
#include "internal-update.hpp"


class MainFrame : public wxFrame
{
  FolderInput*   mkvFolderInput;
  FolderInput*   subsFolderInput;
  FolderInput*   audioFolderInput;
  wxButton*      processButton_;
  InternalUpdate processUpdate_;

public:
  MainFrame();

private:
  void OnHello( wxCommandEvent& event );
  void OnExit( wxCommandEvent& event );
  void OnAbout( wxCommandEvent& event );
  void OnProcess( wxCommandEvent& event );
};

enum
{
  ID_Hello = 1,
  ID_Process,
};
