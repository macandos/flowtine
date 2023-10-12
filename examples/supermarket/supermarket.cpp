#include "../../flowtine.h"
#include <iostream>
using namespace ftn;

int main() {
    Flowtine ftn("items.ftn");

    std::string item;

    std::cout << "Select for items: ";
    getline(std::cin, item);

    if (ftn.get(item) != "") {
        std::cout << "You ordered a(n) " << item << " which costs " << ftn.get(item) << std::endl;
    }
    else {
        std::cout << "no items found!\n";
    }
}