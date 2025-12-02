#include "app.hpp"

#include "bm/ui/mainWindow.hpp"
#include "config/config.hpp"

// Main function is set here
wxIMPLEMENT_APP(BusMonitorApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool BusMonitorApp::OnInit() {
  // Create config file if it doesn't exist
  Config::createConfigFileIfNotExists();

  auto *frame = new BusMonitorFrame();
  frame->Show(true);
  return true;
}