#include "mainWindow.hpp"
#include "common.hpp"
#include "milStd1553.hpp"
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
  ID_FILTER_BTN,
  ID_CLEAR_BTN,
  ID_FILTER_MENU,
  ID_CLEAR_MENU,
  ID_DEVICE_ID_TXT,
  ID_RT_SA_TREE
};
constexpr int TOP_BAR_COMP_HEIGHT = 30;

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Monitor"), m_uiRecentMessageCount(getMaxRecentLineCount()) {

  auto *menuFile = new wxMenu;
  menuFile->Append(ID_START_STOP_MENU, "Start / Stop\tCtrl-R", "Start or stop monitoring on selected DDC device");
  menuFile->Append(ID_FILTER_MENU, "Clear filter\tCtrl-F", "Clear filtering of messages");
  menuFile->Append(ID_CLEAR_MENU, "Clear messages\tCtrl-M", "Clear messages");
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

  m_filterButton = new wxButton(
      this, ID_FILTER_BTN, "No filter set, displaying all messages.", wxDefaultPosition,
      wxSize(-1, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_filterButton->Enable(false);

  auto *clearButton = new wxButton(
      this, ID_CLEAR_BTN, "Clear", wxDefaultPosition,
      wxSize(-1, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_milStd1553Tree =
      new wxTreeCtrl(this, ID_RT_SA_TREE, wxDefaultPosition,
                     wxSize(180, 200)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

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

  m_messageList = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE); // NOLINT(hicpp-signed-bitwise)

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  topHorizontalSizer->Add(deviceIdText, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_startStopButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_filterButton, 1, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(clearButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
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
  SetStatusText("Ready, press Start");

  Bind(wxEVT_BUTTON, &MyFrame::onStartStopClicked, this, ID_START_STOP_BTN);
  Bind(wxEVT_MENU, &MyFrame::onStartStopClicked, this, ID_START_STOP_MENU);
  Bind(wxEVT_BUTTON, &MyFrame::onClearFilterClicked, this, ID_FILTER_BTN);
  Bind(wxEVT_BUTTON, &MyFrame::onClearClicked, this, ID_CLEAR_BTN);
  Bind(wxEVT_MENU, &MyFrame::onClearFilterClicked, this, ID_FILTER_MENU);
  Bind(wxEVT_MENU, &MyFrame::onClearClicked, this, ID_CLEAR_MENU);
  Bind(wxEVT_MENU, &MyFrame::onExit, this, wxID_EXIT);
  m_milStd1553Tree->Bind(wxEVT_TREE_ITEM_ACTIVATED, &MyFrame::onTreeItemClicked, this);

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
      // Default background color is wxSYS_COLOUR_BACKGROUND
      // Default text color is wxSYS_COLOUR_WINDOWTEXT

      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).getTreeObject(), wxColour(state ? "green" : "red"));

      m_milStd1553Tree->SetItemTextColour(MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).getTreeObject(),
                                          wxColour(state ? "white" : "wxSYS_COLOUR_WINDOWTEXT"));

      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).getTreeObject(),
          wxColour(state ? "green" : "red"));

      m_milStd1553Tree->SetItemTextColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).getTreeObject(),
          wxColour(state ? "white" : "wxSYS_COLOUR_WINDOWTEXT"));

      m_milStd1553Tree->SetItemBackgroundColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).saList.at(sa).getTreeObject(),
          wxColour(state ? "green" : "red"));

      m_milStd1553Tree->SetItemTextColour(
          MilStd1553::getInstance().busList.at(bus == 'A' ? 0 : 1).rtList.at(rt).saList.at(sa).getTreeObject(),
          wxColour(state ? "white" : "wxSYS_COLOUR_WINDOWTEXT"));
    });
  });
}

void MyFrame::onStartStopClicked(wxCommandEvent & /*event*/) {
  S16BIT errorCode = 0;
  int deviceNum = 0;

  if (m_bm.isMonitoring()) {
    m_bm.stop();
    m_startStopButton->SetLabelText("Start");

    // Default background color is wxSYS_COLOUR_BACKGROUND
    // Default text color is wxSYS_COLOUR_WINDOWTEXT

    m_startStopButton->SetBackgroundColour(wxColour("#ffcc00"));
    m_startStopButton->SetForegroundColour(
        wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));
  } else {
    m_deviceIdTextInput->GetValue().ToInt(&deviceNum);
    errorCode = m_bm.start(deviceNum);

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
}

void MyFrame::onClearFilterClicked(wxCommandEvent & /*event*/) {
  if (not m_bm.isFiltered()) {
    return;
  }

  m_bm.setFilter(false);
  m_filterButton->SetLabelText("No filter set, displaying all messages.");
  m_filterButton->Enable(false);
  m_filterButton->SetForegroundColour(wxColour("wxSYS_COLOUR_WINDOWTEXT"));
}

void MyFrame::onClearClicked(wxCommandEvent & /*event*/) { m_messageList->Clear(); }

void MyFrame::onTreeItemClicked(wxTreeEvent &event) {
  wxTreeItemId selectedItem = event.GetItem();

  wxString selectedItemText = m_milStd1553Tree->GetItemText(selectedItem);

  wxString logMessage;

  // SA selected
  if (selectedItemText.Contains("SA")) {
    wxTreeItemId rtItem = m_milStd1553Tree->GetItemParent(selectedItem);
    wxString rtText = m_milStd1553Tree->IsEmpty() ? "No RT" : m_milStd1553Tree->GetItemText(rtItem);

    wxTreeItemId busItem = m_milStd1553Tree->GetItemParent(rtItem);
    wxString busText = m_milStd1553Tree->IsEmpty() ? "No Bus" : m_milStd1553Tree->GetItemText(busItem);

    logMessage = busText + ", " + rtText + ", " + selectedItemText;

    char bus = 'A';
    busText.GetChar(busText.size() - 1).GetAsChar(&bus);

    std::regex digitRegex("\\d+");
    std::smatch match;
    std::string saString = selectedItemText.ToStdString();
    std::string rtString = rtText.ToStdString();

    int rt = 0;
    int sa = 0;

    if (std::regex_search(saString, match, digitRegex)) {
      std::string numericPart = match[0];
      sa = std::stoi(numericPart);
    }

    if (std::regex_search(rtString, match, digitRegex)) {
      std::string numericPart = match[0];
      rt = std::stoi(numericPart);
    }

    m_bm.setFilteredBus(bus);
    m_bm.setFilteredRt(rt);
    m_bm.setFilteredSa(sa);
  }
  // RT selected
  else if (selectedItemText.Contains("RT")) {
    wxTreeItemId busItem = m_milStd1553Tree->GetItemParent(selectedItem);
    wxString busText = m_milStd1553Tree->IsEmpty() ? "No Bus" : m_milStd1553Tree->GetItemText(busItem);

    logMessage = busText + ", " + selectedItemText;

    char bus = 'A';
    busText.GetChar(busText.size() - 1).GetAsChar(&bus);

    std::regex digitRegex("\\d+");
    std::smatch match;
    std::string rtString = selectedItemText.ToStdString();

    int rt = 0;

    if (std::regex_search(rtString, match, digitRegex)) {
      std::string numericPart = match[0];
      rt = std::stoi(numericPart);
    }

    m_bm.setFilteredBus(bus);
    m_bm.setFilteredRt(rt);
    m_bm.setFilteredSa(-1);
  }
  // BUS selected
  else if (selectedItemText.Contains("BUS")) {
    logMessage = selectedItemText;

    char bus = 'A';
    selectedItemText.GetChar(selectedItemText.size() - 1).GetAsChar(&bus);

    m_bm.setFilteredBus(bus);
    m_bm.setFilteredRt(-1);
    m_bm.setFilteredSa(-1);
  } else {
    return;
  }

  m_bm.setFilter(true);

  m_filterButton->SetLabelText("Filtering: " + logMessage + ", Clear filter?");
  m_filterButton->Enable(true);
  m_filterButton->SetForegroundColour(wxColour("red"));

  m_messageList->Clear();
}

void MyFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }