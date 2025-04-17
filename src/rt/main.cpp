#include "rt.hpp"

#include <iostream>

int main(int /*argc*/, char ** /*argv*/) {
  RT::getInstance().start(0);

  // RT::getInstance().setRt(1, 1, 5, ACE_BCCTRL_CHL_A, { "AAAA", "BBBB", "CCCC", "1234", "FFFF" });
  RT::getInstance().activateRt(1, 1);

  // Wait for user input to stop
  std::cout << "\nPress enter to stop\n\n";
  char input = 0;
  while (input != '\n') {
    std::cin.get(input);
  }

  return 0;
}