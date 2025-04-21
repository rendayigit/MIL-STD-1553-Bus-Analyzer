#pragma once

#include <wx/wx.h>

#include "common.hpp"

class FrameCreationFrame : public wxFrame {
public:
  explicit FrameCreationFrame(wxWindow *parent, int rt, int sa, std::array<std::string, RT_SA_MAX_COUNT> data);

private:
  void createFrame();

  void onSave(wxCommandEvent &event);
  void onRandomize(wxCommandEvent &event);
  void onClose(wxCommandEvent &event);

  wxBoxSizer *m_mainSizer{};
  wxButton *m_saveButton{};

  std::vector<wxTextCtrl *> m_dataTextCtrls;

  int m_rt;
  int m_sa;
};
