/*
 * C++ AVL Tree implementation
 * Operations:
 *   - insert(k, v) O(log N)
 *   - erase(k)     O(log N)
 *   - find(k)      O(log N)
 *   - prec(k)      O(log N)
 *   - succ(k)      O(log N)
 */

#include <cstdint>
#include <cstdlib>
#include <variant>
#include <utility>
#include <optional>
#include <tuple>
using namespace std;

template<typename K, typename T>
class AVLTree {
    struct Node {
        K key;
        T val;

        int h;
        Node *lc, *rc, *par;

        Node(K key, T val, Node *par) :
            key(key), val(val), par(par), h(1), lc(), rc() { }
    } *root;

    int get_h(Node *node) { return node == NULL ? 0 : node->h; }
    int get_bf(Node *node) { return node == NULL ? 0 : get_h(node->rc) - get_h(node->lc); }

    void update(Node *node) {
        if (node == NULL) return;
        node->h = 1 + max(get_h(node->lc), get_h(node->rc));
    }

    T find(const K &key, Node *node) {
        if (node == NULL) return T();
        if (key < node->key) return find(key, node->lc);
        if (key > node->key) return find(key, node->rc);
        return node->val;
    }

    bool count(const K &key, Node *node) {
        if (node == NULL) return false;
        if (key < node->key) return count(key, node->lc);
        if (key > node->key) return count(key, node->rc);
        return true;
    }

    void left_rot(Node *node) {
        Node *par = node->par;
        Node *rc = node->rc;

        node->rc = rc->lc;
        if (node->rc != NULL) node->rc->par = node;

        rc->lc = node;
        node->par = rc;

        rc->par = par;
        if (par == NULL) {
            root = rc;
        } else if (par->lc == node) {
            par->lc = rc;
        } else {
            par->rc = rc;
        }

        update(node);
        update(rc);
        update(par);
    }

    void right_rot(Node *node) {
        Node *par = node->par;
        Node *lc = node->lc;

        node->lc = lc->rc;
        if (node->lc != NULL) node->lc->par = node;

        lc->rc = node;
        node->par = lc;

        lc->par = par;
        if (par == NULL) {
            root = lc;
        } else if (par->lc == node) {
            par->lc = lc;
        } else {
            par->rc = lc;
        }

        update(node);
        update(lc);
        update(par);
    }

    void balance(Node *node) {
        update(node);

        if (get_bf(node) == -2) {
            if (get_bf(node->lc) != +1) {
                right_rot(node);
            } else {
                left_rot(node->lc);
                right_rot(node);
            }
        } else if (get_bf(node) == +2) {
            if (get_bf(node->rc) != -1) {
                left_rot(node);
            } else {
                right_rot(node->rc);
                left_rot(node);
            }
        }
    }

    void insert(const K &key, const T &val, Node *node) {
        if (node->key == key) return void(node->val = val);

        if (key < node->key) {
            if (node->lc == NULL) {
                node->lc = new Node(key, val, node);
            } else {
                insert(key, val, node->lc);
            }
        } else {
            if (node->rc == NULL) {
                node->rc = new Node(key, val, node);
            } else {
                insert(key, val, node->rc);
            }
        }

        balance(node);
    }

    void erase(const K &key, Node *node, Node *elem = NULL) {
        if (node == NULL) return;

        if (elem != NULL) {
            if (node->lc != NULL) {
                erase(key, node->lc, elem);
            } else {
                tie(elem->key, elem->val) = tie(node->key, node->val);
                
                if (node->rc == NULL) {
                    if (node->par == NULL) {
                        root = NULL;
                    } else if (node->par->lc == node) {
                        node->par->lc = NULL;
                    } else {
                        node->par->rc = NULL;
                    }
                    delete node;
                    return;
                } else {
                    Node *n = node->rc;

                    node->key = n->key;
                    node->val = n->val;
                    node->lc = n->lc;
                    node->rc = n->rc;

                    if (node->lc != NULL) {
                        node->lc->par = node;
                    }
                    if (node->rc != NULL) {
                        node->rc->par = node;
                    }

                    delete n;
                }
            }
        } else if (key == node->key) {
            if (node->lc == NULL && node->rc == NULL) {
                if (node->par == NULL) {
                    root = NULL;
                } else if (node->par->lc == node) {
                    node->par->lc = NULL;
                } else {
                    node->par->rc = NULL;
                }
                delete node;
                return;
            } else if (node->lc == NULL) {
                Node *n = node->rc;

                node->key = n->key;
                node->val = n->val;
                node->lc = n->lc;
                node->rc = n->rc;

                if (node->lc != NULL) {
                    node->lc->par = node;
                }
                if (node->rc != NULL) {
                    node->rc->par = node;
                }

                delete n;
            } else if (node->rc == NULL) {
                Node *n = node->lc;

                node->key = n->key;
                node->val = n->val;
                node->lc = n->lc;
                node->rc = n->rc;

                if (node->lc != NULL) {
                    node->lc->par = node;
                }
                if (node->rc != NULL) {
                    node->rc->par = node;
                }

                delete n;
            } else if (node->rc->lc == NULL) {
                Node *n = node->rc;

                node->key = n->key;
                node->val = n->val;
                node->rc = n->rc;

                if (node->rc != NULL) {
                    node->rc->par = node;
                }

                delete n;
            } else {
                erase(key, node->rc, node);
            }
        } else if (key < node->key) {
            erase(key, node->lc, elem);
        } else {
            erase(key, node->rc, elem);
        }

        balance(node);
    }

    optional<pair<K, T>> succ(const K &key, Node *node) {
        if (node == NULL) return {};
        if (key == node->key) return {{node->key, node->val}};
        if (key < node->key) {
            optional<pair<K, T>> r = succ(key, node->lc);
            if (!r) return {{node->key, node->val}};
            return min({node->key, node->val}, r.value());
        }
        return succ(key, node->rc);
    }

    optional<pair<K, T>> prec(const K &key, Node *node) {
        if (node == NULL) return {};
        if (key == node->key) return {{node->key, node->val}};
        if (key > node->key) {
            optional<pair<K, T>> r = prec(key, node->rc);
            if (!r) return {{node->key, node->val}};
            return max({node->key, node->val}, r.value());
        }
        return prec(key, node->lc);
    }

    void free_memory(Node *node) {
        if (node == NULL) return;
        free_memory(node->lc);
        free_memory(node->rc);
        delete node;
    }

public:

    T find(const K &key) { return find(key, root); }
    bool count(const K &key) { return count(key, root); }

    void insert(const K &key, const T &val) {
        if (root == NULL) {
            root = new Node(key, val, NULL);
        } else {
            insert(key, val, root);
        }
    }

    void erase(const K &key) { erase(key, root); }

    optional<pair<K, T>> succ(const K &key) { return succ(key, root); }
    optional<pair<K, T>> prec(const K &key) { return prec(key, root); }

    AVLTree() : root() { }
    ~AVLTree() { free_memory(root); }
};
