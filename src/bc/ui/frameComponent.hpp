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
  void sendFrame();
  bool isActive();

  wxString getLabel() const { return m_label; }
  char getBus() const { return m_bus; }
  int getRt() const { return m_rt; }
  int getSa() const { return m_sa; }
  int getWc() const { return m_wc; }
  BcMode getMode() const { return m_mode; }
  std::array<std::string, RT_SA_MAX_COUNT> getData() const { return m_data; }

private:
  void onSend(wxCommandEvent &event);
  void onRemove(wxCommandEvent &event);
  void onEdit(wxCommandEvent &event);
  void onActivateToggle(wxCommandEvent &event);

  void updateData(std::array<std::string, RT_SA_MAX_COUNT> data);

  BusControllerFrame *m_mainWindow;

  wxStaticText *m_allText{};
  wxToggleButton *m_activateToggle{};

  std::string m_label;
  char m_bus{};
  int m_rt{};
  int m_sa{};
  int m_wc{};
  BcMode m_mode{};
  std::array<std::string, RT_SA_MAX_COUNT> m_data;
};