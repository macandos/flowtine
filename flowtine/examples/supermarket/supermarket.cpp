#include "../../flowtine.h"
#include <iostream>

int main() {
    ftn::openFile("items.ftn");

    std::string item;

    std::cout << "Select for items: ";
    getline(std::cin, item);

    if (ftn::val(item) != "null") {
        std::cout << "You ordered a " << item << " which costs " << ftn::val(item) << std::endl;
    }
    else {
        std::cout << "no items found!\n";
    }
}