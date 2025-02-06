#pragma once

#include <wx/wx.h>

#include "frameComponent.hpp"

class FrameCreationFrame : public wxFrame {
public:
  explicit FrameCreationFrame(wxWindow *parent);
  explicit FrameCreationFrame(wxWindow *parent, FrameComponent *frame);

private:
  void createFrame();

  void onSaveAdd(wxCommandEvent &event);
  void onSaveEdit(wxCommandEvent &event, FrameComponent *frame);
  void onWcChanged(wxCommandEvent &event);
  void onRandomize(wxCommandEvent &event);
  void onClose(wxCommandEvent &event);

  wxWindow *m_parent;

  wxButton *m_saveButton{};
  wxComboBox *m_busCombo{};
  wxComboBox *m_rtCombo{};
  wxComboBox *m_saCombo{};
  wxComboBox *m_wcCombo{};
  wxComboBox *m_modeCombo{};
  wxTextCtrl *m_labelTextCtrl{};

  std::vector<wxTextCtrl *> m_dataTextCtrls;
};
