#pragma once

#include <wx/wx.h>

class FrameCreationFrame : public wxFrame {
public:
  explicit FrameCreationFrame(wxWindow *parent);

private:
  void onAddClicked(wxCommandEvent &event);
  void onWcChanged(wxCommandEvent &event);
  void onClose(wxCloseEvent &event);

  wxComboBox *m_wcCombo;
  std::vector<wxTextCtrl *> m_dataTextCtrls;
};
