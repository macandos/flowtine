#include "../../flowtine.h"
#include <iostream>

int main() {
    ftn::openFile("nums.ftn");

    // converts the values to integers
    int a = std::stoi(ftn::val("a"));
    int b = std::stoi(ftn::val("b"));

    std::cout << a+b;
}