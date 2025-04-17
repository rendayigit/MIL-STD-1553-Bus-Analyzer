#include "rt.hpp"

int main(int  /*argc*/, char**  /*argv*/) {
    RT::getInstance().start(0);

    RT::getInstance().setRt(1, 1, 5, ACE_BCCTRL_CHL_A, { "AAAA", "BBBB", "CCCC", "1234", "FFFF" });
    RT::getInstance().activateRt(1, 1);
    
    return 0;
}