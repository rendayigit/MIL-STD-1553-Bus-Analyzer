#include "createFrameWindow.hpp"

#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "../rt.hpp"

FrameCreationFrame::FrameCreationFrame(wxWindow *parent, int rt, int sa, std::array<std::string, RT_SA_MAX_COUNT> data)
    : wxFrame(parent, wxID_ANY, "Editing buffer for RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa)),
      m_rt(rt), m_sa(sa) {
  createFrame();

  for (int i = 0; i < m_dataTextCtrls.size(); ++i) {
    m_dataTextCtrls.at(i)->SetValue(data.at(i));
  }
}

void FrameCreationFrame::createFrame() {
  m_mainSizer = new wxBoxSizer(wxVERTICAL);
  auto *middleSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer1 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer2 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer3 = new wxBoxSizer(wxHORIZONTAL);
  auto *dataSizer4 = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  m_saveButton = new wxButton(
      this, wxID_ANY, "Save Buffer", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *closeButton = new wxButton(
      this, wxID_ANY, "Cancel", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *randomizeButton = new wxButton(
      this, wxID_ANY, "Randomize Data", wxDefaultPosition,
      wxSize(120, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *dataLabel = new wxStaticText(this, wxID_ANY, "Data: ");

  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    auto *data = new wxTextCtrl(
        this, wxID_ANY, "", wxDefaultPosition,
        wxSize(70, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    data->SetHint("Data " + std::to_string(i));

    m_dataTextCtrls.push_back(data);
  }

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
  bottomSizer->Add(m_saveButton, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  m_mainSizer->Add(middleSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_mainSizer->Add(dataSizer1, 0,
                   wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  m_mainSizer->Add(dataSizer2, 0,
                   wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  m_mainSizer->Add(dataSizer3, 0,
                   wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  m_mainSizer->Add(dataSizer4, 0,
                   wxALIGN_CENTER_HORIZONTAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_mainSizer->AddStretchSpacer();
  m_mainSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                   5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  randomizeButton->Bind(wxEVT_BUTTON, &FrameCreationFrame::onRandomize, this);
  closeButton->Bind(wxEVT_BUTTON, &FrameCreationFrame::onClose, this);
  m_saveButton->Bind(wxEVT_BUTTON, &FrameCreationFrame::onSave, this);

  SetSizer(m_mainSizer);

  SetSize(700, 470); // NOLINT
}

void FrameCreationFrame::onSave(wxCommandEvent & /*event*/) {
  std::array<std::string, RT_SA_MAX_COUNT> data;

  for (int i = 0; i < data.size(); ++i) {
    data.at(i) = m_dataTextCtrls.at(i)->GetValue();
  }

  RT::getInstance().setRt(m_rt, m_sa, data);
}

void FrameCreationFrame::onRandomize(wxCommandEvent & /*event*/) {
  for (auto &dataTextCtrl : m_dataTextCtrls) {
    // Generate a random number between 0 and 65535
    std::random_device rd;                           // Obtain a random number from hardware
    std::mt19937 eng(rd());                          // Seed the generator
    std::uniform_int_distribution<> distr(0, 65535); // NOLINT

    int randomValue = distr(eng); // Generate the random number

    // Convert the random number to a hexadecimal string
    std::stringstream ss;
    ss << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << randomValue;

    dataTextCtrl->SetValue(ss.str());
  }
}

void FrameCreationFrame::onClose(wxCommandEvent & /*event*/) { Close(true); }
