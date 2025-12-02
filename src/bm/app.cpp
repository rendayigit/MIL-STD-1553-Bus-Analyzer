#include "app.hpp"

#include "bm/ui/mainWindow.hpp"

// Main function is set here
wxIMPLEMENT_APP(BusMonitorApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool BusMonitorApp::OnInit() {
  auto *frame = new BusMonitorFrame();
  frame->Show(true);
  return true;
}