#include "../../flowtine.h"
#include <iostream>
using namespace ftn;

int main() {
    Flowtine ftn("names.ftn");

    std::cout << "John earns: " << ftn.get("employees.John") << "\n";
    std::cout << "Katy earns: " << ftn.get("employees.Katy") << "\n";
    std::cout << "Patrick earns: " << ftn.get("employees.Patrick") << "\n";
}