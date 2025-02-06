#include "frameComponent.hpp"

#include "bc.hpp"
#include "createFrameWindow.hpp"

FrameComponent::FrameComponent(wxWindow *parent, const std::string &label, char bus, int rt, int sa, int wc,
                               BcMode mode, std::array<std::string, RT_SA_MAX_COUNT> data)
    : wxPanel(parent, wxID_ANY), m_mainWindow(dynamic_cast<BusControllerFrame *>(parent->GetParent())) {
  m_allText = new wxStaticText(this, wxID_ANY, "");

  updateValues(label, bus, rt, sa, wc, mode, data);

  auto *mainSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *orderSizer = new wxBoxSizer(wxVERTICAL);
  auto *repeatSendSizer = new wxBoxSizer(wxVERTICAL);

  wxBitmap upImg(getExecutableDirectory() + "../src/bc/icons/up_arrow.png", wxBITMAP_TYPE_PNG);
  wxBitmap downImg(getExecutableDirectory() + "../src/bc/icons/down_arrow.png", wxBITMAP_TYPE_PNG);
  wxBitmap removeImg(getExecutableDirectory() + "../src/bc/icons/remove.png", wxBITMAP_TYPE_PNG);

  auto *upButton = new wxBitmapButton(this, wxID_ANY, upImg, wxDefaultPosition, wxSize(30, 30), wxNO_BORDER);
  auto *downButton = new wxBitmapButton(this, wxID_ANY, downImg, wxDefaultPosition, wxSize(30, 30), wxNO_BORDER);
  auto *removeButton = new wxBitmapButton(this, wxID_ANY, removeImg, wxDefaultPosition, wxSize(30, 30), wxNO_BORDER);

  upButton->SetBackgroundColour(wxTransparentColour);
  removeButton->SetBackgroundColour(wxTransparentColour);
  downButton->SetBackgroundColour(wxTransparentColour);

  m_activateToggle =
      new wxToggleButton(this, wxID_ANY, "Activate Frame", wxDefaultPosition, wxSize(120, TOP_BAR_COMP_HEIGHT));
  auto *editFrameButton =
      new wxButton(this, wxID_ANY, "Edit Frame", wxDefaultPosition, wxSize(120, TOP_BAR_COMP_HEIGHT));
  auto *sendButton = new wxButton(this, wxID_ANY, "Send Frame", wxDefaultPosition, wxSize(120, TOP_BAR_COMP_HEIGHT));

  orderSizer->Add(upButton, 0, wxALIGN_LEFT | wxALL, 0);
  orderSizer->Add(removeButton, 0, wxALIGN_LEFT | wxALL, 0);
  orderSizer->Add(downButton, 0, wxALIGN_LEFT | wxALL, 0);

  repeatSendSizer->Add(m_activateToggle, 0, wxALIGN_LEFT | wxALL, 5);
  repeatSendSizer->Add(editFrameButton, 0, wxALIGN_LEFT | wxALL, 5);
  repeatSendSizer->Add(sendButton, 0, wxALIGN_LEFT | wxALL, 5);

  mainSizer->AddSpacer(15);
  mainSizer->Add(orderSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  mainSizer->AddSpacer(30);
  mainSizer->Add(m_allText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  mainSizer->Add(repeatSendSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  sendButton->Bind(wxEVT_BUTTON, &FrameComponent::onSend, this);
  removeButton->Bind(wxEVT_BUTTON, &FrameComponent::onRemove, this);
  editFrameButton->Bind(wxEVT_BUTTON, &FrameComponent::onEdit, this);
  m_activateToggle->Bind(wxEVT_TOGGLEBUTTON, &FrameComponent::onActivateToggle, this);

  SetBackgroundColour(this->GetBackgroundColour());
  SetSizer(mainSizer);
}

void FrameComponent::updateValues(const std::string &label, char bus, int rt, int sa, int wc, BcMode mode,
                                  std::array<std::string, RT_SA_MAX_COUNT> data) {
  m_label = label;
  m_bus = bus;
  m_rt = rt;
  m_sa = sa;
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
  }

  text += "\nData: ";

  for (int i = 0; i < data.size(); ++i) {
    if (i % 8 == 0) {
      text += "\n\t " + data.at(i);
    } else {
      text += "   " + data.at(i);
    }
  }

  m_allText->SetLabel(text);
}

void FrameComponent::sendFrame() {
  S16BIT status = ACE_ERR_SUCCESS;

  BC::getInstance().stopBc();
  BC::getInstance().startBc(m_mainWindow->getDeviceId());

  std::array<std::string, RT_SA_MAX_COUNT> data;

  if (m_mode == BcMode::BC_TO_RT) {
    status = BC::getInstance().bcToRt(m_rt, m_sa, m_wc, m_bus == 'A' ? ACE_BCCTRL_CHL_A : ACE_BCCTRL_CHL_B, m_data);
  } else if (m_mode == BcMode::RT_TO_BC) {
    status = BC::getInstance().rtToBc(m_rt, m_sa, m_wc, m_bus == 'A' ? ACE_BCCTRL_CHL_A : ACE_BCCTRL_CHL_B, &data);
    updateData(data);
  } else if (m_mode == BcMode::RT_TO_RT) {
    // TODO: implement rt2 and sa 2
    status = BC::getInstance().rtToRt(m_rt, m_sa, 0, 0, m_wc, ACE_BCCTRL_CHL_A, &data);
    updateData(data);
  }

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    m_mainWindow->setStatusText("Error: " + getStatus(status));
  } else {
    Logger::debug("Sent frame: ");                    // TODO: improve log message
    m_mainWindow->setStatusText("Success sending: "); // TODO: improve log message
  }
}

void FrameComponent::updateData(std::array<std::string, RT_SA_MAX_COUNT> data) {
  updateValues(m_label, m_bus, m_rt, m_sa, m_wc, m_mode, data);
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

void FrameComponent::onActivateToggle(wxCommandEvent &event) {
  if (m_activateToggle->GetValue()) {
    m_activateToggle->SetLabel("Frame Active");
  } else {
    m_activateToggle->SetLabel("Activate Frame");
  }
}
