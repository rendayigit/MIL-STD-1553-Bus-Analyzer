#pragma once

#include "bc.hpp"
#include "bcGuiCommon.hpp"
#include <wx/wx.h>

class BusControllerFrame : public wxFrame {
public:
  BusControllerFrame();
  void addFrameToList(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                      std::array<std::string, RT_SA_MAX_COUNT> data);

private:
  void onAddClicked(wxCommandEvent &event);
  void onExit(wxCommandEvent &event);

  BC m_bc;
  wxTextCtrl *m_deviceIdTextInput;
  wxButton *m_addButton;
  wxScrolledWindow *m_scrolledWindow;
  wxBoxSizer *m_scrolledSizer;
};
