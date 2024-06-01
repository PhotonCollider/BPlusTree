#include <iostream>
#include <cassert>
#include "../include/bpt.hpp"
using sjtu::b_plus_tree;
using std::cin, std::cout, std::endl;
int main() {
    b_plus_tree bpt;
    std::ios::sync_with_stdio(false);
    cin.tie(0), cout.tie(0);
    int T;
    cin >> T;
    std::string command;
    char key[65];
    int value;
    for (int i = 1; i <= T; i++) {
        cin >> command;
        if (command == "insert") {
            cin >> key >> value;
            bpt.insert(key, value);
        } else if (command == "find") {
            cin >> key;
            bpt.find(key);
        } else {
            cin >> key >> value;
            assert(command == "delete");
            bpt.erase(key, value);
        }
    }
    return 0;
}
/*
8
insert FlowersForAlgernon 1966
insert CppPrimer 2012
insert Dune 2021
insert CppPrimer 2001
find CppPrimer
find Java
delete Dune 2021
find Dune
*/