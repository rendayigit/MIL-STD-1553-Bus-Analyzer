#include "mainWindow.hpp"
#include "common.hpp"
#include "milStd1553.hpp"
#include "wx/sizer.h"
#include <array>
#include <exception>
#include <iostream>
#include <string>

enum { ID_CONNECT_BTN = 1, ID_DEVICE_ID_TXT, ID_RT_SA_TREE };

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Monitor", wxPoint(250, 250), wxSize(440, 340)) {
  m_uiRecentMessageCount = Json(CONFIG_PATH).getNode("UI_RECENT_MESSAGE_COUNT").getValue<int>();

  auto *menuFile = new wxMenu;
  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&File");

  m_deviceIdTextInput = new wxTextCtrl(this, ID_DEVICE_ID_TXT, "0000", wxPoint(10, 10), wxSize(60, 50));

  auto *connectButton = new wxButton(this, ID_CONNECT_BTN, "Connect", wxPoint(75, 10), wxSize(100, 50));

  m_milStd1553Tree = new wxTreeCtrl(this, ID_RT_SA_TREE, wxPoint(10, 65), wxSize(180, 195));

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

  m_messageList =
      new wxTextCtrl(this, wxID_ANY, "", wxPoint(180, 10), wxSize(250, 250), wxTE_READONLY | wxTE_MULTILINE);

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, 5);
  topHorizontalSizer->Add(connectButton, 0, wxEXPAND | wxALL, 5);

  bottomHorizontalSizer->Add(m_milStd1553Tree, 0, wxEXPAND | wxALL, 5);
  bottomHorizontalSizer->Add(m_messageList, 1, wxEXPAND | wxALL, 5);

  verticalSizer->Add(topHorizontalSizer, 0, wxEXPAND | wxALL, 5);
  verticalSizer->Add(bottomHorizontalSizer, 1, wxEXPAND | wxALL, 5);

  SetSizer(verticalSizer);

  verticalSizer->Fit(this);
  verticalSizer->SetSizeHints(this);

  CreateStatusBar();
  SetStatusText("Ready, press connect");

  Bind(wxEVT_BUTTON, &MyFrame::onConnectClicked, this, ID_CONNECT_BTN);
  Bind(wxEVT_MENU, &MyFrame::onExit, this, wxID_EXIT);
  m_milStd1553Tree->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::onSaClicked, this);

  m_deviceIdTextInput->SetValue(std::to_string(m_bm.getDevNum()));

  m_bm.setUpdateFilter([&](const std::string &text) {
    // TODO(renda): implement filter
    wxLogMessage("Filter set to: %s", text.c_str());
  });

  m_bm.setUpdateMessages([&](const std::string &text) {
    std::lock_guard<std::mutex> lock(m_mutex);
    wxTheApp->CallAfter([this, text] {
      wxString currentText = text + m_messageList->GetValue();
      wxArrayString lines = wxSplit(currentText, '\n');

      // If the number of lines exceeds limit, trim the excess lines
      if (lines.size() > m_uiRecentMessageCount) {
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
    wxTheApp->CallAfter([this, bus, rt, sa, state] {
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
  } else {
    SetStatusText(getStatus(errorCode).c_str());
  }
}

void MyFrame::onSaClicked(wxTreeEvent &event) {
  wxTreeItemId selectedItem = event.GetItem();

  // Retrieve the SA (Sub-Address) item text
  wxString saText = m_milStd1553Tree->GetItemText(selectedItem);

  // Retrieve the parent item of the selected item (which should be the RT)
  wxTreeItemId rtItem = m_milStd1553Tree->GetItemParent(selectedItem);

  // Check if the parent exists and retrieve its text
  wxString rtText = m_milStd1553Tree->IsEmpty() ? "No RT" : m_milStd1553Tree->GetItemText(rtItem);

  // Log both RT and SA information
  wxLogMessage("Selected item: %s, %s", rtText, saText);

  // TODO(renda): implement filter
}

void MyFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }