#include "app.hpp"
#include "mainWindow.hpp"

// Main function is set here
wxIMPLEMENT_APP(RtEmulatorApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool RtEmulatorApp::OnInit() {
  auto *frame = new RtEmulatorFrame();
  frame->Show(true);
  return true;
}