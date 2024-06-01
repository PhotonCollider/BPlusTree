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

    b_plus_tree bpt;
    // sjtu::string str = "aaaaaaaaaaaaaaaaaaaaaaaaaa";
    sjtu::string str = "zzzzzzzzzzzzzzzzzzzzzzzzzz";
    for (int i = 8000; i >= 1; i--) {
        bpt.insert(str, i);
        --str;
    }
    bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");
    bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", 8000);
    bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");
    bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", 7998);
    bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");
    bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", 7998);
    bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");
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

    return 0;
}