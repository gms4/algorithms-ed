#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <queue>
#include <limits>
#include <string>
#include <bits/stdc++.h>

using namespace std;

//variables
long long generator_seed; //pseudoaleatory generator seed
long long int keyUniverse_size, burnin_op, total_op, insert_frequency, query_frequency, print_frequency; //input values
long long int depth = 0;
bool insertOp;

//random generator implementation
struct RNG {
    long long int A = 1664525, C = 1013904223, R = pow(2, 32), xKn = -1;
    long long int next() {
        return xKn == -1 ? xKn = generator_seed : xKn = (long long int)(A * xKn + C) % R;
    }
};

//building the tree's node
class node {
    public:
    long long int key; //node's key
    node *left, *right, *parent;    
};

//now we're creating and returning a new node, allocated dinamically in the memory
node* newNode(long long int key) {
    node* node = new class node;
    node -> key = key;
    node -> left = NULL;
    node -> right = NULL;
    node -> parent = NULL;
    return node;
};

/*
Splay tree is a self organized BST
They contain a method called splaying, to self balance the tree
The nodes with the most queries are closer to the tree's root.
If node x is consulted, it ascends (splayed) to the root. If not, it'll
be inserted as a new node in a leaf and then splayed to the root.
*/
class ST {
    public:
    node* root;
    bool exists;

    /*
    Following the classes notes:
    Rotates left around X
    Returns the new recently placed node
    */
    node* rotate_left(node* X) {
        //setting the nodes pointers
        node *P = X -> parent; //points to X's parent
        node *R = X -> right; //points to the new tree's node's root. it was X's right subtree before
        node *RL = R -> left; //points to R's left subtree, that I'll be X's right subtree

        X -> right = RL;
        if (RL) {
            RL -> parent = X;
        }
        //reorganizing the splay tree after left rotation
        R -> left = X; //R's left child is now X
        X -> parent = R; //R is now X's parent
        R -> parent = P; //P is now R's parent

        //verifying if X is P's left or right child
       if (P && X == P -> left) {
            P -> left = R;
        } else if (P && X == P -> right) {
            P -> right = R;
        }
        return R;
    };
    /*
    Following the classes notes:
    Rotates right around X
    Returns the new recently placed node
    */
    node* rotate_right(node* X) {
        node *P = X -> parent; //points to X's parent
        node *L = X -> left; //points to the new tree's node's root. it was X's left subtree before
        node *LR = L -> right; //points to L's right subtree, that I'll be X's left subtree

        X -> left = LR;
        if (LR) {
            LR -> parent = X;
        }

        //reorganizing the splay tree after right rotation
        L -> right = X; //L's right child is now X
        X -> parent = L; //L is now X's parent
        L -> parent = P; //P is now L's parent

        //verifying if X is P's left or right child
       if (P && X == P -> left) {
            P -> left = L;
        } else if (P && X == P -> right) {
            P -> right = L;
        }
        return L;
    };

    /*
    here we have some methods of the splay tree that almost mimics
    what happens in a BST. in a splay tree, the insertion and search are 
    performed in a similar way, but after each operation, the accessed node 
    (inserted or searched) is moved to the root of the tree through 
    a series of rotations. This technique aims to keep frequently accessed nodes close 
    to the root of the tree, improving the performance of future operations.
    */
    node* BST_Search(node* root, long long int key, long long int &depth) {
    /*
    If the key being searched is smaller than the current key of the root, 
    the search continues in the left subtree; otherwise, the search continues 
    recursively in the right subtree.
    */

    if (!root) {
            return NULL;
        } else if (root -> key == key) {
            return root;
        } else if (key < root -> key) {
            depth++;
            return BST_Search(root -> left, key, depth);
        } else {
            depth++;
            return BST_Search(root -> right, key, depth);
        }
    };

    node* BST_Insert(node* root, node* new_node, node* parent,long long int &depth) {
        if (!root) { //node is null -> returns new node, which is actually the new root
            new_node -> parent = parent;
            return new_node;
        } else if (new_node -> key > root -> key) { //search continues at right's subtree
            depth++;
            root -> right = BST_Insert(root -> right, new_node, root, depth);
        } else if (new_node -> key < root -> key) { //search continues at left's subtree
            depth++;
            root -> left = BST_Insert(root -> left, new_node, root, depth);
        } else { //node has the same key as the root node 
            exists = true;
        }
        return root;
    };

