#include "createFrameWindow.hpp"

#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "bcGuiCommon.hpp"
#include "common.hpp"
#include "mainWindow.hpp"

FrameCreationFrame::FrameCreationFrame(wxWindow *parent)
    : wxFrame(parent, wxID_ANY, "Create 1553 Frame"), m_parent(parent) {
  auto *mainSizer = new wxBoxSizer(wxVERTICAL);
  auto *topSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *middleSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer1 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer2 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer3 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer4 = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  auto *addButton = new wxButton(
      this, ID_ACCEPT_FRAME_BTN, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *closeButton = new wxButton(
      this, ID_CANCEL_FRAME_BTN, "Cancel", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *randomizeButton = new wxButton(
      this, ID_RANDOMIZE_DATA_BTN, "Randomize Data", wxDefaultPosition,
      wxSize(120, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  wxString rtSaWcOptions[] = // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
      {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11", "12", "13", "14", "15",
       "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};

  wxString busOptions[] = // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
      {"A", "B"};

  wxString modeOptions[] = // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
      {"BC->RT", "RT->BC", "RT->RT"};

  auto *busLabel = new wxStaticText(this, wxID_ANY, "Bus: ");

  m_busCombo = new wxComboBox(this, wxID_ANY, "A", wxDefaultPosition, wxDefaultSize, WXSIZEOF(busOptions), busOptions,
                              wxCB_READONLY);

  auto *rtLabel = new wxStaticText(this, wxID_ANY, "RT: ");

  m_rtCombo = new wxComboBox(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, WXSIZEOF(rtSaWcOptions),
                             rtSaWcOptions, wxCB_READONLY);

  auto *saLabel = new wxStaticText(this, wxID_ANY, "SA: ");

  m_saCombo = new wxComboBox(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, WXSIZEOF(rtSaWcOptions),
                             rtSaWcOptions, wxCB_READONLY);

  auto *wcLabel = new wxStaticText(this, wxID_ANY, "WC: ");

  m_wcCombo = new wxComboBox(this, ID_WC_COMBO, "0", wxDefaultPosition, wxDefaultSize, WXSIZEOF(rtSaWcOptions),
                             rtSaWcOptions, wxCB_READONLY);

  auto *modeLabel = new wxStaticText(this, wxID_ANY, "Mode: ");

  m_modeCombo = new wxComboBox(this, wxID_ANY, "BC->RT", wxDefaultPosition, wxDefaultSize, WXSIZEOF(modeOptions),
                               modeOptions, wxCB_READONLY);

  auto *dataLabel = new wxStaticText(this, wxID_ANY, "Data: ");

  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    auto *data = new wxTextCtrl(
        this, ID_DEVICE_ID_TXT, "", wxDefaultPosition,
        wxSize(70, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    data->SetHint("Data " + std::to_string(i));

    m_dataTextCtrls.push_back(data);
  }

  m_rtCombo->SetSelection(0);
  m_saCombo->SetSelection(0);

  topSizer->Add(busLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(m_busCombo, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                5);        // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->AddSpacer(20); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(rtLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(m_rtCombo, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                5);        // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->AddSpacer(20); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(saLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(m_saCombo, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                5);        // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->AddSpacer(20); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(wcLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(m_wcCombo, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                5);        // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->AddSpacer(20); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(modeLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topSizer->Add(m_modeCombo, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  middleSizer->Add(dataLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  middleSizer->AddStretchSpacer();

  int dataIndex = 0;

  for (; dataIndex < RT_SA_MAX_COUNT / 4; ++dataIndex) {
    dataSizer1->Add(m_dataTextCtrls.at(dataIndex), 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                    5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }

  for (; dataIndex < RT_SA_MAX_COUNT / 2; ++dataIndex) {
    dataSizer2->Add(m_dataTextCtrls.at(dataIndex), 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                    5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }

  for (; dataIndex < RT_SA_MAX_COUNT / 4 * 3; ++dataIndex) {
    dataSizer3->Add(m_dataTextCtrls.at(dataIndex), 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                    5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }

  for (; dataIndex < RT_SA_MAX_COUNT; ++dataIndex) {
    dataSizer4->Add(m_dataTextCtrls.at(dataIndex), 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                    5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }

  bottomSizer->Add(randomizeButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  bottomSizer->AddStretchSpacer();
  bottomSizer->Add(closeButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  bottomSizer->Add(addButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(topSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(middleSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  mainSizer->Add(dataSizer1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(dataSizer2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(dataSizer3, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(dataSizer4, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  mainSizer->AddStretchSpacer();
  mainSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  Bind(wxEVT_COMBOBOX, &FrameCreationFrame::onWcChanged, this, ID_WC_COMBO);
  Bind(wxEVT_BUTTON, &FrameCreationFrame::onAddClicked, this, ID_ACCEPT_FRAME_BTN);
  Bind(wxEVT_BUTTON, &FrameCreationFrame::onRandomize, this, ID_RANDOMIZE_DATA_BTN);
  Bind(wxEVT_BUTTON, &FrameCreationFrame::onClose, this, ID_CANCEL_FRAME_BTN);

  SetSizer(mainSizer);

  SetSize(650, 370);
}

void FrameCreationFrame::onAddClicked(wxCommandEvent & /*event*/) {
  auto *parentFrame = dynamic_cast<BusControllerFrame *>(m_parent);
  parentFrame->addFrameToList(m_busCombo->GetValue()[0], wxAtoi(m_rtCombo->GetValue()), wxAtoi(m_saCombo->GetValue()),
                              wxAtoi(m_wcCombo->GetValue()), wxAtoi(m_modeCombo->GetValue()), 0);
}

void FrameCreationFrame::onWcChanged(wxCommandEvent & /*event*/) {
  if (wxAtoi(m_wcCombo->GetValue()) == 0) {
    for (auto &dataTextCtrl : m_dataTextCtrls) {
      dataTextCtrl->Enable(true);
    }

    return;
  }

  for (auto &dataTextCtrl : m_dataTextCtrls) {
    dataTextCtrl->Enable(false);
  }

  for (int i = 0; i < wxAtoi(m_wcCombo->GetValue()); ++i) {
    m_dataTextCtrls.at(i)->Enable(true);
  }
}

void FrameCreationFrame::onRandomize(wxCommandEvent & /*event*/) {
  for (auto &dataTextCtrl : m_dataTextCtrls) {
    // Generate a random number between 0 and 65535
    std::random_device rd;                           // Obtain a random number from hardware
    std::mt19937 eng(rd());                          // Seed the generator
    std::uniform_int_distribution<> distr(0, 65535); // Define the range

    int randomValue = distr(eng); // Generate the random number

    // Convert the random number to a hexadecimal string
    std::stringstream ss;
    ss << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << randomValue;

    dataTextCtrl->SetValue(ss.str());
  }
}

void FrameCreationFrame::onClose(wxCommandEvent & /*event*/) { Close(true); }
