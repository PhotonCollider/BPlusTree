#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstring> // memmove

#include "util.hpp"
#include "file_io.hpp"
#include "vector.hpp"

using std::cout, std::endl;

namespace sjtu {

template <
    class Key = sjtu::string,
    class Value = int,
    class Hash = sjtu::string_hash
> class b_plus_tree {
private:
    static constexpr int BLOCK_SIZE = 4096;
#ifdef BIGTREE
    static constexpr int M = 340; // sizeof(node) <= BLOCK_SIZE
    static constexpr int L = 45; // sizeof(data_block) <= BLOCK_SIZE
#endif
#ifndef BIGTREE
    static constexpr int M = 5; // max son count
    static constexpr int L = 20; // max data in a data_block
#endif
    static constexpr int MIN_M = (M + 1) / 2; // this is the size of the just-split node
    static constexpr int MIN_L = (L + 1) / 2; // the is the size of the just-split data_block

    struct node {
        bool is_leaf; // if is_leaf, sons[i] is a data_block id, otherwise sons[i] is a node_id
        int sz; // num of keys
        // SZ IS NUM OF KEYS!!!!!
        // SZ IS NUM OF KEYS!!!!!
        // SZ IS NUM OF KEYS!!!!!


        size_t keys[M]; // less than key[i], goto son[i]
        int sons[M + 1]; // M keys and M + 1 sons is the "before split" state

        node(bool _is_leaf) { // node(int _node_id);
            is_leaf = _is_leaf;
            sz = 0;
            // node_id = _node_id;
        }
    };

    struct data_block { // sorted by value
        struct pair {
            size_t hash;
            Key k;
            Value v;

            bool operator < (const pair& rhs) const {
                return hash < rhs.hash;
            }
        };

        int sz;
        pair data[L + 1]; // L+1 elements in transient state (before split)

        data_block() {
            sz = 0;
        }
    };

    int rootid;
    File_Manager<node> node_file;
    File_Manager<data_block> data_block_file;
    sjtu::vector<Value> found; // used in find()

    /**
     * inserts at node nodeid (node cur)
     * finds sons[i] of node cur and calls insert_node/insert_data_block(sons[i])
     * after insertion, check if the called insertion returns a nodeid new_node
     * (which means that sons[i] just split, and need to insert new_node after sons[i])
     * after that, if node cur is oversized, split a half into a new node
     * and return its nodeid in ret_id, the individual key in ret_key
     * otherwise ret_id = -1
     */
    void insert_node(int nodeid, const Key& key, const Value& value, int& ret_id, size_t& ret_key) {
        size_t hash = Hash()(key);
        node cur(true);
        node_file.read(cur, nodeid);

        bool gone = false;
        int new_id, x;
        size_t new_key;
        for (int i = 0; i < cur.sz; i++) {
            if (hash < cur.keys[i]) {
                gone = true;
                x = i;
                break;
            }
        }
        if (!gone) {
            x = cur.sz;
        }
        assert(0 <= x && x <= cur.sz);
        if (cur.is_leaf) {
            insert_data_block(cur.sons[x], key, value, new_id, new_key);
        } else {
            insert_node(cur.sons[x], key, value, new_id, new_key);
        }
        if (new_id == -1) {
            ret_id = -1;
            return;
        }
        memmove(cur.sons + x + 2, cur.sons + x + 1, (cur.sz - x) * sizeof(int));
        memmove(cur.keys + x + 1, cur.keys + x, (cur.sz - x) * sizeof(size_t));
        cur.sons[x + 1] = new_id;
        cur.keys[x] = new_key;
        ++cur.sz;
        if (cur.sz + 1 <= M) {
            node_file.update(cur, nodeid);
            ret_id = -1;
        } else { // split node
            // remember: node.sz means the number of keys in the node
            node new_node(cur.is_leaf);
            new_node.sz = (cur.sz + 1) - (cur.sz + 1) / 2 - 1;
            cur.sz = (cur.sz + 1) / 2 - 1;
            assert(new_node.sz + cur.sz + 1 == M);
            memmove(new_node.sons, cur.sons + cur.sz + 1, (new_node.sz + 1) * sizeof(int));
            memmove(new_node.keys, cur.keys + cur.sz + 1, new_node.sz * sizeof(size_t));
            ret_key = cur.keys[cur.sz];
            ret_id = node_file.write(new_node);
            node_file.update(cur, nodeid);
        }
    }

    /**
     * inserts at data block dbid (data_block db)
     * after insertion, check if db is oversized
     * if so, split a half into a new data block and return its dbid
     * otherwise return -1
     */
    void insert_data_block(int dbid, const Key& key, const Value& value, int& ret_id, size_t& ret_key) {
        size_t hash = Hash()(key);
        data_block db;
        data_block_file.read(db, dbid);

        bool gone = false;
        int x;
        for (int i = 0; i < db.sz; i++) {
            if (hash < db.data[i].hash) {
                gone = true;
                x = i;
                break;
            }
        }
        if (!gone) {
            x = db.sz;
        }
        memmove(db.data + x + 1, db.data + x, (db.sz - x) * sizeof(typename data_block::pair));
        db.sz++;
        db.data[x] = { Hash()(key), key, value };

        if (db.sz <= L) {
            data_block_file.update(db, dbid);
            ret_id = -1;
        } else { // split
            data_block new_block;
            new_block.sz = db.sz - db.sz / 2; // wrong in node, correct here
            db.sz /= 2;
            memmove(new_block.data, db.data + db.sz, new_block.sz * sizeof(typename data_block::pair));
            data_block_file.update(db, dbid);
            ret_id = data_block_file.write(new_block);
            // cout << "split and created new dbid " << ret_id << endl;
            ret_key = new_block.data[0].hash;
        }
    }

    /**
     * for every data in the block whose key is key
     * put its value in the vector found
     */
    void find_data_block(int dbid, const Key& key) {
        size_t hash = Hash()(key);
        data_block db;
        data_block_file.read(db, dbid);

        for (int i = 0; i < db.sz; i++) {
            if (db.data[i].hash == hash && db.data[i].k == key) {
                found.push_back(db.data[i].v);
            }
        }
    }

    /**
     * finds all data with its key == key, and put them in the vector found
     * vector found is managed by the public find function
     */
    void find_node(int nodeid, const Key& key) {
        // cout << "finding in node " << nodeid << endl;
        size_t hash = Hash()(key);
        node cur(true);
        node_file.read(cur, nodeid);

        size_t prev_key = 0;
        for (int i = 0; i < cur.sz; i++) { // caution! there may be same elements
            if (prev_key <= hash && hash <= cur.keys[i]) {
                if (cur.is_leaf) {
                    find_data_block(cur.sons[i], key);
                } else {
                    find_node(cur.sons[i], key);
                }
            }
            prev_key = cur.keys[i];
        }
        if (cur.sz == 0 || cur.keys[cur.sz - 1] <= hash) {
            if (cur.is_leaf) {
                find_data_block(cur.sons[cur.sz], key);
            } else {
                find_node(cur.sons[cur.sz], key);
            }
        }
    }

    /**
     * erases data {hash, key, value} under node nodeid
     * erase_node/erase_data_block(sons[x])
     * return true if succesfully erased, this will skip erasing the same data in other nodes
     * since the problem guaranteed that {key, value} would be unique
     * bool& toosmall serves as a return value, toosmall=true shows that the erase made sons[x] too small
     * and needs resizing from node cur
     */
    bool erase_node(int nodeid, const Key& key, const Value& value, bool& ret_toosmall) {
        size_t hash = Hash()(key);
        node cur(true);
        node_file.read(cur, nodeid);

        bool toosmall = false, done = false;
        int x;
        size_t prev_key = 0;
        for (int i = 0; i < cur.sz; i++) { // caution! there may be same elements
            if (prev_key <= hash && hash <= cur.keys[i]) {
                if (cur.is_leaf) {
                    done = erase_data_block(cur.sons[i], key, value, toosmall);
                } else {
                    done = erase_node(cur.sons[i], key, value, toosmall);
                }
                if (done) {
                    x = i;
                    break;
                }
            }
            prev_key = cur.keys[i];
        }
        if (!done && (cur.sz == 0 || cur.keys[cur.sz - 1] <= hash)) {
            x = cur.sz;
            if (cur.is_leaf) {
                done = erase_data_block(cur.sons[cur.sz], key, value, toosmall);
            } else {
                done = erase_node(cur.sons[cur.sz], key, value, toosmall);
            }
        }

        if (!done) {
            return false;
        }
        if (toosmall) { // only when done is true is "toosmall" valid
            if (cur.sz == 0) { // cannot borrow or merge
                ret_toosmall = true;
                return true;
            }
            if (cur.is_leaf) {
                // try borrowing
                data_block db;
                data_block_file.read(db, cur.sons[x]);
                assert(db.sz == MIN_L - 1);
                bool success = false;
                if (x > 0) {
                    data_block left_db;
                    data_block_file.read(left_db, cur.sons[x - 1]);
                    if (left_db.sz > MIN_L) {
                        int transfer = left_db.sz - (db.sz + left_db.sz) / 2;
                        memmove(db.data + transfer, db.data, sizeof(typename data_block::pair) * db.sz);
                        memmove(db.data, left_db.data + left_db.sz - transfer, sizeof(typename data_block::pair) * transfer);
                        left_db.sz -= transfer;
                        db.sz += transfer;
                        data_block_file.update(db, cur.sons[x]);
                        data_block_file.update(left_db, cur.sons[x - 1]);

                        cur.keys[x - 1] = db.data[0].hash;
                        node_file.update(cur, nodeid);
                        success = true;
                    }
                }
                if (!success && x < cur.sz) {
                    data_block right_db;
                    data_block_file.read(right_db, cur.sons[x + 1]);
                    if (right_db.sz > MIN_L) {
                        int transfer = (db.sz + right_db.sz) / 2 - db.sz;
                        memmove(db.data + db.sz, right_db.data, sizeof(typename data_block::pair) * transfer);
                        memmove(right_db.data, right_db.data + transfer, sizeof(typename data_block::pair) * (right_db.sz - transfer));
                        db.sz += transfer;
                        right_db.sz -= transfer;
                        data_block_file.update(db, cur.sons[x]);
                        data_block_file.update(right_db, cur.sons[x + 1]);

                        cur.keys[x] = right_db.data[0].hash;
                        node_file.update(cur, nodeid);
                        success = true;
                    }
                }

                if (!success) {// failed in borrowing, merge now
                    // merge with left node (x > 0)
                    // merge with right node (x == 0)
                    if (x == 0) {
                        x = 1; // now we only need to eat the left data_block of x
                        assert(cur.sz >= 1); // or x = 1 would be faulty
                        data_block_file.read(db, cur.sons[x]);
                    }
                    data_block left_db;
                    data_block_file.read(left_db, cur.sons[x - 1]);
                    memmove(db.data + left_db.sz, db.data, db.sz * sizeof(typename data_block::pair));
                    memmove(db.data, left_db.data, left_db.sz * sizeof(typename data_block::pair));
                    db.sz += left_db.sz;
                    data_block_file.erase(cur.sons[x - 1]);
                    data_block_file.update(db, cur.sons[x]);

                    memmove(cur.sons + x - 1, cur.sons + x, (cur.sz + 1 - x) * sizeof(int));
                    memmove(cur.keys + x - 1, cur.keys + x, (cur.sz - x) * sizeof(size_t));
                    cur.sz--;
                    node_file.update(cur, nodeid);
                    if (cur.sz + 1 < MIN_M) {
                        ret_toosmall = true;
                    }
                }
            } else { // if (cur.isleaf)
                // try borrowing
                node son(true);
                node_file.read(son, cur.sons[x]);
                assert(son.sz == MIN_M - 1 - 1); // SZ IS NUM OF KEYS!!!!!
                bool success = false;
                if (x > 0) {
                    node left_son(true);
                    node_file.read(left_son, cur.sons[x - 1]);
                    if (left_son.sz + 1 > MIN_M) {
                        // only transfer one, because transfering many is soooooo hard
                        memmove(son.sons + 1, son.sons, (son.sz + 1) * sizeof(int));
                        memmove(son.keys + 1, son.keys, son.sz * sizeof(size_t));
                        son.sons[0] = left_son.sons[left_son.sz];
                        son.keys[0] = cur.keys[x - 1];
                        cur.keys[x - 1] = left_son.keys[left_son.sz - 1];

                        left_son.sz--;
                        son.sz++;
                        node_file.update(son, cur.sons[x]);
                        node_file.update(left_son, cur.sons[x - 1]);
                        node_file.update(cur, nodeid);
                        success = true;
                    }
                }
                if (!success && x < cur.sz) { // SZ IS NUM OF KEYS!!!!!
                    node right_son(true);
                    node_file.read(right_son, cur.sons[x + 1]);
                    if (right_son.sz + 1 > MIN_M) {
                        // only transfer one, because transfering many is soooooo hard

                        son.sons[son.sz + 1] = right_son.sons[0];
                        memmove(right_son.sons, right_son.sons + 1, right_son.sz * sizeof(int));
                        son.keys[son.sz] = cur.keys[x];
                        cur.keys[x] = right_son.keys[0];
                        memmove(right_son.keys, right_son.keys + 1, (right_son.sz - 1) * sizeof(size_t));

                        right_son.sz--;
                        son.sz++;
                        node_file.update(son, cur.sons[x]);
                        node_file.update(right_son, cur.sons[x + 1]);
                        node_file.update(cur, nodeid);
                        success = true;
                    }
                }

                if (!success) {// failed in borrowing, merge now
                    // merge with left node (x > 0)
                    // merge with right node (x == 0)
                    if (x == 0) {
                        x = 1; // now we only need to eat the left node of x
                        assert(cur.sz >= 1); // or x = 1 would be faulty   // SZ IS NUM OF KEYS!!!!!
                        node_file.read(son, cur.sons[x]);
                    }
                    node left_son(true);
                    node_file.read(left_son, cur.sons[x - 1]);

                    memmove(son.sons + left_son.sz + 1, son.sons, (son.sz + 1) * sizeof(int));
                    memmove(son.sons, left_son.sons, (left_son.sz + 1) * sizeof(int));
                    memmove(son.keys + left_son.sz + 1, son.keys, son.sz * sizeof(size_t)); // leave left_son.sz + 1 space
                    memmove(son.keys, left_son.keys, left_son.sz * sizeof(size_t)); // occupy left_son.sz space
                    son.keys[left_son.sz] = cur.keys[x - 1];
                    son.sz += left_son.sz + 1;
                    node_file.erase(cur.sons[x - 1]);
                    node_file.update(son, cur.sons[x]);

                    memmove(cur.sons + x - 1, cur.sons + x, (cur.sz - x + 1) * sizeof(int));
                    memmove(cur.keys + x - 1, cur.keys + x, (cur.sz - x) * sizeof(size_t));
                    cur.sz--;
                    node_file.update(cur, nodeid);
                    if (cur.sz + 1 < MIN_M) {
                        ret_toosmall = true;
                    }
                }
            }
        }
        return true;
    }

    /**
     * erases data {hash, key, value} under data_block dbid
     * return true if succesfully erased, this will skip erasing the same data in other nodes
     * since the problem guaranteed that {key, value} would be unique
     */
    bool erase_data_block(int dbid, const Key& key, const Value& value, bool& ret_toosmall) {
        size_t hash = Hash()(key);
        data_block db;
        data_block_file.read(db, dbid);

        bool done = false;
        for (int i = 0; i < db.sz; i++) { // caution! there may be same elements
            if (db.data[i].hash == hash && db.data[i].k == key && db.data[i].v == value) {
                memmove(db.data + i, db.data + i + 1, (db.sz - i - 1) * sizeof(typename data_block::pair));
                db.sz--;
                done = true;
                break;
            }
        }
        if (done) {
            data_block_file.update(db, dbid);
            // cout << "the erased data_block looks like this" << endl;
            // print_db(dbid);
            // cout << "the above is the erased data block" << endl;
            if (db.sz < MIN_L) {
                ret_toosmall = true;
            }
        }
        return done;
    }

public:
    b_plus_tree() : node_file("node.txt"), data_block_file("data_block.txt") {
        // cout << "size of node : " << sizeof(node) << endl;
        // cout << "size of data block : " << sizeof(data_block) << endl;
        node_file.get_root(rootid);
        if (rootid == -1) {
            node root(true);
            root.sz = 0;
            root.sons[0] = data_block_file.write(data_block());
            rootid = node_file.write(root);
            assert(rootid == 4);
        }
    }

    ~b_plus_tree() {
        node_file.write_root(rootid);
    }

    void insert(const Key& key, const Value& value) {
        int new_id;
        size_t new_key;
        insert_node(rootid, key, value, new_id, new_key);
        if (new_id != -1) { // root splits
            // cout << "ROOT SPLITS!!!" << endl;
            node new_root(false);
            new_root.sz = 1; // SZ IS NUM OF KEYS!!!!!
            new_root.keys[0] = new_key;
            new_root.sons[0] = rootid;
            new_root.sons[1] = new_id;
            rootid = node_file.write(new_root);
        }
    }

    bool erase(const Key& key, const Value& value) { // may not exist
        bool root_toosmall = false;
        bool res = erase_node(rootid, key, value, root_toosmall);
        if (res) { // the criteria for toosmall is different from other nodes
            node root(false);
            node_file.read(root, rootid);
            if (root.sz == 0 && !root.is_leaf) {
                // cout << "ROOT FALLS!!!" << endl;
                node_file.erase(rootid);
                rootid = root.sons[0];
            }
        }
        return res;
    }

    /**
     * calls find_node, which finds all data with its key == key, and put them in the vector found
     * sorts found, output found
     * clears found
     */
    void find(const Key& key) {
        find_node(rootid, key);
        if (found.empty()) {
            cout << "null" << endl;
        } else {
            // cout << "found " << found.size() << " elements" << endl;
            found.sort();
            for (int i = 0; i < found.size() - 1; i++) {
                cout << found[i] << ' ';
            }
            cout << found[found.size() - 1] << endl;
            found.clear();
        }
    }

#ifdef BPT_DEBUG_FUNCTIONS
    /**
     * self checks that if any node have two same sons (which is of course a bug)
    */
    void self_check(int nodeid = -1) {
        if (nodeid == -1) {
            nodeid = rootid;
        }
        node cur(true);
        node_file.read(cur, nodeid);
        for (int i = 1; i <= cur.sz; i++) {
            if (cur.sons[i] == cur.sons[i - 1]) {
                cout << "SAME SONS ERROR AT NODEID " << nodeid << endl;
                exit(0);
                // return;
            }
        }
        if (!cur.is_leaf) {
            for (int i = 0; i <= cur.sz; i++) {
                self_check(cur.sons[i]);
            }
        }
    }

    void print_only_one_node(int nodeid) {
        cout << "start print node----------------------------------------------------------------------------------------------------------------\n";
        node cur(true);
        node_file.read(cur, nodeid);
        cout << "now printing node " << nodeid << endl;
        cout << "is leaf: " << cur.is_leaf << endl;
        cout << "size: " << cur.sz << endl;
        cout << "Printing Keys:" << endl << '\t';
        for (int i = 0; i < cur.sz; i++) {
            cout << cur.keys[i] << ' ';
        }
        cout << endl;
        cout << "Printing Sons:" << endl << '\t';
        for (int i = 0; i <= cur.sz; i++) {
            cout << cur.sons[i] << ' ';
        }
        cout << endl;
        cout << "end print node----------------------------------------------------------------------------------------------------------------\n";
    }

    void print(int nodeid = -1) {
        if (nodeid == -1) {
            nodeid = rootid;
        }
        node cur(true);
        node_file.read(cur, nodeid);
        cout << "now printing node " << nodeid << endl;
        cout << "is leaf: " << cur.is_leaf << endl;
        cout << "size: " << cur.sz << endl;
        cout << "Printing Keys:" << endl << '\t';
        for (int i = 0; i < cur.sz; i++) {
            cout << cur.keys[i] << ' ';
        }
        cout << endl;
        cout << "Printing Sons:" << endl << '\t';
        for (int i = 0; i <= cur.sz; i++) {
            cout << cur.sons[i] << ' ';
        }
        cout << endl;

        for (int i = 0; i <= cur.sz; i++) {
            cout << "Printing in detail the " << i << " th ";
            if (!cur.is_leaf) {
                cout << "node\n";
                print(cur.sons[i]);
            } else {
                cout << "data_block\n";
                print_db(cur.sons[i]);
            }
        }
        // cout << "----------------------------------------------------------------------------------------------------------------\n";
    }

    void print_db(int dbid) {
        data_block db;
        data_block_file.read(db, dbid);
        cout << "now printing data_block " << dbid << endl;
        cout << "size: " << db.sz << endl;
        cout << "Printing contents\n";
        for (int i = 0; i < db.sz; i++) {
            db.data[i].k.print();
            cout << ' ';
        }
        cout << '\n' << endl;
    }

    int size(int nodeid = -1) {
        if (nodeid == -1) {
            nodeid = rootid;
        }
        node cur(true);
        node_file.read(cur, nodeid);
        int ans = 0;

        for (int i = 0; i <= cur.sz; i++) {
            if (!cur.is_leaf) {
                ans += size(cur.sons[i]);
            } else {
                ans += size_db(cur.sons[i]);
            }
        }
        return ans;
    }

    int size_db(int dbid) {
        data_block db;
        data_block_file.read(db, dbid);
        return db.sz;
    }
#endif

};

}

#endif