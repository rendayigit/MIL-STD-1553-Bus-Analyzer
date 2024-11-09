#include "mainWindow.hpp"
#include "common.hpp"
#include "wx/sizer.h"
#include <array>
#include <exception>
#include <iostream>
#include <string>

enum { ID_CONNECT_BTN = 1, ID_DEVICE_ID_TXT, ID_RT_SA_TREE };

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Monitor", wxPoint(250, 250),
              wxSize(440, 340)) {
  auto *menuFile = new wxMenu;
  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&File");

  auto *deviceIdTextInput = new wxTextCtrl(this, ID_DEVICE_ID_TXT, "0000",
                                           wxPoint(10, 10), wxSize(60, 50));

  auto *connectButton = new wxButton(this, ID_CONNECT_BTN, "Connect",
                                     wxPoint(75, 10), wxSize(100, 50));

  rtSaTree =
      new wxTreeCtrl(this, ID_RT_SA_TREE, wxPoint(10, 65), wxSize(165, 195));

  auto rtSaTreeRoot = rtSaTree->AddRoot("BUS A");

  for (int i = 0; i < 32; i++) {
    wxTreeItemId rt =
        rtSaTree->AppendItem(rtSaTreeRoot, "RT " + std::to_string(i));
    rtTreeArray[i] = rt;
    std::array<wxTreeItemId, 32> saArray;

    for (int j = 0; j < 32; j++) {
      wxTreeItemId sa = rtSaTree->AppendItem(rt, "SA " + std::to_string(j));

      saArray[j] = sa;
    }

    saTreeArray[i] = saArray;
  }

  rtSaTree->Expand(rtSaTreeRoot);

  messageList =
      new wxTextCtrl(this, wxID_ANY, "", wxPoint(180, 10), wxSize(250, 250),
                     wxTE_READONLY | wxTE_MULTILINE);
  messageList->Enable(false);

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *bottomHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  topHorizontalSizer->Add(deviceIdTextInput, 0, wxEXPAND | wxALL, 5);
  topHorizontalSizer->Add(connectButton, 0, wxEXPAND | wxALL, 5);

  bottomHorizontalSizer->Add(rtSaTree, 0, wxEXPAND | wxALL, 5);
  bottomHorizontalSizer->Add(messageList, 1, wxEXPAND | wxALL, 5);

  verticalSizer->Add(topHorizontalSizer, 0, wxEXPAND | wxALL, 5);
  verticalSizer->Add(bottomHorizontalSizer, 1, wxEXPAND | wxALL, 5);

  SetSizer(verticalSizer);

  verticalSizer->Fit(this);
  verticalSizer->SetSizeHints(this);

  CreateStatusBar();
  SetStatusText("Ready, press connect");

  Bind(wxEVT_BUTTON, &MyFrame::onConnectClicked, this, ID_CONNECT_BTN);
  Bind(wxEVT_MENU, &MyFrame::onExit, this, wxID_EXIT);
  rtSaTree->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::onSaClicked, this);

  bm.setUpdateFilter([&](const std::string &text) {
    // TODO(renda): implement
    wxLogMessage("Filter set to: %s", text.c_str());
  });

  bm.setUpdateMessages([&](const std::string &text) {
    wxTheApp->CallAfter([this, text] {
      std::lock_guard<std::mutex> lock(m_mutex);

      try {
        // messageList->SetValue(text);
        // messageList->AppendText(text);

        wxString currentText = messageList->GetValue() + text;
        wxArrayString lines = wxSplit(currentText, '\n');

        // If the number of lines exceeds 50, trim the excess lines
        if (lines.size() > 50) {
          lines.RemoveAt(0, lines.size() - 50); // Keep only the last 50 lines
        }

        // Join the lines back together
        wxString newText = wxJoin(lines, '\n');
        messageList->SetValue(newText);

        // Auto-scroll to the end
        messageList->ShowPosition(messageList->GetLastPosition());
      } catch (std::exception &e) {
        // ignore
      }
    });
  });

  bm.setUpdateSaState([&](int rt, int sa, bool state) {
    if (state) {
      rtSaTree->SetItemBackgroundColour(rtTreeArray[rt], wxColour("green"));
      rtSaTree->SetItemBackgroundColour(saTreeArray[rt][sa], wxColour("green"));
    } else {
      rtSaTree->SetItemBackgroundColour(rtTreeArray[rt], wxColour("red"));
      rtSaTree->SetItemBackgroundColour(saTreeArray[rt][sa], wxColour("red"));
    }
  });
}

void MyFrame::onConnectClicked(wxCommandEvent & /*event*/) {
  S16BIT errorCode = 0;
  S16BIT deviceNum = 0; // TODO(renda): get from ui
  errorCode = bm.startBm(deviceNum);

  if (errorCode == 0) {
    SetStatusText("Connected to device " + std::to_string(deviceNum));
  } else {
    SetStatusText(getStatus(errorCode).c_str());
  }
}

void MyFrame::onSaClicked(wxTreeEvent &event) {
  wxTreeItemId selectedItem = event.GetItem();

  // Retrieve the SA (Sub-Address) item text
  wxString saText = rtSaTree->GetItemText(selectedItem);

  // Retrieve the parent item of the selected item (which should be the RT)
  wxTreeItemId rtItem = rtSaTree->GetItemParent(selectedItem);

  // Check if the parent exists and retrieve its text
  wxString rtText =
      rtSaTree->IsEmpty() ? "No RT" : rtSaTree->GetItemText(rtItem);

  // Log both RT and SA information
  wxLogMessage("Selected item: %s, %s", rtText, saText);

  // TODO(renda): implement filter
}

void MyFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }