#pragma once

#include "bc.hpp"
#include <wx/wx.h>

class BusControllerFrame : public wxFrame {
public:
  BusControllerFrame();
  void addFrameToList(char bus, int rt, int sa, int wc, int mode, int data);

private:
  void onAddClicked(wxCommandEvent &event);
  void onExit(wxCommandEvent &event);

  BC m_bc;
  wxTextCtrl *m_deviceIdTextInput;
  wxButton *m_addButton;
  wxScrolledWindow *m_scrolledWindow;
  wxBoxSizer *m_scrolledSizer;
};
