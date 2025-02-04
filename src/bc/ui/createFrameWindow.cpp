
#include "createFrameWindow.hpp"

enum {
  ID_ADD_BTN = 1, // TODO(renda): This enum must be moved to a common file or the IDs overlap with the other IDs.
};

constexpr int TOP_BAR_COMP_HEIGHT = 30; // TODO(renda): Consider moving to a common file

FrameCreationFrame::FrameCreationFrame(wxWindow *parent) : wxFrame(parent, wxID_ANY, "Create 1553 Frame") {
  auto *mainSizer = new wxBoxSizer(wxHORIZONTAL);

  auto *addButton = new wxButton(
      this, ID_ADD_BTN, "Add Frame", wxDefaultPosition,
      wxSize(100, TOP_BAR_COMP_HEIGHT)); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  mainSizer->Add(addButton, 0, wxALIGN_CENTER_VERTICAL, // NOLINT(bugprone-suspicious-enum-usage)
                 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

  SetSizer(mainSizer);
}
