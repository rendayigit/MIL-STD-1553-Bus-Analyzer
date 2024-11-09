#pragma once

#include "bm.hpp"
#include <mutex>
#include <wx/treectrl.h>
#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
  MyFrame();

private:
  void onConnectClicked(wxCommandEvent &event);
  void onSaClicked(wxTreeEvent &event);
  void onExit(wxCommandEvent &event);

  BM bm;
  int m_uiRecentMessageCount;
  std::mutex m_mutex;
  wxTreeCtrl *rtSaTree;
  wxTextCtrl *messageList;
  std::array<wxTreeItemId, 32> rtTreeArray;
  std::array<std::array<wxTreeItemId, 32>, 32> saTreeArray;
};
