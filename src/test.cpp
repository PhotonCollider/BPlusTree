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

    const int N = 2000;
    b_plus_tree bpt;
    {
        sjtu::string str = "zzzzzzzzzzzzzzzzzzzzzzzzzz";
        for (int i = N; i >= 1; i--) {
            bpt.insert(str, i);
            --str;
        }
    }

    bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");
    // bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", N);
    bpt.erase("zzzzzzzzzzzzzzzzzzzzzzzzzz", N);

    // {
    //     sjtu::string str = "zzzzzzzzzzzzzzzzzzzzzzzzzz";
    //     for (int i = N; i >= 1; i--) {
    //         bpt.erase(str, i);
    //         --str;
    //         cout << "erased i = " << i << endl;
    //     }
    // }
    // bpt.find("zzzzzzzzzzzzzzzzzzzzzzzzzz");
    // bpt.find("yzzzzzzzzzzzzzzzzzzzzzzzzz");
    // bpt.find("xzzzzzzzzzzzzzzzzzzzzzzzzz");

    return 0;
}