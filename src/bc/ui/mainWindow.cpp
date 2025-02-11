#include "mainWindow.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

#include <wx/wx.h>

#include "common.hpp"
#include "createFrameWindow.hpp"
#include "frameComponent.hpp"
#include "logger.hpp"

constexpr int MAX_FILE_PATH_SIZE = 1024;

BusControllerFrame::BusControllerFrame() : wxFrame(nullptr, wxID_ANY, "MIL-STD-1553 Bus Controller") {
  auto *menuFile = new wxMenu;
  menuFile->AppendSeparator();

  int addFrameId = wxNewId();
  menuFile->Append(addFrameId, "Add Frame\tCtrl-A", "Add a frame to the frame list");

  int clearFramesId = wxNewId();
  menuFile->Append(clearFramesId, "Clear All Frames\tCtrl-W", "Clear all frames from the frame list");

  int loadFramesId = wxNewId();
  menuFile->Append(loadFramesId, "Load frames\tCtrl-L", "Load frames from a file");

  int saveFramesId = wxNewId();
  menuFile->Append(saveFramesId, "Save frames\tCtrl-S", "Save frame into a file");

  menuFile->Append(wxID_EXIT);

  auto *menuBar = new wxMenuBar;
  SetMenuBar(menuBar);
  menuBar->Append(menuFile, "&Commands");

  auto *deviceIdLabel = new wxStaticText(this, wxID_ANY, "DDC Device ID");

  m_deviceIdTextInput = new wxTextCtrl(
      this, wxID_ANY, "00", wxDefaultPosition,
      wxSize(30, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_repeatToggle = new wxToggleButton(
      this, wxID_ANY, "Repeat Off", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_sendActiveFramesToggle = new wxToggleButton(
      this, wxID_ANY, "Send Active Frames", wxDefaultPosition,
      wxSize(170, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_sendActiveFramesToggle->SetBackgroundColour(wxColour("#00ccff"));
  m_sendActiveFramesToggle->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  m_addButton = new wxButton(
      this, wxID_ANY, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  m_addButton->SetBackgroundColour(wxColour("#ffcc00"));
  m_addButton->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));

  auto *verticalSizer = new wxBoxSizer(wxVERTICAL);
  auto *topHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

  // Replace bottomHorizontalSizer with scrolled window
  m_scrolledWindow = new wxScrolledWindow(this, wxID_ANY);
  m_scrolledWindow->SetBackgroundColour(this->GetBackgroundColour());
  m_scrolledSizer = new wxBoxSizer(wxVERTICAL); // NOLINT(cppcoreguidelines-prefer-member-initializer)
  m_scrolledWindow->SetSizer(m_scrolledSizer);

  // Set scroll speed
  m_scrolledWindow->SetScrollRate(10, 10); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  topHorizontalSizer->Add(deviceIdLabel, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_deviceIdTextInput, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddStretchSpacer();
  topHorizontalSizer->Add(m_repeatToggle, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5);       // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddSpacer(5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->Add(m_sendActiveFramesToggle, 0,
                          wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                          5);       // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  topHorizontalSizer->AddSpacer(5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
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

  Bind(wxEVT_MENU, &BusControllerFrame::onAddFrameClicked, this, addFrameId);
  Bind(wxEVT_MENU, &BusControllerFrame::onClearFramesClicked, this, clearFramesId);
  Bind(wxEVT_MENU, &BusControllerFrame::onLoadFrames, this, loadFramesId);
  Bind(wxEVT_MENU, &BusControllerFrame::onSaveFrames, this, saveFramesId);
  Bind(wxEVT_MENU, &BusControllerFrame::onExit, this, wxID_EXIT);

  m_addButton->Bind(wxEVT_BUTTON, &BusControllerFrame::onAddFrameClicked, this);
  m_repeatToggle->Bind(wxEVT_TOGGLEBUTTON, &BusControllerFrame::onRepeatToggle, this);
  m_sendActiveFramesToggle->Bind(wxEVT_TOGGLEBUTTON, &BusControllerFrame::onSendActiveFrames, this);

  m_deviceIdTextInput->SetValue("0");

  nlohmann::json config;

  // Load the JSON file
  std::ifstream configFile(CONFIG_PATH);
  if (not configFile.is_open()) {
    Logger::error("Could not open the config file: " + CONFIG_PATH);
  } else {
    // Parse the JSON file
    try {
      configFile >> config; // Parse the JSON file

      // Check if the Bus_Controller key exists and contains Default_Device_Number
      if (config.contains("Bus_Controller") and config["Bus_Controller"].contains("Default_Device_Number") and
          config["Bus_Controller"]["Default_Device_Number"].is_number_integer()) {
        m_deviceIdTextInput->SetValue(std::to_string(config["Bus_Controller"]["Default_Device_Number"].get<int>()));
      } else {
        Logger::error("Key 'Default_Device_Number' not found in 'Bus_Monitor' or is not an integer.");
      }
    } catch (const nlohmann::json::parse_error &e) {
      Logger::error("JSON parse error: " + std::string(e.what()));
    }
  }

  SetSize(650, 750); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

void BusControllerFrame::onAddFrameClicked(wxCommandEvent & /*event*/) {
  auto *frame = new FrameCreationFrame(this);
  frame->Show(true);
}

void BusControllerFrame::onClearFramesClicked(wxCommandEvent & /*event*/) { m_scrolledSizer->Clear(true); }

void BusControllerFrame::onRepeatToggle(wxCommandEvent & /*event*/) {
  if (m_repeatToggle->GetValue()) {
    m_repeatToggle->SetLabel("Repeat On");
  } else {
    m_repeatToggle->SetLabel("Repeat Off");
    stopSending();
  }
}

void BusControllerFrame::onSendActiveFrames(wxCommandEvent & /*event*/) {
  if (m_sendActiveFramesToggle->GetValue()) {
    m_sendActiveFramesToggle->SetLabel("Sending Active Frames");
    m_sendActiveFramesToggle->SetBackgroundColour(wxColour("#ff4545"));
    m_sendActiveFramesToggle->SetForegroundColour(wxColour("white"));

    if (m_repeatToggle->GetValue()) {
      startSendingThread();
    } else {
      sendActiveFrames();
      stopSending();
    }
  } else {
    stopSending();
  }
}

void BusControllerFrame::onLoadFrames(wxCommandEvent & /*event*/) {
  nlohmann::json framesJson;
  std::string framesJsonPath;

  try {
    char filename[MAX_FILE_PATH_SIZE];               // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                                     // cppcoreguidelines-avoid-c-arrays)
    FILE *f = popen("zenity --file-selection", "r"); // NOLINT (cert-env33-c)
    fgets(filename, MAX_FILE_PATH_SIZE, f);          // NOLINT (cert-err33-c)
    framesJsonPath = filename;
    framesJsonPath.pop_back(); // Remove last "\n"
  } catch (std::exception &e) {
    std::string logError = "Cannot load file: " + std::string(e.what());
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  // Load the JSON file
  std::ifstream jsonFile(framesJsonPath);
  if (not jsonFile.is_open()) {
    std::string logError = "Could not open file: " + framesJsonPath;
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  // Parse the JSON file
  try {
    jsonFile >> framesJson; // Parse the JSON file
  } catch (const nlohmann::json::parse_error &e) {
    std::string logError = "JSON parse error: " + std::string(e.what());
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  // Check if the json file contains the necessary keys
  if (not framesJson.contains("Frames")) {
    std::string logError = "Key 'Frames' not found in JSON file.";
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  if (framesJson["Frames"].empty()) {
    std::string logError = "No frames found in JSON file.";
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  for (auto &frame : framesJson["Frames"]) {
    if (frame.contains("Label") and frame["Label"].is_string() and frame.contains("Bus") and
        frame["Bus"].is_string() and frame.contains("Rt") and frame["Rt"].is_number_integer() and
        frame.contains("Sa") and frame["Sa"].is_number_integer() and frame.contains("Wc") and
        frame["Wc"].is_number_integer() and frame.contains("Mode") and frame["Mode"].is_string()) {

      auto mode = BcMode::BC_TO_RT;
      std::array<std::string, RT_SA_MAX_COUNT> data;
      data.fill("0000");

      int rt2 = 0;
      int sa2 = 0;

      if (frame["Mode"] == "BC->RT") {
        mode = BcMode::BC_TO_RT;

        if (frame.contains("Data") and frame["Data"].is_array()) {
          for (int i = 0; i < frame["Data"].size(); ++i) {
            data.at(i) = frame["Data"][i].get<std::string>();
          }
        } else {
          Logger::error("Data error from frame " + frame["Label"].get<std::string>() + ", skipping");
        }
      } else if (frame["Mode"] == "RT->BC") {
        mode = BcMode::RT_TO_BC;
      } else if (frame["Mode"] == "RT->RT") {
        mode = BcMode::RT_TO_RT;
        if (frame.contains("Rt2") and frame["Rt2"].is_number_integer() and frame.contains("Sa2") and
            frame["Sa2"].is_number_integer()) {
          rt2 = frame["Rt2"].get<int>();
          sa2 = frame["Sa2"].get<int>();
        } else {
          Logger::error("Rt2 or Sa2 error from frame " + frame["Label"].get<std::string>() + ", skipping");
        }

      } else {
        Logger::error("Invalid mode for frame " + frame["Label"].get<std::string>() + ", skipping");
        return;
      }

      auto *component = new FrameComponent(m_scrolledWindow, frame["Label"].get<std::string>(),
                                           frame["Bus"].get<std::string>()[0], frame["Rt"].get<int>(), rt2,
                                           frame["Sa"].get<int>(), sa2, frame["Wc"].get<int>(), mode, data);
      m_scrolledSizer->Add(component, 0, wxEXPAND | wxALL, // NOLINT(bugprone-suspicious-enum-usage)
                           5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

      updateList();
    } else {
      Logger::error("Invalid frame read from json, skipping frame");
      continue;
    }
  }

  SetStatusText("Loaded frames from " + framesJsonPath);
}

void BusControllerFrame::onSaveFrames(wxCommandEvent & /*event*/) {
  nlohmann::json framesJson;
  std::string framesJsonPath;

  for (auto &child : m_scrolledSizer->GetChildren()) {
    nlohmann::json frame;
    auto *frameComponent = dynamic_cast<FrameComponent *>(child->GetWindow());

    frame["Label"] = frameComponent->getLabel().ToStdString();
    frame["Bus"] = std::string(1, frameComponent->getBus());
    frame["Rt"] = frameComponent->getRt();
    frame["Sa"] = frameComponent->getSa();
    frame["Wc"] = frameComponent->getWc();

    if (frameComponent->getMode() == BcMode::BC_TO_RT) {
      frame["Mode"] = "BC->RT";

      for (auto &data : frameComponent->getData()) {
        frame["Data"].push_back(data);
      }
    } else if (frameComponent->getMode() == BcMode::RT_TO_BC) {
      frame["Mode"] = "RT->BC";
    } else {
      frame["Mode"] = "RT->RT";

      frame["Rt2"] = frameComponent->getRt2();
      frame["Sa2"] = frameComponent->getSa2();
    }

    framesJson["Frames"].push_back(frame);
  }

  try {
    char filename[MAX_FILE_PATH_SIZE]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
    // cppcoreguidelines-avoid-c-arrays)
    FILE *f = popen("zenity --file-selection --save", "r"); // NOLINT (cert-env33-c)
    fgets(filename, MAX_FILE_PATH_SIZE, f);                 // NOLINT (cert-err33-c)
    framesJsonPath = filename;
    framesJsonPath.pop_back(); // Remove last "\n"
  } catch (std::exception &e) {
    std::string logError = "Cannot save to file: " + std::string(e.what());
    Logger::error(logError);
    SetStatusText(logError);
    return;
  }

  std::ofstream file(framesJsonPath);

  if (file.is_open()) {
    file << framesJson.dump(2);
    file.close();

    wxString infoLog("Frames saved to " + framesJsonPath);
    SetStatusText(infoLog);
  } else {
    wxString errorLog("Could not open " + framesJsonPath + " for writing.");
    SetStatusText(errorLog);
    wxLogError(errorLog);
  }
}

void BusControllerFrame::onExit(wxCommandEvent & /*event*/) { Close(true); }

void BusControllerFrame::sendActiveFrames() {
  for (auto &child : m_scrolledSizer->GetChildren()) {
    auto *frame = dynamic_cast<FrameComponent *>(child->GetWindow());

    if (frame != nullptr and frame->isActive()) {
      frame->sendFrame();

      std::this_thread::sleep_for(
          std::chrono::milliseconds(100)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
  }
}

void BusControllerFrame::startSendingThread() {
  try {
    m_isSending = true;
    m_repeatedSendThread = std::thread([&] {
      while (m_isSending) {
        sendActiveFrames();
      }
    });
  } catch (std::exception &e) {
    Logger::error("Failed to start sending thread, " + std::string(e.what()));
  }
}

void BusControllerFrame::stopSending() {
  m_isSending = false;
  if (m_repeatedSendThread.joinable()) {
    m_repeatedSendThread.join();
  }

  m_sendActiveFramesToggle->SetValue(false);
  m_sendActiveFramesToggle->SetLabel("Send Active Frames");
  m_sendActiveFramesToggle->SetBackgroundColour(wxColour("#00ccff"));
  m_sendActiveFramesToggle->SetForegroundColour(
      wxColour(wxSystemSettingsNative::GetAppearance().IsDark() ? "black" : "wxSYS_COLOUR_WINDOWTEXT"));
}

void BusControllerFrame::setStatusText(const wxString &status) { SetStatusText(status); }

int BusControllerFrame::getDeviceId() { return wxAtoi(m_deviceIdTextInput->GetValue()); }

void BusControllerFrame::moveUp(FrameComponent *item) {
  int index = getFrameIndex(item);

  if (index == 0) {
    return;
  }

  m_scrolledSizer->Remove(index);
  m_scrolledSizer->Insert(index - 1, item, 0, wxEXPAND | wxALL, // NOLINT (bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  updateList();
}

void BusControllerFrame::moveDown(FrameComponent *item) {
  int index = getFrameIndex(item);

  if (index == m_scrolledSizer->GetItemCount() - 1) {
    return;
  }

  m_scrolledSizer->Remove(index);
  m_scrolledSizer->Insert(index + 1, item, 0, wxEXPAND | wxALL, // NOLINT (bugprone-suspicious-enum-usage)
                          5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  updateList();
}

void BusControllerFrame::updateList() {
  m_scrolledWindow->FitInside(); // Update scrollable area
}

int BusControllerFrame::getFrameIndex(FrameComponent *frame) {
  auto *item = m_scrolledSizer->GetItem(frame);

  for (int i = 0; i < m_scrolledSizer->GetItemCount(); ++i) {
    if (m_scrolledSizer->GetItem(i) == item) {
      return i;
    }
  }

  return -1;
}

void BusControllerFrame::addFrameToList(const std::string &label, char bus, int rt, int rt2, int sa, int sa2, int wc,
                                        BcMode mode, std::array<std::string, RT_SA_MAX_COUNT> data) {
  auto *component = new FrameComponent(m_scrolledWindow, label, bus, rt, rt2, sa, sa2, wc, mode, std::move(data));
  m_scrolledSizer->Add(component, 0, wxEXPAND | wxALL, // NOLINT (bugprone-suspicious-enum-usage)
                       5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  updateList();
}
