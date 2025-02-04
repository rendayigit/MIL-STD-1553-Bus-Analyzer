#pragma once

#include <wx/wx.h>

class FrameCreationFrame : public wxFrame {
public:
  explicit FrameCreationFrame(wxWindow *parent);

private:
  void onAddClicked(wxCommandEvent &event);
  void onClose(wxCloseEvent &event);
};
