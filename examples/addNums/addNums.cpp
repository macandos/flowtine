#include "../../flowtine.h"
#include <iostream>
#include <string>
using namespace ftn;

int main() {
    Flowtine ftn("nums.ftn");

    // converts the values to integers
    int a = std::stoi(ftn.get("a"));
    int b = std::stoi(ftn.get("b"));

    std::cout << a + b;
}