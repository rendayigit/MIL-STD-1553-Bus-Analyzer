#include "app.hpp"
#include "mainWindow.hpp"

// Main function is set here
wxIMPLEMENT_APP(BusControllerApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool BusControllerApp::OnInit() {
  auto *frame = new BusControllerFrame();
  frame->Show(true);
  return true;
}