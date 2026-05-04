#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MSG_1 "Tree is empty!"

typedef enum {
    BLACK,
    RED
} NodeColor;

typedef struct Node {
    int data;
    int count;
    NodeColor color;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

typedef struct {
    Node* root;
    Node* NIL;
} RBTree;

bool empty(RBTree* tree) {
    return (tree == NULL || tree->root == tree->NIL);
}

Node* createNIL() {
    Node* nil = (Node*)malloc(sizeof(Node));
    if (nil == NULL) return NULL;
    
    nil->color = BLACK;
    nil->data = 0;
    nil->left = nil;
    nil->right = nil;
    nil->parent = nil;

    return nil;
}

Node* createNode(RBTree* tree, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) return NULL;
    
    new_node->data = value;
    new_node->count = 1;
    new_node->color = RED;
    new_node->left = tree->NIL;
    new_node->right = tree->NIL;
    new_node->parent = tree->NIL;

    return new_node;
}

void right_rotate(RBTree* tree, Node* y) {
    Node* x = y->left;

    y->left = x->right;
    if (y->left != tree->NIL)
        y->left->parent = y;

    x->parent = y->parent;
    if (y->parent == tree->NIL)
        tree->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    x->right = y;
    y->parent = x;
}

void left_rotate(RBTree* tree, Node* x) {
    Node* y = x->right;

    x->right = y->left;
    if (x->right != tree->NIL)
        x->right->parent = x;

    y->parent = x->parent;
    if (x->parent == tree->NIL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void insertFixup(RBTree* tree, Node* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node* uncle = z->parent->parent->right;

            if (uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }

            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        }

        else {
            Node* uncle = z->parent->parent->left;

            if (uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }

            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }

    tree->root->color = BLACK;
}

void treeInsert(RBTree* tree, int value) {
    Node* current = tree->root;
    Node* parent = tree->NIL;

    while (current != tree->NIL && current->data != value) {
        parent = current;
        if (value < current->data)
            current = current->left;
        else
            current = current->right;
    }

    if (current != tree->NIL) {
        current->count++;
        return;
    }

    Node* z = createNode(tree, value);
    if (z == NULL) return;

    z->parent = parent;
    if (parent == tree->NIL)
        tree->root = z;
    else if (value < parent->data)
        parent->left = z;
    else
        parent->right = z;

    insertFixup(tree, z);
}

Node* nodeSearch(RBTree* tree, int value) {
    Node* current = tree->root;

    while (current != tree->NIL && current->data != value)
        if (value < current->data)
            current = current->left;
        else
            current = current->right;

    return current;
}

void treeTransplant(RBTree* tree, Node* u, Node* v) {
    if (u->parent == tree->NIL)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

Node* getSucessor(RBTree* tree, Node* node) {
    Node* current = node;
    while (current->left != tree->NIL)
        current = current->left;
    return current;
}

void removeFixup(RBTree* tree, Node* x) {
    Node* bro;
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            bro = x->parent->right;

            if (bro->color == RED) {
                bro->color = BLACK;
                x->parent->color = RED;
                left_rotate(tree, x->parent);
                bro = x->parent->right;
            }

            if (bro->left->color == BLACK && bro->right->color == BLACK) {
                bro->color = RED;
                x = x->parent;
            }

            else {
                if (bro->right->color == BLACK) {
                    bro->color = RED;
                    bro->left->color = BLACK;
                    right_rotate(tree, bro);
                    bro = x->parent->right;
                }

                bro->color = x->parent->color;
                x->parent->color = BLACK;
                bro->right->color = BLACK;
                left_rotate(tree, x->parent);

                x = tree->root;
            }
        }

        else {
            bro = x->parent->left;

            if (bro->color == RED) {
                bro->color = BLACK;
                x->parent->color = RED;
                right_rotate(tree, x->parent);
                bro = x->parent->left;
            }

            if (bro->left->color == BLACK && bro->right->color == BLACK) {
                bro->color = RED;
                x = x->parent;
            }

            else {
                if (bro->left->color == BLACK) {
                    bro->color = RED;
                    bro->right->color = BLACK;
                    left_rotate(tree, bro);
                    bro = x->parent->left;
                }

                bro->color = x->parent->color;
                x->parent->color = BLACK;
                bro->left->color = BLACK;
                right_rotate(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->color = BLACK;
}

void treeRemove(RBTree* tree, Node* z) {
    Node* y = z;
    Node* x;
    NodeColor y_original_color = y->color;

    if (z->left == tree->NIL) {
        x = z->right;
        treeTransplant(tree, z, x);
    } else if (z->right == tree->NIL) {
        x = z->left;
        treeTransplant(tree, z, x);
    } else {
        y = getSucessor(tree, z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x->parent = y;
        } else {
            treeTransplant(tree, y, x);
            y->right = z->right;
            y->right->parent = y;
        }

        treeTransplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z);

    if (y_original_color == BLACK)
        removeFixup(tree, x);
}

bool removeValue(RBTree* tree, int value) {
    if (empty(tree)) return false;

    Node* target = nodeSearch(tree, value);
    
    if (target == tree->NIL)
        return false;
    else if (target->count > 1)
        target->count--;
    else
        treeRemove(tree, target);

    return true;
}

void _inOrder(RBTree* tree, Node* node) {
    if (node != tree->NIL) {
        _inOrder(tree, node->left);
        printf("%d ", node->data);
        _inOrder(tree, node->right);
    }
}

void inOrder(RBTree* tree) {
    if (!empty(tree)) _inOrder(tree, tree->root);
    else printf(MSG_1);
    printf("\n");
}

void _preOrder(RBTree* tree, Node* node) {
    if (node != tree->NIL) {
        printf("%d ", node->data);
        _preOrder(tree, node->left);
        _preOrder(tree, node->right);
    }
}

void preOrder(RBTree* tree) {
    if (!empty(tree)) _preOrder(tree, tree->root);
    else printf(MSG_1);
    printf("\n");
}

void _postOrder(RBTree* tree, Node* node) {
    if (node != tree->NIL) {
        _postOrder(tree, node->left);
        _postOrder(tree, node->right);
        printf("%d ", node->data);
    }
}

void postOrder(RBTree* tree) {
    if (!empty(tree)) _postOrder(tree, tree->root);
    else printf(MSG_1);
    printf("\n");
}

RBTree* createTree() {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    if (tree == NULL) return NULL;

    tree->NIL = createNIL();
    if (tree->NIL == NULL) {
        free(tree);
        return NULL;
    }
    tree->root = tree->NIL;

    return tree;
}

void deleteNode(RBTree* tree, Node* node) {
    if (node != tree->NIL) {
        deleteNode(tree, node->left);
        deleteNode(tree, node->right);
        free(node);
    }
}

void deleteTree(RBTree* tree) {
    if (tree != NULL) {
        deleteNode(tree, tree->root);
        free(tree->NIL);
        free(tree);
    }
}

int main() {
    return 0;
}
