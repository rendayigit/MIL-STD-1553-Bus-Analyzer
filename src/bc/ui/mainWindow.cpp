#include "mainWindow.hpp"
#include "common.hpp"
#include "wx/debug.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/gtk/button.h"
#include "wx/gtk/colour.h"
#include "wx/gtk/stattext.h"
#include "wx/sizer.h"
#include <array>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

enum {
  ID_START_STOP_BTN = 1,
  ID_START_STOP_MENU,
  ID_DEVICE_ID_TXT
};
constexpr int TOP_BAR_COMP_HEIGHT = 30;

BusControllerFrame::BusControllerFrame() : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Controller") {
  auto *menuFile = new wxMenu;
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&Commands");

  auto *deviceIdText = new wxStaticText(this, wxID_ANY, "DDC Device ID");

  m_deviceIdTextInput = new wxTextCtrl(
      this, ID_DEVICE_ID_TXT, "00", wxDefaultPosition,
      wxSize(30, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_startStopButton = new wxButton(
      this, ID_START_STOP_BTN, "Start", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_startStopButton->SetBackgroundColour(wxColour("#ffcc00"));
  m_startStopButton->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  topHorizontalSizer->Add(deviceIdText, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_startStopButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  verticalSizer->Add(topHorizontalSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  verticalSizer->Add(bottomHorizontalSizer, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  SetSizer(verticalSizer);

  verticalSizer->Fit(this);
  verticalSizer->SetSizeHints(this);

  CreateStatusBar();
  SetStatusText("Ready, press Start");

  Bind(wxEVT_BUTTON, &BusControllerFrame::onStartStopClicked, this, ID_START_STOP_BTN);
  Bind(wxEVT_MENU, &BusControllerFrame::onStartStopClicked, this, ID_START_STOP_MENU);
  Bind(wxEVT_MENU, &BusControllerFrame::onExit, this, wxID_EXIT);

  m_deviceIdTextInput->SetValue(std::to_string(m_bc.getDevNum()));
}

void BusControllerFrame::onStartStopClicked(wxCommandEvent & /*event*/) {
  S16BIT errorCode = 0;
  int deviceNum = 0;
}

void BusControllerFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }