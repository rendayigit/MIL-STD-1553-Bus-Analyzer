#pragma once

#include <mutex>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "common.hpp"
#include "mainWindow.hpp"

class FrameComponent : public wxPanel {
public:
  explicit FrameComponent(wxWindow *parent, const std::string &label, char bus, int rt, int rt2, int sa, int sa2,
                          int wc, BcMode mode, std::array<std::string, RT_SA_MAX_COUNT> data);

  void updateValues(const std::string &label, char bus, int rt, int rt2, int sa, int sa2, int wc, BcMode mode,
                    std::array<std::string, RT_SA_MAX_COUNT> data);
  void sendFrame();
  bool isActive();

  wxString getLabel() const { return m_label; }
  char getBus() const { return m_bus; }
  int getRt() const { return m_rt; }
  int getRt2() const { return m_rt2; }
  int getSa() const { return m_sa; }
  int getSa2() const { return m_sa2; }
  int getWc() const { return m_wc; }
  BcMode getMode() const { return m_mode; }
  std::array<std::string, RT_SA_MAX_COUNT> getData() const { return m_data; }

private:
  void onSend(wxCommandEvent &event);
  void onRemove(wxCommandEvent &event);
  void onEdit(wxCommandEvent &event);
  void onActivateToggle(wxCommandEvent &event);
  void onUp(wxCommandEvent &event);
  void onDown(wxCommandEvent &event);

  void updateData(std::array<std::string, RT_SA_MAX_COUNT> data);

  std::mutex m_mutex;

  BusControllerFrame *m_mainWindow;

  wxStaticText *m_allText{};
  wxToggleButton *m_activateToggle{};

  std::string m_label;
  char m_bus{};
  int m_rt{};
  int m_rt2{};
  int m_sa{};
  int m_sa2{};
  int m_wc{};
  BcMode m_mode{};
  std::array<std::string, RT_SA_MAX_COUNT> m_data;
};