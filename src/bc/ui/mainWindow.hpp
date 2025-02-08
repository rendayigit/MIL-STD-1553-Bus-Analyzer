#pragma once

#include <thread>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "bcGuiCommon.hpp"
#include "common.hpp"

class FrameComponent;

class BusControllerFrame : public wxFrame {
public:
  BusControllerFrame();
  void addFrameToList(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                      std::array<std::string, RT_SA_MAX_COUNT> data);
  void setStatusText(const wxString &status);
  int getDeviceId();

  void moveUp(FrameComponent *item);
  void moveDown(FrameComponent *item);
  int getFrameIndex(FrameComponent *frame);
  void updateList();

private:
  void onAddClicked(wxCommandEvent &event);
  void onRepeatToggle(wxCommandEvent &event);
  void onSendActiveFrames(wxCommandEvent &event);
  void onExit(wxCommandEvent &event);

  void sendActiveFrames();
  void startSendingThread();
  void stopSending();

  wxTextCtrl *m_deviceIdTextInput;
  wxButton *m_addButton;
  wxToggleButton *m_repeatToggle;
  wxToggleButton *m_sendActiveFramesToggle;
  wxScrolledWindow *m_scrolledWindow;
  wxBoxSizer *m_scrolledSizer;

  std::thread m_repeatedSendThread;
  bool m_isSending = false;
};
