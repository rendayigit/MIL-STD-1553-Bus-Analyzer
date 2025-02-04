#include "mainWindow.hpp"
#include "wx/debug.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/gtk/button.h"
#include "wx/gtk/colour.h"
#include "wx/gtk/stattext.h"
#include "wx/scrolwin.h"
#include "wx/sizer.h"
#include <nlohmann/json.hpp>
#include <string>

enum { ID_ADD_BTN = 1, ID_ADD_MENU, ID_DEVICE_ID_TXT };
constexpr int TOP_BAR_COMP_HEIGHT = 30;

class CustomComponent : public wxPanel {
public:
  CustomComponent(wxWindow *parent, const wxString &label) : wxPanel(parent, wxID_ANY) {
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lbl = new wxStaticText(this, wxID_ANY, label);
    wxButton *btn = new wxButton(this, wxID_ANY, "Button");
    sizer->Add(lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    SetSizer(sizer);
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
  SetStatusText("Ready, press Start");

  Bind(wxEVT_BUTTON, &BusControllerFrame::onAddClicked, this, ID_ADD_BTN);
  Bind(wxEVT_MENU, &BusControllerFrame::onAddClicked, this, ID_ADD_MENU);
  Bind(wxEVT_MENU, &BusControllerFrame::onExit, this, wxID_EXIT);

  m_deviceIdTextInput->SetValue(std::to_string(m_bc.getDevNum()));
}

void BusControllerFrame::onAddClicked(wxCommandEvent & /*event*/) {
  wxString label = m_deviceIdTextInput->GetValue();
  auto *component = new CustomComponent(m_scrolledWindow, label);
  m_scrolledSizer->Add(component, 0, wxEXPAND | wxALL, 5);
  m_scrolledWindow->FitInside(); // Update scrollable area
}

void BusControllerFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }