#include "mainWindow.hpp"

#include <nlohmann/json.hpp>
#include <string>

#include <wx/debug.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/gtk/button.h>
#include <wx/gtk/colour.h>
#include <wx/gtk/stattext.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "bcGuiCommon.hpp"
#include "createFrameWindow.hpp"

class CustomComponent : public wxPanel {
public:
  CustomComponent(wxWindow *parent, const std::string &name, char bus, int rt, int sa, int wc, int mode)
      : wxPanel(parent, wxID_ANY) {
    std::string text = name + "\nBus: " + bus + "\tRT: " + std::to_string(rt) + "\tSA: " + std::to_string(sa) +
                       "\tWC: " + std::to_string(wc) + "\tMode: " + std::to_string(mode) +
                       "\nData: 0000 0000 0000 0000 0000 0000 0000 0000\n" +
                       "\t 0000 0000 0000 0000 0000 0000 0000 0000\n" + "\t 0000 0000 0000 0000 0000 0000 0000 0000\n" +
                       "\t 0000 0000 0000 0000 0000 0000 0000 0000\n";

    auto *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    auto *orderSizer = new wxBoxSizer(wxVERTICAL);
    auto *repeatSendSizer = new wxBoxSizer(wxVERTICAL);

    auto *upButton = new wxButton(this, wxID_ANY, "^", wxDefaultPosition, wxSize(50, TOP_BAR_COMP_HEIGHT));
    auto *downButton = new wxButton(this, wxID_ANY, "v", wxDefaultPosition, wxSize(50, TOP_BAR_COMP_HEIGHT));
    auto *nameLabel = new wxStaticText(this, wxID_ANY, text);
    auto *activeToggle = new wxToggleButton(this, wxID_ANY, "Frame Active");
    auto *sendButton = new wxButton(this, wxID_ANY, "Send Single");

    orderSizer->Add(upButton, 0, wxALIGN_LEFT | wxALL, 5);
    orderSizer->Add(downButton, 0, wxALIGN_LEFT | wxALL, 5);

    repeatSendSizer->Add(activeToggle, 0, wxALIGN_LEFT | wxALL, 5);
    repeatSendSizer->Add(sendButton, 0, wxALIGN_LEFT | wxALL, 5);

    mainSizer->Add(orderSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    mainSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    mainSizer->Add(repeatSendSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    SetSizer(mainSizer);
  }
};

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

  auto *repeatToggle = new wxToggleButton(this, wxID_ANY, "Repeated Send");

  auto *sendActiveFramesButton = new wxButton(
      this, ID_SEND_ACTIVE_BTN, "Send Active Frames", wxDefaultPosition,
      wxSize(150, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_addButton = new wxButton(
      this, ID_ADD_BTN, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_addButton->SetBackgroundColour(wxColour("#ffcc00"));
  m_addButton->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  // Replace bottomHorizontalSizer with scrolled window
  m_scrolledWindow = new wxScrolledWindow(this, wxID_ANY);
  m_scrolledWindow->SetBackgroundColour(wxColour(0, 0, 0));
  m_scrolledSizer = new wxBoxSizer(wxVERTICAL);
  m_scrolledWindow->SetSizer(m_scrolledSizer);
  m_scrolledWindow->SetScrollRate(10, 10); // Set scroll speed

  topHorizontalSizer->Add(deviceIdText, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddStretchSpacer();
  topHorizontalSizer->Add(repeatToggle, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(sendActiveFramesButton, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
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

  Bind(wxEVT_BUTTON, &BusControllerFrame::onAddClicked, this, ID_ADD_BTN);
  Bind(wxEVT_MENU, &BusControllerFrame::onAddClicked, this, ID_ADD_MENU);
  Bind(wxEVT_MENU, &BusControllerFrame::onExit, this, wxID_EXIT);

  m_deviceIdTextInput->SetValue(std::to_string(m_bc.getDevNum()));
  SetSize(600, 400);
}

void BusControllerFrame::onAddClicked(wxCommandEvent & /*event*/) {
  auto *component = new CustomComponent(m_scrolledWindow, "Sample Message", 'A', 1, 1, 8, 0);
  m_scrolledSizer->Add(component, 0, wxEXPAND | wxALL, 5);
  m_scrolledWindow->FitInside(); // Update scrollable area

  auto *frame = new FrameCreationFrame(this);
  frame->Show(true);
}

void BusControllerFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }