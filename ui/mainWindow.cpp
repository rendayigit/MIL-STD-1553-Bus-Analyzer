#include "mainWindow.hpp"
#include "common.hpp"
#include "milStd1553.hpp"
#include "wx/debug.h"
#include "wx/gdicmn.h"
#include "wx/gtk/button.h"
#include "wx/gtk/colour.h"
#include "wx/sizer.h"
#include <array>
#include <exception>
#include <iostream>
#include <string>

enum { ID_CONNECT_BTN = 1, ID_START_STOP_BTN, ID_FILTER_BTN, ID_DEVICE_ID_TXT, ID_RT_SA_TREE };

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Monitor") {
  m_uiRecentMessageCount = Json(CONFIG_PATH).getNode("UI_RECENT_LINE_COUNT").getValue<int>();

  auto *menuFile = new wxMenu;
  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&File");

  m_deviceIdTextInput =
      new wxTextCtrl(this, ID_DEVICE_ID_TXT, "0000",
                     wxPoint(10, 10), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                     wxSize(60, 50)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *connectButton =
      new wxButton(this, ID_CONNECT_BTN, "Connect",
                   wxPoint(75, 10),  // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                   wxSize(100, 50)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_startStopButton =
      new wxButton(this, ID_START_STOP_BTN, "Start",
                   wxPoint(185, 10), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                   wxSize(100, 50)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_startStopButton->Enable(false);

  m_filterButton =
      new wxButton(this, ID_FILTER_BTN, "No filter set, displaying all messages.",
                   wxPoint(340, 10), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                   wxSize(420, 50)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  m_filterButton->Enable(false);

  m_milStd1553Tree = new wxTreeCtrl(
      this, ID_RT_SA_TREE, wxPoint(10, 65), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
      wxSize(180, 195));                    // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto rtSaTreeRoot = m_milStd1553Tree->AddRoot("MIL-STD-1553");

  for (auto &bus : MilStd1553::getInstance().busList) {
    bus.setTreeObject(m_milStd1553Tree->AppendItem(rtSaTreeRoot, bus.getName()));

    for (auto &rt : bus.rtList) {
      rt.setTreeObject(m_milStd1553Tree->AppendItem(bus.getTreeObject(), rt.getName()));

      for (auto &sa : rt.saList) {
        sa.setTreeObject(m_milStd1553Tree->AppendItem(rt.getTreeObject(), sa.getName()));
      }
    }
  }

  m_milStd1553Tree->Expand(rtSaTreeRoot);
  m_milStd1553Tree->Expand(MilStd1553::getInstance().busList.at(0).getTreeObject());

  m_messageList = new wxTextCtrl(
      this, wxID_ANY, "", wxPoint(180, 10), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
      wxSize(250, 250),                     // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
      wxTE_READONLY | wxTE_MULTILINE);      // NOLINT(hicpp-signed-bitwise)

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(connectButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_startStopButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_filterButton, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  bottomHorizontalSizer->Add(m_milStd1553Tree, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                             5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  bottomHorizontalSizer->Add(m_messageList, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                             5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  verticalSizer->Add(topHorizontalSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  verticalSizer->Add(bottomHorizontalSizer, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                     5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  SetSizer(verticalSizer);

  verticalSizer->Fit(this);
  verticalSizer->SetSizeHints(this);

  CreateStatusBar();
  SetStatusText("Ready, press connect");

  Bind(wxEVT_BUTTON, &MyFrame::onConnectClicked, this, ID_CONNECT_BTN);
  Bind(wxEVT_BUTTON, &MyFrame::onStartStopClicked, this, ID_START_STOP_BTN);
  Bind(wxEVT_BUTTON, &MyFrame::onFilterClicked, this, ID_FILTER_BTN);
  Bind(wxEVT_MENU, &MyFrame::onExit, this, wxID_EXIT);
  m_milStd1553Tree->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::onSaClicked, this);

  m_deviceIdTextInput->SetValue(std::to_string(m_bm.getDevNum()));

  m_bm.setUpdateMessages([&](const std::string &text) {
    std::lock_guard<std::mutex> lock(m_mutex);
    wxTheApp->CallAfter([this, text] { // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
      wxString currentText = text + m_messageList->GetValue();
      wxArrayString lines = wxSplit(currentText, '\n');

      // If the number of lines exceeds limit, trim the excess lines
      while (lines.size() > m_uiRecentMessageCount) {
        lines.RemoveAt(lines.size() - 1, 1);
      }

      // Join the lines back together
      wxString newText = wxJoin(lines, '\n');
      m_messageList->SetValue(newText);

      // Auto-scroll to the end
      // m_messageList->ShowPosition(m_messageList->GetLastPosition());
    });
  });

  m_bm.setUpdateSaState([&](char bus, int rt, int sa, bool state) {
    std::lock_guard<std::mutex> lock(m_mutex);
    wxTheApp->CallAfter([this, bus, rt, sa, state] { // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).getTreeObject(), wxColour(state ? "green" : "red"));

      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).getTreeObject(),
          wxColour(state ? "green" : "red"));

      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).saList.at(sa).getTreeObject(),
          wxColour(state ? "green" : "red"));
    });
  });
}

void MyFrame::onConnectClicked(wxCommandEvent & /*event*/) {
  S16BIT errorCode = 0;
  int deviceNum = 0;
  m_deviceIdTextInput->GetValue().ToInt(&deviceNum);
  errorCode = m_bm.startBm(deviceNum);

  if (errorCode == 0) {
    SetStatusText("Connected to device " + std::to_string(deviceNum));
    m_startStopButton->Enable(true);
    m_startStopButton->SetLabelText("Stop");
    m_startStopButton->SetBackgroundColour(wxColour("#ff4545"));
    m_startStopButton->SetForegroundColour(wxColour("white"));
  } else {
    std::string errorString = getStatus(errorCode);
    SetStatusText(errorString.c_str());
    wxLogError(errorString.c_str());
  }
}

void MyFrame::onStartStopClicked(wxCommandEvent & /*event*/) {
  if (m_bm.isMonitoring()) {
    m_bm.stopBm();
    m_startStopButton->SetLabelText("Start");

    // Default background color is wxSYS_COLOUR_BACKGROUND
    // Default text color is wxSYS_COLOUR_WINDOWTEXT

    m_startStopButton->SetBackgroundColour(wxColour("#ffcc00"));
    m_startStopButton->SetForegroundColour(
        wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));
  } else {
    m_bm.startBm(m_bm.getDevNum());
    m_startStopButton->SetLabelText("Stop");
    m_startStopButton->SetBackgroundColour(wxColour("#ff4545"));
    m_startStopButton->SetForegroundColour(wxColour("white"));
  }
}

void MyFrame::onFilterClicked(wxCommandEvent & /*event*/) {
  m_bm.setFilter(false);
  m_filterButton->SetLabelText("No filter set, displaying all messages.");
  m_filterButton->Enable(false);
  m_filterButton->SetForegroundColour(wxColour("wxSYS_COLOUR_WINDOWTEXT"));
}

void MyFrame::onSaClicked(wxTreeEvent &event) {
  wxTreeItemId selectedItem = event.GetItem();

  wxString saText = m_milStd1553Tree->GetItemText(selectedItem);

  if (not saText.Contains("SA")) {
    return;
  }

  wxTreeItemId rtItem = m_milStd1553Tree->GetItemParent(selectedItem);
  wxString rtText = m_milStd1553Tree->IsEmpty() ? "No RT" : m_milStd1553Tree->GetItemText(rtItem);

  wxTreeItemId busItem = m_milStd1553Tree->GetItemParent(rtItem);
  wxString busText = m_milStd1553Tree->IsEmpty() ? "No Bus" : m_milStd1553Tree->GetItemText(busItem);

  wxString logMessage = wxString::Format("Filtering messages for: %s, %s, %s", busText, rtText, saText);

  wxLogMessage(logMessage);

  char bus = 'A';
  busText.GetChar(busText.size() - 1).GetAsChar(&bus);
  int rt = static_cast<int>(rtText.GetChar(rtText.size() - 1).GetValue() - '0');
  int sa = static_cast<int>(saText.GetChar(saText.size() - 1).GetValue() - '0');

  m_bm.setFilteredBus(bus);
  m_bm.setFilteredRt(rt);
  m_bm.setFilteredSa(sa);
  m_bm.setFilter(true);

  m_filterButton->SetLabelText(logMessage + ". Click to clear filter.");
  m_filterButton->Enable(true);
  m_filterButton->SetForegroundColour(wxColour("red"));

  m_messageList->Clear();
}

void MyFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }