#pragma once

#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "bc/ui/mainWindow.hpp"
#include "common.hpp"

class FrameComponent : public wxPanel {
public:
  explicit FrameComponent(wxWindow *parent, const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                          std::array<std::string, RT_SA_MAX_COUNT> data);

  void updateValues(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                    std::array<std::string, RT_SA_MAX_COUNT> data);

private:
  void onSendSingle(wxCommandEvent &event);
  void onRemove(wxCommandEvent &event);
  void onEdit(wxCommandEvent &event);
  void onActivateToggle(wxCommandEvent &event);

  BusControllerFrame *m_mainWindow;

  wxStaticText *m_label{};
  wxToggleButton *m_activateToggle{};

  char m_bus{};
  int m_rt{};
  int m_sa{};
  int m_wc{};
  BcMode m_mode{};
  std::array<std::string, RT_SA_MAX_COUNT> m_data;
};