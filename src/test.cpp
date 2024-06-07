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

    const int N = 50000;

    time_t time1 = time(NULL);
    {
        sjtu::string str = "zzzzz";
        for (int i = N; i >= 1; i--) {
            bpt.insert(str, i);
            --str;
            // cout << "inserted " << N - i + 1 << endl;
        }
    }
    cout << "ALL INSERTED" << endl;
    assert(bpt.size() == N);

    time_t time2 = time(NULL);
    // int a;
    // cin >> a;
    // print();
    // bpt.find("xzzzzzz");
    // bpt.erase("xzzzzzzzzzzzzzzzzzzzzzzzzz", N);
    // bpt.erase("zzzzzzzzzzzzzzzzzzzzzzzzzz", N);
    // print();
    // todo : solve same elements
    {
        int successcnt = 0;
        sjtu::string str = "zzzzz";
        for (int i = N; i >= 1; i--, --str) {
            // if (i == 41) {
            //                     print();
            // }
            if (bpt.erase(str, i)) {
                // int sz = bpt.size();
                successcnt++;
                // cout << "successfully erased ";
                // str.print();
                // cout << endl;
                // print();
                // cout << "erased " << N - i + 1 << endl;
                // if (sz != i - 1) {
                //     cout << i - 1 << " bpt: " << sz << endl;
                //     exit(0);
                // }
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