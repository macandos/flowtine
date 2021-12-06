#include "../../flowtine.h"
#include <iostream>

int main() {
    ftn::openFile("names.ftn");

    std::cout << ftn::val("name1") << " earns " << ftn::val("sal1") << std::endl;
    std::cout << ftn::val("name2") << " earns " << ftn::val("sal2") << std::endl;
    std::cout << ftn::val("name3") << " earns " << ftn::val("sal3") << std::endl;

    std::cout << "combined salaries: " << std::stoi(ftn::val("sal1")) + std::stoi(ftn::val("sal2")) + std::stoi(ftn::val("sal3")) << std::endl;
}