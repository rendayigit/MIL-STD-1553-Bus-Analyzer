#pragma once

#include "bc.hpp"
#include <mutex>
#include <wx/wx.h>

class BusControllerFrame : public wxFrame {
public:
  BusControllerFrame();

private:
  void onAddClicked(wxCommandEvent &event);
  void onExit(wxCommandEvent &event);

  BC m_bc;
  std::mutex m_mutex;
  wxTextCtrl *m_deviceIdTextInput;
  wxButton *m_addButton;
  wxScrolledWindow *m_scrolledWindow;
  wxBoxSizer *m_scrolledSizer;
};
