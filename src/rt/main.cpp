#include "rt.hpp"

#include <array>
#include <iostream>

int main(int /*argc*/, char ** /*argv*/) {
  RT::getInstance().start(0);

  std::array<std::string, RT_SA_MAX_COUNT> data = {"AAAA", "BBBB", "CCCC", "DDDD", "1234", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000"};

  RT::getInstance().activateRt(1);
  RT::getInstance().setRt(1, 1, 5, ACE_BCCTRL_CHL_A, data);

  // Wait for user input to stop
  std::cout << "\nPress enter to stop\n\n";
  char input = 0;
  while (input != '\n') {
    std::cin.get(input);
  }

  return 0;
}