
#include "createFrameWindow.hpp"
#include "bcGuiCommon.hpp"

FrameCreationFrame::FrameCreationFrame(wxWindow *parent) : wxFrame(parent, wxID_ANY, "Create 1553 Frame") {
  auto *mainSizer = new wxBoxSizer(wxHORIZONTAL);

  auto *addButton = new wxButton(
      this, ID_ACCEPT_FRAME_BTN, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  mainSizer->Add(addButton, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  SetSizer(mainSizer);
}
