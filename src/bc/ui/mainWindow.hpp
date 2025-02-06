#pragma once

#include <wx/string.h>
#include <wx/wx.h>

#include "bcGuiCommon.hpp"
#include "common.hpp"

class BusControllerFrame : public wxFrame {
public:
  BusControllerFrame();
  void addFrameToList(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                      std::array<std::string, RT_SA_MAX_COUNT> data);
  void setStatusText(const wxString &status);
  int getDeviceId();
  void updateList();

private:
  void onAddClicked(wxCommandEvent &event);
  void onExit(wxCommandEvent &event);

  wxTextCtrl *m_deviceIdTextInput;
  wxButton *m_addButton;
  wxScrolledWindow *m_scrolledWindow;
  wxBoxSizer *m_scrolledSizer;
};
