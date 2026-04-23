#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

enum Color { RED, BLACK };

struct Node {
    int   value;
    Color color;
    Node* parent;
    Node* left;
    Node* right;
};

class RBTree {
public:
    RBTree() {
        NIL = new Node{0, BLACK, nullptr, nullptr, nullptr};
        NIL->parent = NIL;
        NIL->left   = NIL;
        NIL->right  = NIL;
        root = NIL;
    }

    ~RBTree() {
        destroyTree(root);
        delete NIL;
    }

    void insert(int value) {
        Node* z = new Node{value, RED, NIL, NIL, NIL};
        Node* y = NIL;
        Node* x = root;

        while (x != NIL) {
            y = x;
            if (z->value < x->value)
                x = x->left;
            else
                x = x->right;
        }

        z->parent = y;

        if (y == NIL)
            root = z;
        else if (z->value < y->value)
            y->left = z;
        else
            y->right = z;

        insertFixUp(z);
    }

    void remove(int value) {
        Node* z = search(root, value);
        if (z == NIL)
            return; // valor nao encontrado, nao altera estrutura

        Node* y = z;
        Node* x = NIL;
        Color yOriginalColor = y->color;

        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;

        if (yOriginalColor == BLACK)
            deleteFixUp(x);
    }

    void printInOrder(std::ostream& fileOut) const {
        std::ostringstream buf;
        inOrderHelper(root, 0, buf);
        buf << ";";
        std::string result = buf.str();
        fileOut << result << "\n";
        std::cout  << result << "\n";
    }

private:
    Node* root;
    Node* NIL;

    Node* search(Node* node, int value) const {
        while (node != NIL) {
            if (value == node->value)
                return node;
            else if (value < node->value)
                node = node->left;
            else
                node = node->right;
        }
        return NIL;
    }

    Node* minimum(Node* x) const {
        while (x->left != NIL)
            x = x->left;
        return x;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == NIL)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;

        if (y->left != NIL)
            y->left->parent = x;

        y->parent = x->parent;

        if (x->parent == NIL)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left   = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        x->left = y->right;

        if (y->right != NIL)
            y->right->parent = x;

        y->parent = x->parent;

        if (x->parent == NIL)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;

        y->right  = x;
        x->parent = y;
    }

    void insertFixUp(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right; // tio
                if (y->color == RED) {
                    // Caso 1: tio vermelho
                    z->parent->color          = BLACK;
                    y->color                  = BLACK;
                    z->parent->parent->color  = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        // Caso 2: tio preto, z filho direito
                        z = z->parent;
                        rotateLeft(z);
                    }
                    // Caso 3: tio preto, z filho esquerdo
                    z->parent->color         = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else {
                // Simetrico
                Node* y = z->parent->parent->left; // tio
                if (y->color == RED) {
                    // Caso 1
                    z->parent->color          = BLACK;
                    y->color                  = BLACK;
                    z->parent->parent->color  = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        // Caso 2
                        z = z->parent;
                        rotateRight(z);
                    }
                    // Caso 3
                    z->parent->color         = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void deleteFixUp(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right; // irmao
                if (w->color == RED) {
                    // Caso 1: irmao vermelho
                    w->color          = BLACK;
                    x->parent->color  = RED;
                    rotateLeft(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    // Caso 2: irmao preto, ambos filhos pretos
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        // Caso 3: irmao preto, filho direito preto
                        w->left->color = BLACK;
                        w->color       = RED;
                        rotateRight(w);
                        w = x->parent->right;
                    }
                    // Caso 4: irmao preto, filho direito vermelho
                    w->color          = x->parent->color;
                    x->parent->color  = BLACK;
                    w->right->color   = BLACK;
                    rotateLeft(x->parent);
                    x = root;
                }
            } else {
                // Simetrico
                Node* w = x->parent->left; // irmao
                if (w->color == RED) {
                    // Caso 1
                    w->color          = BLACK;
                    x->parent->color  = RED;
                    rotateRight(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    // Caso 2
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        // Caso 3
                        w->right->color = BLACK;
                        w->color        = RED;
                        rotateLeft(w);
                        w = x->parent->left;
                    }
                    // Caso 4
                    w->color          = x->parent->color;
                    x->parent->color  = BLACK;
                    w->left->color    = BLACK;
                    rotateRight(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    void inOrderHelper(Node* node, int depth, std::ostringstream& buf) const {
        if (node == NIL)
            return;
        inOrderHelper(node->left, depth + 1, buf);
        buf << node->value << "," << depth << "," << (node->color == RED ? "R" : "N") << " ";
        inOrderHelper(node->right, depth + 1, buf);
    }

    void destroyTree(Node* node) {
        if (node == NIL)
            return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
};

int main() {
    std::ifstream inFile("entrada.txt");
    std::ofstream outFile("saida.txt");

    if (!inFile.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir entrada.txt\n";
        return 1;
    }
    if (!outFile.is_open()) {
        std::cerr << "Erro: nao foi possivel criar saida.txt\n";
        return 1;
    }

    RBTree tree;
    std::string line;

    while (std::getline(inFile, line)) {
        if (line.empty())
            continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "INC") {
            int val;
            ss >> val;
            tree.insert(val);
        } else if (cmd == "REM") {
            int val;
            ss >> val;
            tree.remove(val);
        } else if (cmd == "IMP") {
            tree.printInOrder(outFile);
        }
    }

    inFile.close();
    outFile.close();
    return 0;
}
