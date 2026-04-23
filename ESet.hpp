#ifndef ESET_HPP
#define ESET_HPP

#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>

template <typename Key>
class ESet {
public:
    struct Node {
        int l, r;
        Key val;
        int sz;
        unsigned int prio;

        Node() : l(0), r(0), val(Key()), sz(0), prio(0) {}
        Node(Key v) : l(0), r(0), val(v), sz(1), prio(rand()) {}
    };

    static std::vector<Node> pool;
    static int newNode(Key v) {
        Node n(v);
        pool.push_back(n);
        return (int)pool.size() - 1;
    }
    static int copyNode(int u) {
        if (!u) return 0;
        Node n = pool[u];
        pool.push_back(n);
        return (int)pool.size() - 1;
    }

    int root;

    ESet() : root(0) {
        if (pool.empty()) {
            srand(time(0));
            pool.reserve(2000000);
            pool.emplace_back(); // null node
        }
    }

    static void update(int u) {
        if (u) pool[u].sz = pool[pool[u].l].sz + pool[pool[u].r].sz + 1;
    }

    // Split by value: l will have values < v, r will have values >= v
    static void split(int u, Key v, int &l, int &r) {
        if (!u) {
            l = r = 0;
            return;
        }
        u = copyNode(u);
        if (pool[u].val < v) {
            l = u;
            split(pool[u].r, v, pool[l].r, r);
            update(l);
        } else {
            r = u;
            split(pool[u].l, v, l, pool[r].l);
            update(r);
        }
    }

    static int merge(int l, int r) {
        if (!l || !r) return l | r;
        if (pool[l].prio > pool[r].prio) {
            l = copyNode(l);
            pool[l].r = merge(pool[l].r, r);
            update(l);
            return l;
        } else {
            r = copyNode(r);
            pool[r].l = merge(l, pool[r].l);
            update(r);
            return r;
        }
    }

    int size() const { return pool[root].sz; }

    void emplace(Key v) {
        int l, m, r;
        split(root, v, l, r);
        split(r, v + 1, m, r);
        if (!m) m = newNode(v);
        root = merge(merge(l, m), r);
    }

    void erase(Key v) {
        int l, m, r;
        split(root, v, l, r);
        split(r, v + 1, m, r);
        // m is the part equal to v, discard it
        root = merge(l, r);
    }

    struct iterator {
        std::vector<int> path;

        iterator() {}
        iterator(int r, Key v) {
            int curr = r;
            while (curr) {
                path.push_back(curr);
                if (v < pool[curr].val) curr = pool[curr].l;
                else if (pool[curr].val < v) curr = pool[curr].r;
                else break;
            }
            if (curr && pool[curr].val != v) path.clear();
        }

        const Key& operator*() const {
            return pool[path.back()].val;
        }

        bool operator==(const iterator& other) const {
            return path == other.path;
        }
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        iterator& operator++() {
            if (path.empty()) return *this;
            int curr = path.back();
            if (pool[curr].r) {
                curr = pool[curr].r;
                while (curr) {
                    path.push_back(curr);
                    curr = pool[curr].l;
                }
            } else {
                int last = path.back();
                path.pop_back();
                while (!path.empty() && pool[path.back()].r == last) {
                    last = path.back();
                    path.pop_back();
                }
            }
            return *this;
        }

        iterator& operator--() {
            if (path.empty()) return *this;
            int curr = path.back();
            if (pool[curr].l) {
                curr = pool[curr].l;
                while (curr) {
                    path.push_back(curr);
                    curr = pool[curr].r;
                }
            } else {
                int last = path.back();
                path.pop_back();
                while (!path.empty() && pool[path.back()].l == last) {
                    last = path.back();
                    path.pop_back();
                }
            }
            return *this;
        }
    };

    iterator end() const { return iterator(); }

    iterator find(Key v) const {
        iterator it(root, v);
        if (it.path.empty() || pool[it.path.back()].val != v) return end();
        return it;
    }

    int count_less(int u, Key v) const {
        int count = 0;
        while (u) {
            if (pool[u].val < v) {
                count += pool[pool[u].l].sz + 1;
                u = pool[u].r;
            } else {
                u = pool[u].l;
            }
        }
        return count;
    }

    int range(Key L, Key R) const {
        if (R < L) return 0;
        return count_less(root, R + 1) - count_less(root, L);
    }
};

template <typename Key>
std::vector<typename ESet<Key>::Node> ESet<Key>::pool;

#endif
