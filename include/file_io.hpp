/**
 * DONT FORGET TO SET BIGTREE AND CLEAR_AT_START
*/
#ifndef SJTU_FILE_IO_HPP
#define SJTU_FILE_IO_HPP

#define BIGTREE

#include <fstream>
#include <cassert>
#include <iostream>
using std::cin, std::cout, std::endl; // DEBUG

template<class T>
class File_Manager {
private:
    struct vacantList {
        int pos;
        vacantList* nxt;
    };

    static constexpr bool CLEAR_AT_START = false;

    vacantList* head = nullptr;
    std::fstream file;
    std::string file_name;

public:
    File_Manager() = delete; // must specify path

    File_Manager(const std::string& file_name) : file_name(file_name) {
        file.open(file_name);
        if (file.is_open()) {
            file.close();
            if (CLEAR_AT_START) {
                init();
            }
        } else {
            init();
        }
    }

    void init() {
        if (file_name.empty()) return;
        file.open(file_name, std::ios::out); // truncate all
        int t = -1;
        file.write(reinterpret_cast<char*>(&t), sizeof(int));
        file.close();
    }

    void get_root(int& tmp) {
        file.open(file_name, std::ios::in);
        file.read(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
    }

    void write_root(int tmp) {
        file.open(file_name);
        file.write(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
    }

    int write(const T& t) { // returns index
        // cout << "FM going to write" << endl;
        file.open(file_name);
        assert(file.is_open());
        if (head) {
            file.seekp(head->pos);
            auto tmp = head->nxt;
            delete head;
            head = tmp;
        } else {
            file.seekp(0, std::ios::end);
        }
        int index = file.tellp();
        file.write(reinterpret_cast<const char*>(&t), sizeof(T));
        file.close();
        return index;
    }

    int peek() { // peek where the next t will be put
        file.open(file_name);
        if (head) {
            file.seekp(head->pos);
        } else {
            file.seekp(0, std::ios::end);
        }
        int ret = file.tellp();
        file.close();
        return ret;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T& t, const int index) {
        file.open(file_name);
        file.seekp(index);
        file.write(reinterpret_cast<char*>(&t), sizeof(T));
        file.close();
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T& t, const int index) {
        file.open(file_name, std::ios::in);
        file.seekg(index);
        file.read(reinterpret_cast<char*>(&t), sizeof(T));
        file.close();
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void erase(int index) {
        head = new vacantList{ index, head };
        // if (head == nullptr)
        //     head = new vacantList{ index, nullptr };
        // else
        //     head = new vacantList{ index, head };
    }
};

#endif // SJTU_FILE_IO_HPP
