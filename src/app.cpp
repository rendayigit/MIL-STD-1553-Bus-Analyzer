#include "app.hpp"
#include "mainWindow.hpp"

// Main function is set here
wxIMPLEMENT_APP(MyApp); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

bool MyApp::OnInit() {
  auto *frame = new MyFrame();
  frame->Show(true);
  return true;
}