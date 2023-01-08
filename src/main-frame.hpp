#pragma once
#include "pch.hpp"
#include "folder-input.hpp"


class MainFrame : public wxFrame
{
  FolderInput* mkvFolderInput;
  FolderInput* subsFolderInput;
  FolderInput* audioFolderInput;
  wxButton*    processButton_;
  bool         updateInProgress_{ false };

public:
  MainFrame();

private:
  void OnExit( wxCommandEvent& event );
  void OnAbout( wxCommandEvent& event );
  void OnProcess( wxCommandEvent& event );
  void OnProcessStatusUpdate( wxThreadEvent& event );
};

enum
{
  ID_Process = 1,
};
