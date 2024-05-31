#include <iostream>
#include "../include/file_io.hpp"
#include "../include/util.hpp"
#include "../include/bpt.hpp"
using std::cin, std::cout, std::endl;
using sjtu::string, sjtu::qsort, sjtu::b_plus_tree;

void separate() {
    cout << "----------------------------------------------------------------------------------------------------------------\n";
}

int main() {
    // fstream file;
    // file.open("totoo.txt", std::ios::in | std::ios::out);
    // assert(file.is_open());
    // cout << "at first " << file.tellp() << endl;
    // file << 'a';
    // cout << file.tellp();
    // file.close();
    // b_plus_tree bpt;
    // for (int i = 1; i <= 1; i++) {
    //     bpt.insert("nice", 111);
        // bpt.print();
        // separate();
    // }
    // bpt.print();
    // separate();
    // bpt.insert("nipse", 16);
    // bpt.print();
    // separate();
    // bpt.insert("speni", 17);
    // bpt.print();
    // separate();
    // bpt.insert("penis", 18);
    // bpt.print();
    // separate();
    // bpt.insert("pinse", 19);
    // bpt.print();
    // separate();;

    // bpt.find("penis");
    // bpt.find("certainlydoesnotexist");
    // bpt.find("penis");
    // bpt.erase("penis", 18);
    // bpt.find("penis");

    // bpt.print();
    // separate();
    // bpt.insert("abcde", 114514);
    // bpt.insert("zzzzz", 114514);
    // bpt.print();
    // bpt.find("nipse");
    // separate();
    // bpt.find("nice");
    // separate();
    // bpt.find("ppppppp");






    b_plus_tree bpt;
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
    // bpt.insert("FlowersForAlgernon", 1966);
    // bpt.insert("CppPrimer", 2012);
    // bpt.insert("Dune", 2021);
    // bpt.insert("CppPrimer", 2001);
    bpt.find("CppPrimer");
    bpt.find("Java");
    bpt.erase("Dune", 2021);
    bpt.find("Dune");
    return 0;
}