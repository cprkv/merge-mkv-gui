#pragma once
#include "pch.hpp"
#include "folder-input.hpp"

class MainFrame : public wxFrame
{
  FolderInput* mkvPanel;
  FolderInput* subsPanel;
  FolderInput* audioPanel;

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
