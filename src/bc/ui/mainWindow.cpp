#include "mainWindow.hpp"

#include <nlohmann/json.hpp>
#include <string>

#include <wx/wx.h>

#include "bc.hpp"
#include "bcGuiCommon.hpp"
#include "createFrameWindow.hpp"

BusControllerFrame::BusControllerFrame() : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Controller") {
  auto *menuFile = new wxMenu;
  menuFile->AppendSeparator();

  int addFrameId = wxNewId();
  menuFile->Append(addFrameId, "Add Frame\tCtrl-A", "Add a frame to the frame list");
  
  int clearFramesId = wxNewId();
  menuFile->Append(clearFramesId, "Clear All Frames\tCtrl-W", "Clear all frames from the frame list");
  
  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&Commands");

  auto *deviceIdLabel = new wxStaticText(this, wxID_ANY, "DDC Device ID");

  m_deviceIdTextInput = new wxTextCtrl(
      this, wxID_ANY, "00", wxDefaultPosition,
      wxSize(30, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_repeatToggle = new wxToggleButton(
      this, wxID_ANY, "Repeat Off", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_sendActiveFramesToggle = new wxToggleButton(
      this, wxID_ANY, "Send Active Frames", wxDefaultPosition,
      wxSize(150, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_addButton = new wxButton(
      this, wxID_ANY, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_addButton->SetBackgroundColour(wxColour("#ffcc00"));
  m_addButton->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  // Replace bottomHorizontalSizer with scrolled window
  m_scrolledWindow = new wxScrolledWindow(this, wxID_ANY);
  m_scrolledWindow->SetBackgroundColour(this->GetBackgroundColour());
  m_scrolledSizer = new wxBoxSizer(wxVERTICAL);
  m_scrolledWindow->SetSizer(m_scrolledSizer);
  m_scrolledWindow->SetScrollRate(10, 10); // Set scroll speed

  topHorizontalSizer->Add(deviceIdLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddStretchSpacer();
  topHorizontalSizer->Add(m_repeatToggle, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddSpacer(5);
  topHorizontalSizer->Add(m_sendActiveFramesToggle, 0,
                          wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddSpacer(5);
  topHorizontalSizer->Add(m_addButton, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  verticalSizer->Add(topHorizontalSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  verticalSizer->Add(m_scrolledWindow, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  SetSizer(verticalSizer);

  verticalSizer->Fit(this);
  verticalSizer->SetSizeHints(this);

  CreateStatusBar();
  SetStatusText("Ready, add frames to send");

  Bind(wxEVT_MENU, &BusControllerFrame::onAddFrameClicked, this, addFrameId);
  Bind(wxEVT_MENU, &BusControllerFrame::onClearFramesClicked, this, clearFramesId);
  Bind(wxEVT_MENU, &BusControllerFrame::onExit, this, wxID_EXIT);

  m_addButton->Bind(wxEVT_BUTTON, &BusControllerFrame::onAddFrameClicked, this);
  m_repeatToggle->Bind(wxEVT_TOGGLEBUTTON, &BusControllerFrame::onRepeatToggle, this);
  m_sendActiveFramesToggle->Bind(wxEVT_TOGGLEBUTTON, &BusControllerFrame::onSendActiveFrames, this);

  m_deviceIdTextInput->SetValue(std::to_string(BC::getInstance().getDevNum()));
  SetSize(650, 650);
}

void BusControllerFrame::onAddFrameClicked(wxCommandEvent & /*event*/) {
  auto *frame = new FrameCreationFrame(this);
  frame->Show(true);
}

void BusControllerFrame::onClearFramesClicked(wxCommandEvent & /*event*/) {
  m_scrolledSizer->Clear(true);
}

void BusControllerFrame::onRepeatToggle(wxCommandEvent & /*event*/) {
  if (m_repeatToggle->GetValue()) {
    m_repeatToggle->SetLabel("Repeat On");
  } else {
    m_repeatToggle->SetLabel("Repeat Off");
    stopSending();
  }
}

void BusControllerFrame::onSendActiveFrames(wxCommandEvent &event) {
  if (m_sendActiveFramesToggle->GetValue()) {
    m_sendActiveFramesToggle->SetLabel("Sending Active Frames");

    if (m_repeatToggle->GetValue()) {
      startSendingThread();
    } else {
      sendActiveFrames();
      stopSending();
    }
  } else {
    m_sendActiveFramesToggle->SetLabel("Send Active Frames");
    stopSending();
  }
}

void BusControllerFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }

void BusControllerFrame::sendActiveFrames() {
  for (auto &child : m_scrolledSizer->GetChildren()) {
    auto *frame = dynamic_cast<FrameComponent *>(child->GetWindow());

    if (frame != nullptr and frame->isActive()) {
      frame->sendFrame();

      std::this_thread::sleep_for(
          std::chrono::milliseconds(100)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
  }
}

void BusControllerFrame::startSendingThread() {
  try {
    m_isSending = true;
    m_repeatedSendThread = std::thread([&] {
      while (m_isSending) {
        sendActiveFrames();
      }
    });
  } catch (std::exception &e) {
    Logger::error("Failed to start sending thread, " + std::string(e.what()));
  }
}

void BusControllerFrame::stopSending() {
  m_isSending = false;
  if (m_repeatedSendThread.joinable()) {
    m_repeatedSendThread.join();
  }

  m_sendActiveFramesToggle->SetValue(false);
  m_sendActiveFramesToggle->SetLabel("Send Active Frames");
}

void BusControllerFrame::setStatusText(const wxString &status) { SetStatusText(status); }

int BusControllerFrame::getDeviceId() { return wxAtoi(m_deviceIdTextInput->GetValue()); }

void BusControllerFrame::moveUp(FrameComponent *item) {
  int index = getFrameIndex(item);

  if (index == 0) {
    return;
  }

  m_scrolledSizer->Remove(index);
  m_scrolledSizer->Insert(index - 1, item, 0, wxEXPAND | wxALL, 5);

  updateList();
}

void BusControllerFrame::moveDown(FrameComponent *item) {
  int index = getFrameIndex(item);

  if (index == m_scrolledSizer->GetItemCount() - 1) {
    return;
  }

  m_scrolledSizer->Remove(index);
  m_scrolledSizer->Insert(index + 1, item, 0, wxEXPAND | wxALL, 5);

  updateList();
}

void BusControllerFrame::updateList() {
  m_scrolledWindow->FitInside(); // Update scrollable area
}

int BusControllerFrame::getFrameIndex(FrameComponent *frame) {
  auto *item = m_scrolledSizer->GetItem(frame);

  for (int i = 0; i < m_scrolledSizer->GetItemCount(); ++i) {
    if (m_scrolledSizer->GetItem(i) == item) {
      return i;
    }
  }

  return -1;
}

void BusControllerFrame::addFrameToList(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                                        std::array<std::string, RT_SA_MAX_COUNT> data) {
  auto *component = new FrameComponent(m_scrolledWindow, label, bus, rt, sa, wc, mode, data);
  m_scrolledSizer->Add(component, 0, wxEXPAND | wxALL, 5);

  updateList();
}
