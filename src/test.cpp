#include <iostream>
#include "../include/file_io.hpp"
#include "../include/util.hpp"
#include "../include/bpt.hpp"
#include <ctime>
using std::cin, std::cout, std::endl;
using sjtu::string, sjtu::qsort, sjtu::b_plus_tree;

b_plus_tree bpt;

void print() {
    std::cout << "tree start----------------------------------------------------------------------------------------------------------------\n";
    bpt.print();
    std::cout << "tree end----------------------------------------------------------------------------------------------------------------\n";
}

int main() {
    // fstream file;
    // file.open("totoo.txt", std::ios::in | std::ios::out);
    // assert(file.is_open());
    // cout << "at first " << file.tellp() << endl;
    // file << 'a';
    // cout << file.tellp();
    // file.close();

    const int N = 30000;

    time_t time1 = time(NULL);
    {
        sjtu::string str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
        for (int i = N; i >= 1; i--) {
            bpt.insert(str, i);
            --str;
        }
    }

    // assert(bpt.size() == N);
    time_t time2 = time(NULL);
    // print();
    // bpt.find("xzzzzzz");
    // bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", N);
    // bpt.erase("zzzzzzzzzzzzzzzzzzzzzzzzzz", N);
    // print();
    // todo : solve same elements
    {
        int successcnt = 0;
        sjtu::string str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
        for (int i = N; i >= 1; i--, --str) {
            // if (i == 41) {
            //                     print();
            // }
            if (bpt.erase(str, i)) {
                successcnt++;
                // cout << "successfully erased ";
                // str.print();
                // cout << endl;
                // print();
            } else {
                cout << "could not erase ";
                str.print();
                cout << endl;
                cout << i << endl;
                print();
                exit(0);
            }
        }
        cout << "succeeded " << successcnt << endl;
    }
    time_t time3 = time(NULL);
    cout << "insertion " << time2 - time1 << endl;
    cout << "deletion " << time3 - time2 << endl;
    return 0;
}