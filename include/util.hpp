#ifndef SJTU_UTIL_HPP
#define SJTU_UTIL_HPP

#include <cstddef>
#include <iostream>

namespace sjtu {
/*
 * templated quicksort
 * T should have operator <
 * operator == is not used
 */
template<class T>
void qsort(T* begin, T* end) {
    if (begin + 1 >= end) {
        return;
    }
    T pivot = *(begin + static_cast<int>((end - begin - 1) * 1.0 * rand() / RAND_MAX));
    T* ptr1 = begin, * ptr = begin, * ptr2 = end;
    auto swap = [](T* t1, T* t2) {
        T tmp = *t1;
        *t1 = *t2;
        *t2 = tmp;
        };
    while (ptr < ptr2) {
        if (*ptr < pivot) {
            swap(ptr++, ptr1++);
        } else if (pivot < *ptr) {
            swap(ptr, --ptr2);
        } else {
            ptr++;
        }
    }
    qsort(begin, ptr1);
    qsort(ptr2, end);
}

class string { // fixed_size string
private:
    static constexpr int max_size = 64;
    static constexpr size_t hash_b = 101;
    static constexpr size_t hash_MOD = 998244353;

    int sz;
    char a[max_size + 1]; // '\0'
public:
    string(const char* s = nullptr) { // strlen(s) should <= maxsize
        if (s == nullptr) {
            sz = 0;
            a[0] = 0;
            return;
        }
        sz = -1;
        for (int i = 0; i <= max_size; i++) {
            a[i] = s[i];
            if (s[i]) {
                a[i] = s[i];
            } else {
                sz = i;
                break;
            }
        }
        assert(sz != -1);
    }
    int size() const {
        return sz;
    }
    void print() const {
        std::cout << a;
    }
    size_t hash() const {
        size_t ret = 0;
        for (int i = 0; i < sz; i++) {
            ret *= hash_b;
            ret += a[i];
            ret %= hash_MOD;
        }
        return ret;
    }
    bool operator==(const string& rhs) const {
        if (sz != rhs.sz) {
            return false;
        }
        for (int i = 0; i < sz; i++) {
            if (a[i] != rhs.a[i]) {
                return false;
            }
        }
        return true;
    }
    string& operator++() { // used only for 'a' to 'z'
        a[sz - 1]++;
        for (int i = sz - 1; i >= 0; i--) {
            if (a[i] > 'z') {
                a[i] = 'a';
                if (i > 0) {
                    a[i - 1]++;
                } else {
                    std::cout << "string ++ overflow" << std::endl;
                    exit(0);
                }
            } else {
                break;
            }
        }
        return *this;
    }
    string& operator--() {
        a[sz - 1]--;
        for (int i = sz - 1; i >= 0; i--) {
            if (a[i] < 'a') {
                a[i] = 'z';
                if (i > 0) {
                    a[i - 1]--;
                } else {
                    std::cout << "string -- overflow" << std::endl;
                    exit(0);
                }
            } else {
                break;
            }
        }
        return *this;
    }
};

class string_hash {
public:
    size_t operator()(const string& s) {
        return s.hash();
    }
};

}

#endif