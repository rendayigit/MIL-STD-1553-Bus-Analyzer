#pragma once

#include <wx/wx.h>

class FrameCreationFrame : public wxFrame {
public:
  explicit FrameCreationFrame(wxWindow *parent);

private:
  void onSaveClicked(wxCommandEvent &event);
  void onWcChanged(wxCommandEvent &event);
  void onRandomize(wxCommandEvent &event);
  void onClose(wxCommandEvent &event);

  wxWindow *m_parent;

  wxComboBox *m_busCombo;
  wxComboBox *m_rtCombo;
  wxComboBox *m_saCombo;
  wxComboBox *m_wcCombo;
  wxComboBox *m_modeCombo;

  std::vector<wxTextCtrl *> m_dataTextCtrls;
  wxTextCtrl *m_labelTextCtrl;
};
