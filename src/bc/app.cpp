#include "app.hpp"

#include "bc/ui/mainWindow.hpp"
#include "config/config.hpp"

// Main function is set here
wxIMPLEMENT_APP(BusControllerApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool BusControllerApp::OnInit() {
  // Create config file if it doesn't exist
  Config::createConfigFileIfNotExists();

  auto *frame = new BusControllerFrame();
  frame->Show(true);
  return true;
}