    node* zig(node* X) {
        node* P = X -> parent;
        /*
        If X is equal to P -> left, then rotate_right(P) is called; 
        otherwise, rotate_left(P) is called. If P is NULL, the function returns NULL.
        */
        return (P) ? ((X == P -> left) ? rotate_right(P) : rotate_left(P)) : NULL;
}
    /*
    The zigzag performs two rotations to bring a node up two levels in the tree. 
    It checks the relationship between the node, its parent, and grandparent to 
    determine which rotations to perform
    */
    node* zigzag(node* X) {

        node* P = X -> parent; //points to X's parent
        node* G = P -> parent; //points to P's parent

        if (P && G) {
            if (P == G -> left) {
                if (X == P -> left) {
                    rotate_right(G);
                    return rotate_right(P);
                } else {
                    rotate_left(P);
                    return rotate_right(G);
                }
            } else {
                if (X == P -> right) {
                    rotate_left(G);
                    return rotate_left(P);
                } else {
                    rotate_right(P);
                    return rotate_left(G);
                }
            }
        }
    };

    /*
    It brings the accessed node to the root of the tree by performing 
    zig and zigzag operations. It takes a node X as input and repeatedly 
    applies zig and zigzag operations until X becomes the root of the tree, then returns X
    */
    node* splay(node* X) {
    while (X -> parent != NULL) {
        node* G = X -> parent -> parent;
        X = (G) ? zigzag(X) : zig(X);
    }
    return X;
};

    /*
    This one I just followed what we learned on the class notes
    It searches for a node with the given key. If it is found, it 
    splays the node to the root and returns it. Otherwise, 
    it returns null. The function also updates the depth parameter passed by reference.
    */
    node* STFind(long long int key, long long int &depth){
        node* X = BST_Search(root, key, depth);
        if (X != NULL) {
            splay(X);
            this -> root = X;
        }
        return X;
    };

    /*
    it inserts a new node with a given key, and returns 
    the new node if it was successfully inserted
    */
    node* STInsert(long long int key, long long int &depth){
        node* new_node = newNode(key);
        node* refreshedTree = BST_Insert(root, new_node, NULL, depth);
        if (!this -> exists) {
            this -> root = refreshedTree;
            return new_node;
        } 
        this -> exists = false;
        return NULL;
    };
        
};

//main code
int main() {

   //sync_with_stdio(false) turns off I/O flux sincronization between CPP and SO
    ios_base::sync_with_stdio(false);

    /*cin.tie(NULL) and cout.tie(NULL) lines
    disassociate the cin and cout streams from the standard input/output streams,
    respectively. This disassociation is important to prevent the program from stalling
    if there's an input/output operation on one stream while the other stream is waiting
    for input/output operations.*/
    cin.tie(NULL);
    cout.tie(NULL);
    node* new_insert = nullptr;
    ST splay = ST();
    while (std::cin >> generator_seed >> keyUniverse_size >> burnin_op >> total_op >> insert_frequency >> query_frequency >> print_frequency) { 
        RNG randomNum;
        for (int i = 0; i < burnin_op; ++i) {
            long long int nextRandom = randomNum.next() % keyUniverse_size;
            splay.STInsert(nextRandom, depth);
        }

        for (int i = 0; i < total_op; ++i) {
            long long int nextRandom = randomNum.next();
            insertOp = nextRandom % (insert_frequency + query_frequency) < insert_frequency;
            switch (insertOp) {
                case true:
                    depth = 0;
                    nextRandom = randomNum.next() % keyUniverse_size;
                    new_insert = splay.STInsert(nextRandom, depth);
                    if (new_insert == NULL) {
                        depth = -1;
                    }
                    if (i % print_frequency == 0) {
                        std::cout << "I " << nextRandom << " " << depth << std::endl;
                    }
                    break;
                default:
                    depth = 0;
                    nextRandom = randomNum.next() % keyUniverse_size;
                    node* queryOne = splay.STFind(nextRandom, depth);
                    if (queryOne == NULL) {
                        depth = -1;
                    }
                    if (i % print_frequency == 0) {
                        std::cout << "Q " << nextRandom << " " << depth << std::endl;
                    }
                    break;
            }

        }
    }
}