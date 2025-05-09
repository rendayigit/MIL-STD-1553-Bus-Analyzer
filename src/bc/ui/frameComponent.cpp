#include "frameComponent.hpp"

#include <string>
#include <utility>

#include "bc.hpp"
#include "common.hpp"
#include "createFrameWindow.hpp"
#include "logger.hpp"

FrameComponent::FrameComponent(wxWindow *parent, const std::string &label, char bus, int rt, int rt2, int sa, int sa2,
                               int wc, BcMode mode, std::array<std::string, RT_SA_MAX_COUNT> data)
    : wxPanel(parent, wxID_ANY), m_mainWindow(dynamic_cast<BusControllerFrame *>(parent->GetParent())) {
  m_allText = new wxStaticText(this, wxID_ANY, "");

  updateValues(label, bus, rt, rt2, sa, sa2, wc, mode, std::move(data));

  auto *mainSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *orderSizer = new wxBoxSizer(wxVERTICAL);
  auto *repeatSendSizer = new wxBoxSizer(wxVERTICAL);

  std::string upImgOPath = getExecutableDirectory() + "../src/bc/icons/up_arrow.png";
  std::string downImgPath = getExecutableDirectory() + "../src/bc/icons/down_arrow.png";
  std::string removeImgPath = getExecutableDirectory() + "../src/bc/icons/remove.png";

  if (wxSystemSettingsNative::GetAppearance().IsDark()) {
    upImgOPath = getExecutableDirectory() + "../src/bc/icons/up_arrow.png";
    downImgPath = getExecutableDirectory() + "../src/bc/icons/down_arrow.png";
    removeImgPath = getExecutableDirectory() + "../src/bc/icons/remove.png";
  } else {
    upImgOPath = getExecutableDirectory() + "../src/bc/icons/up_arrow_dark.png";
    downImgPath = getExecutableDirectory() + "../src/bc/icons/down_arrow_dark.png";
    removeImgPath = getExecutableDirectory() + "../src/bc/icons/remove_dark.png";
  }

  wxBitmap upImg(upImgOPath, wxBITMAP_TYPE_PNG);
  wxBitmap downImg(downImgPath, wxBITMAP_TYPE_PNG);
  wxBitmap removeImg(removeImgPath, wxBITMAP_TYPE_PNG);

  auto *upButton =
      new wxBitmapButton(this, wxID_ANY, upImg, wxDefaultPosition,
                         wxSize(30, 30), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                         wxNO_BORDER);
  auto *downButton =
      new wxBitmapButton(this, wxID_ANY, downImg, wxDefaultPosition,
                         wxSize(30, 30), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                         wxNO_BORDER);
  auto *removeButton =
      new wxBitmapButton(this, wxID_ANY, removeImg, wxDefaultPosition,
                         wxSize(30, 30), // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                         wxNO_BORDER);

  upButton->SetBackgroundColour(wxTransparentColour);
  removeButton->SetBackgroundColour(wxTransparentColour);
  downButton->SetBackgroundColour(wxTransparentColour);

  m_activateToggle = new wxToggleButton(
      this, wxID_ANY, "Activate Frame", wxDefaultPosition,
      wxSize(120, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *editFrameButton = new wxButton(
      this, wxID_ANY, "Edit Frame", wxDefaultPosition,
      wxSize(120, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  auto *sendButton = new wxButton(
      this, wxID_ANY, "Send Frame", wxDefaultPosition,
      wxSize(120, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  sendButton->SetBackgroundColour(wxColour("#55ff55"));
  sendButton->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  orderSizer->Add(upButton, 0, wxALIGN_LEFT | wxALL, 0); // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
  orderSizer->Add(removeButton, 0, wxALIGN_LEFT | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                  0);
  orderSizer->Add(downButton, 0, wxALIGN_LEFT | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                  0);

  repeatSendSizer->Add(m_activateToggle, 0,
                       wxALIGN_LEFT | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                       5);                   // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  repeatSendSizer->Add(editFrameButton, 0,
                       wxALIGN_LEFT | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                       5);                   // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  repeatSendSizer->Add(sendButton, 0,
                       wxALIGN_LEFT | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                       5);                   // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  mainSizer->AddSpacer(15); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(orderSizer, 0,
                 wxALIGN_CENTER_VERTICAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                 0);
  mainSizer->AddSpacer(30); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(m_allText, 0,
                 wxALIGN_CENTER_VERTICAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  mainSizer->Add(repeatSendSizer, 0,
                 wxALIGN_CENTER_VERTICAL | wxALL, // NOLINT(bugprone-suspicious-enum-usage, hicpp-signed-bitwise)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  sendButton->Bind(wxEVT_BUTTON, &FrameComponent::onSend, this);
  removeButton->Bind(wxEVT_BUTTON, &FrameComponent::onRemove, this);
  editFrameButton->Bind(wxEVT_BUTTON, &FrameComponent::onEdit, this);
  m_activateToggle->Bind(wxEVT_TOGGLEBUTTON, &FrameComponent::onActivateToggle, this);
  upButton->Bind(wxEVT_BUTTON, &FrameComponent::onUp, this);
  downButton->Bind(wxEVT_BUTTON, &FrameComponent::onDown, this);

  SetBackgroundColour(this->GetBackgroundColour());
  SetSizer(mainSizer);
}

void FrameComponent::updateValues(const std::string &label, char bus, int rt, int rt2, int sa, int sa2, int wc,
                                  BcMode mode, std::array<std::string, RT_SA_MAX_COUNT> data) {
  m_label = label;
  m_bus = bus;
  m_rt = rt;
  m_rt2 = rt2;
  m_sa = sa;
  m_sa2 = sa2;
  m_wc = wc;
  m_mode = mode;
  m_data = data;

  std::string text = label + "\n\nBus: " + bus + "\tRT: " + std::to_string(rt) + "\tSA: " + std::to_string(sa) +
                     "\tWC: " + std::to_string(wc) + "\tMode: ";

  if (mode == BcMode::BC_TO_RT) {
    text += "BC->RT";
  } else if (mode == BcMode::RT_TO_BC) {
    text += "RT->BC";
  } else if (mode == BcMode::RT_TO_RT) {
    text += "RT->RT";
    text += "\n\t\tRT RX: " + std::to_string(rt2) + " \tSA RX: " + std::to_string(sa2);
  }

  text += "\nData: ";

  for (int i = 0; i < data.size(); ++i) {
    if (i % DATA_OCTET == 0) {
      text += "\n\t " + data.at(i);
    } else {
      text += "   " + data.at(i);
    }
  }

  m_allText->SetLabel(text);

  m_mainWindow->updateList();
}

void FrameComponent::sendFrame() {
  std::lock_guard<std::mutex> lock(m_mutex);
  wxTheApp->CallAfter([this] { // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    S16BIT status = ACE_ERR_SUCCESS;

    BC::getInstance().stop();
    BC::getInstance().start(m_mainWindow->getDeviceId());

    std::array<std::string, RT_SA_MAX_COUNT> data;

    if (m_mode == BcMode::BC_TO_RT) {
      status = BC::getInstance().bcToRt(m_rt, m_sa, m_wc, m_bus == 'A' ? ACE_BCCTRL_CHL_A : ACE_BCCTRL_CHL_B, m_data);
    } else if (m_mode == BcMode::RT_TO_BC) {
      status = BC::getInstance().rtToBc(m_rt, m_sa, m_wc, m_bus == 'A' ? ACE_BCCTRL_CHL_A : ACE_BCCTRL_CHL_B, &data);
      updateData(data);
    } else if (m_mode == BcMode::RT_TO_RT) {
      status = BC::getInstance().rtToRt(m_rt, m_sa, m_rt2, m_sa2, m_wc,
                                        m_bus == 'A' ? ACE_BCCTRL_CHL_A : ACE_BCCTRL_CHL_B, &data);
      updateData(data);
    }

    if (status != ACE_ERR_SUCCESS) {
      Logger::error("Error sending frame (" + m_label + "): " + getStatus(status));
      m_mainWindow->setStatusText("Error sending frame (" + m_label + "): " + getStatus(status));
    } else {
      Logger::debug("Sent frame (" + m_label + ")");
      m_mainWindow->setStatusText("Sent frame (" + m_label + ")");
    }
  });
}

void FrameComponent::updateData(std::array<std::string, RT_SA_MAX_COUNT> data) {
  updateValues(m_label, m_bus, m_rt, m_rt2, m_sa, m_sa2, m_wc, m_mode, std::move(data));
}

bool FrameComponent::isActive() { return m_activateToggle->GetValue(); }

void FrameComponent::onSend(wxCommandEvent & /*event*/) { sendFrame(); }

void FrameComponent::onRemove(wxCommandEvent & /*event*/) {
  this->Destroy();
  m_mainWindow->updateList();
}

void FrameComponent::onEdit(wxCommandEvent & /*event*/) {
  auto *frame = new FrameCreationFrame(m_mainWindow, this);
  frame->Show(true);
}

void FrameComponent::onActivateToggle(wxCommandEvent & /*event*/) {
  if (m_activateToggle->GetValue()) {
    m_activateToggle->SetLabel("Frame Active");
  } else {
    m_activateToggle->SetLabel("Activate Frame");
  }
}

void FrameComponent::onUp(wxCommandEvent & /*event*/) { m_mainWindow->moveUp(this); }

void FrameComponent::onDown(wxCommandEvent & /*event*/) { m_mainWindow->moveDown(this); }
