#include <iostream>
#include <vector>
#include <cstdio>
#include <stack>
#include <ctime>
#include <cstdlib>

using namespace std;

template <typename Key>
struct Node {
    int l, r;
    Key val;
    int sz;
    unsigned int prio;
};

Node<long long> pool[2100005];
int pool_ptr = 1;

int newNode(long long v) {
    if (pool_ptr >= 2100000) return 0; // Out of memory
    int u = pool_ptr++;
    pool[u].l = pool[u].r = 0;
    pool[u].val = v;
    pool[u].sz = 1;
    pool[u].prio = rand();
    return u;
}

int copyNode(int u) {
    if (!u) return 0;
    if (pool_ptr >= 2100000) return 0;
    int v = pool_ptr++;
    pool[v] = pool[u];
    return v;
}

void update(int u) {
    if (u) pool[u].sz = pool[pool[u].l].sz + pool[pool[u].r].sz + 1;
}

void split(int u, long long v, int &l, int &r) {
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

int merge(int l, int r) {
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

struct iterator_state {
    vector<int> path;

    const long long& operator*() const {
        return pool[path.back()].val;
    }

    bool operator==(const iterator_state& other) const {
        return path == other.path;
    }
    bool operator!=(const iterator_state& other) const {
        return !(*this == other);
    }

    void operator++() {
        if (path.empty()) return;
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
    }

    void operator--() {
        if (path.empty()) return;
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
    }
};

struct ESet {
    int root;
    ESet() : root(0) {}

    int size() const { return pool[root].sz; }

    void emplace(long long v) {
        int l, m, r;
        split(root, v, l, r);
        split(r, v + 1, m, r);
        if (!m) m = newNode(v);
        root = merge(merge(l, m), r);
    }

    void erase(long long v) {
        int l, m, r;
        split(root, v, l, r);
        split(r, v + 1, m, r);
        root = merge(l, r);
    }

    iterator_state find(long long v) const {
        iterator_state it;
        int curr = root;
        while (curr) {
            it.path.push_back(curr);
            if (v < pool[curr].val) curr = pool[curr].l;
            else if (pool[curr].val < v) curr = pool[curr].r;
            else break;
        }
        if (curr && pool[curr].val == v) return it;
        it.path.clear();
        return it;
    }

    int count_less(long long v) const {
        int count = 0;
        int u = root;
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

    int range(long long L, long long R) const {
        if (R < L) return 0;
        return count_less(R + 1) - count_less(L);
    }
};

ESet s[1000005];
int main_lst = 0;

int main() {
    srand(1337);
    int op;
    long long a, b, c;
    int it_a = -1;
    iterator_state it;
    bool valid = false;

    while (scanf("%d", &op) != EOF) {
        if (op == 0) {
            if (scanf("%lld%lld", &a, &b) == 2) {
                int old_sz = s[a].size();
                s[a].emplace(b);
                if (s[a].size() > old_sz) {
                    it_a = (int)a;
                    it = s[a].find(b);
                    valid = true;
                }
            }
        } else if (op == 1) {
            if (scanf("%lld%lld", &a, &b) == 2) {
                if (valid && it_a == a && *it == b) {
                    valid = false;
                }
                s[a].erase(b);
            }
        } else if (op == 2) {
            if (scanf("%lld", &a) == 1) {
                s[++main_lst] = s[a];
            }
        } else if (op == 3) {
            if (scanf("%lld%lld", &a, &b) == 2) {
                auto it2 = s[a].find(b);
                if (!it2.path.empty()) {
                    printf("true\n");
                    it_a = (int)a;
                    it = it2;
                    valid = true;
                } else {
                    printf("false\n");
                }
            }
        } else if (op == 4) {
            if (scanf("%lld%lld%lld", &a, &b, &c) == 3) {
                printf("%d\n", s[a].range(b, c));
            }
        } else if (op == 5) {
            if (valid) {
                --it;
                if (it.path.empty()) valid = false;
                else printf("%lld\n", *it);
            } else {
                printf("-1\n");
            }
        } else if (op == 6) {
            if (valid) {
                ++it;
                if (it.path.empty()) {
                    valid = false;
                } else {
                    printf("%lld\n", *it);
                }
            }
        }
    }
    return 0;
}
