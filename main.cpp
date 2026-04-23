#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>

enum Color { RED, BLACK };

struct Node {
    int key;
    Node* origLeft;
    Node* origRight;
    Node* origParent;
    Color origColor;
    // listas de mods por campo, ordenadas por versao crescente → busca binaria O(log m)
    std::vector<std::pair<int, Node*>>  leftMods;
    std::vector<std::pair<int, Node*>>  rightMods;
    std::vector<std::pair<int, Node*>>  parentMods;
    std::vector<std::pair<int, Color>>  colorMods;
};

static const int MAX_VERSIONS = 100;
static const int D = 1000; // limite de mods por no (configuravel)

class PersistentRBTree {
public:
    PersistentRBTree() {
        NIL = new Node();
        NIL->key = 0;
        NIL->origLeft = NIL->origRight = NIL->origParent = NIL;
        NIL->origColor = BLACK;
        for (int i = 0; i < MAX_VERSIONS; ++i) roots[i] = NIL;
        curVer = 0;
    }

    ~PersistentRBTree() {
        for (auto n : allNodes) delete n;
        delete NIL;
    }

    void insert(int value) {
        if (curVer + 1 >= MAX_VERSIONS) return;
        ++curVer;
        roots[curVer] = roots[curVer - 1];

        Node* z = makeNode(value);
        Node* y = NIL;
        Node* x = roots[curVer];

        while (x != NIL) {
            y = x;
            x = (z->key < x->key) ? getLeft(x, curVer) : getRight(x, curVer);
        }

        setParent(z, y, curVer);
        if      (y == NIL)           roots[curVer] = z;
        else if (z->key < y->key)    setLeft(y,  z, curVer);
        else                          setRight(y, z, curVer);

        insertFixUp(z, curVer);
    }

    void remove(int value) {
        if (curVer + 1 >= MAX_VERSIONS) return;
        ++curVer;
        roots[curVer] = roots[curVer - 1];

        Node* z = searchVer(roots[curVer], value, curVer);
        if (z == NIL) return; // nao encontrado: nova versao com mesma raiz

        Node* y     = z;
        Color yOrig = getColor(y, curVer);
        Node* x     = NIL;
        Node* xpar  = NIL;

        if (getLeft(z, curVer) == NIL) {
            x    = getRight(z, curVer);
            xpar = getParent(z, curVer);
            transplant(z, x, curVer);
        } else if (getRight(z, curVer) == NIL) {
            x    = getLeft(z, curVer);
            xpar = getParent(z, curVer);
            transplant(z, x, curVer);
        } else {
            y     = minVer(getRight(z, curVer), curVer);
            yOrig = getColor(y, curVer);
            x     = getRight(y, curVer);

            if (getParent(y, curVer) == z) {
                xpar = y;
            } else {
                xpar = getParent(y, curVer);
                transplant(y, x, curVer);
                setRight(y, getRight(z, curVer), curVer);
                setParent(getRight(y, curVer), y, curVer);
            }
            transplant(z, y, curVer);
            setLeft(y,  getLeft(z, curVer), curVer);
            setParent(getLeft(y, curVer), y, curVer);
            setColor(y, getColor(z, curVer), curVer);
        }

        if (yOrig == BLACK)
            deleteFixUp(x, xpar, curVer);
    }

    void printVersion(int v, std::ostream& fileOut) {
        int tv = clamp(v);
        std::ostringstream buf;
        buf << "IMP " << v << "\n";
        inorder(roots[tv], 0, tv, buf);
        buf << ";";
        emit(buf.str(), fileOut);
    }

    void successor(int x, int v, std::ostream& fileOut) {
        int tv   = clamp(v);
        int best = INT_MAX;
        succHelper(roots[tv], x, tv, best);
        std::ostringstream buf;
        buf << "SUC " << x << " " << v << " ";
        if (best == INT_MAX) buf << "infinito";
        else                 buf << best;
        emit(buf.str(), fileOut);
    }

    int currentVersion() const { return curVer; }

private:
    Node* NIL;
    Node* roots[MAX_VERSIONS];
    int   curVer;
    std::vector<Node*> allNodes;

    // ── getters versionados (busca binaria) ───────────────────

    template<typename T>
    T lastLE(const std::vector<std::pair<int,T>>& mods, int v, T def) const {
        int lo = 0, hi = (int)mods.size() - 1, idx = -1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (mods[mid].first <= v) { idx = mid; lo = mid + 1; }
            else hi = mid - 1;
        }
        return idx >= 0 ? mods[idx].second : def;
    }

    Node* getLeft  (Node* n, int v) const {
        if (n == NIL) return NIL;
        return lastLE(n->leftMods, v, n->origLeft);
    }
    Node* getRight (Node* n, int v) const {
        if (n == NIL) return NIL;
        return lastLE(n->rightMods, v, n->origRight);
    }
    Node* getParent(Node* n, int v) const {
        if (n == NIL) return NIL;
        return lastLE(n->parentMods, v, n->origParent);
    }
    Color getColor (Node* n, int v) const {
        if (n == NIL) return BLACK;
        return lastLE(n->colorMods, v, n->origColor);
    }

    // ── setters versionados (com overflow → copia) ────────────

    int totalMods(Node* n) const {
        return (int)(n->leftMods.size() + n->rightMods.size() +
                     n->parentMods.size() + n->colorMods.size());
    }

    Node* handleOverflow(Node* old, int v) {
        Node* n       = new Node();
        n->key        = old->key;
        n->origLeft   = getLeft(old,   v);
        n->origRight  = getRight(old,  v);
        n->origParent = getParent(old, v);
        n->origColor  = getColor(old,  v);
        allNodes.push_back(n);

        Node* par = n->origParent;
        if      (par == NIL)                  roots[v] = n;
        else if (getLeft(par, v) == old)       setLeft(par,  n, v);
        else                                    setRight(par, n, v);

        if (n->origLeft  != NIL) setParent(n->origLeft,  n, v);
        if (n->origRight != NIL) setParent(n->origRight, n, v);
        return n;
    }

    Node* setLeft  (Node* n, Node* val, int v) {
        if (n == NIL) return NIL;
        n->leftMods.push_back({v, val});
        return (totalMods(n) > D) ? handleOverflow(n, v) : n;
    }
    Node* setRight (Node* n, Node* val, int v) {
        if (n == NIL) return NIL;
        n->rightMods.push_back({v, val});
        return (totalMods(n) > D) ? handleOverflow(n, v) : n;
    }
    Node* setParent(Node* n, Node* val, int v) {
        if (n == NIL) return NIL;
        n->parentMods.push_back({v, val});
        return (totalMods(n) > D) ? handleOverflow(n, v) : n;
    }
    Node* setColor (Node* n, Color val, int v) {
        if (n == NIL) return NIL;
        n->colorMods.push_back({v, val});
        return (totalMods(n) > D) ? handleOverflow(n, v) : n;
    }

    // ── rotacoes ──────────────────────────────────────────────

    void rotateLeft(Node* x, int v) {
        Node* y    = getRight(x, v);
        Node* yL   = getLeft(y, v);
        Node* xPar = getParent(x, v);

        setRight(x, yL, v);
        if (yL != NIL) setParent(yL, x, v);

        setParent(y, xPar, v);
        if      (xPar == NIL)            roots[v] = y;
        else if (x == getLeft(xPar, v))  setLeft(xPar,  y, v);
        else                              setRight(xPar, y, v);

        setLeft(y, x, v);
        setParent(x, y, v);
    }

    void rotateRight(Node* x, int v) {
        Node* y    = getLeft(x, v);
        Node* yR   = getRight(y, v);
        Node* xPar = getParent(x, v);

        setLeft(x, yR, v);
        if (yR != NIL) setParent(yR, x, v);

        setParent(y, xPar, v);
        if      (xPar == NIL)             roots[v] = y;
        else if (x == getRight(xPar, v))  setRight(xPar, y, v);
        else                               setLeft(xPar,  y, v);

        setRight(y, x, v);
        setParent(x, y, v);
    }

    // ── insertFixUp ───────────────────────────────────────────

    void insertFixUp(Node* z, int v) {
        while (getColor(getParent(z, v), v) == RED) {
            Node* par  = getParent(z, v);
            Node* gpar = getParent(par, v);

            if (par == getLeft(gpar, v)) {
                Node* uncle = getRight(gpar, v);
                if (getColor(uncle, v) == RED) {
                    // caso 1
                    setColor(par,   BLACK, v);
                    setColor(uncle, BLACK, v);
                    setColor(gpar,  RED,   v);
                    z = gpar;
                } else {
                    if (z == getRight(par, v)) {
                        // caso 2 → transforma em caso 3
                        z = par;
                        rotateLeft(z, v);
                        par  = getParent(z, v);
                        gpar = getParent(par, v);
                    }
                    // caso 3
                    setColor(par,  BLACK, v);
                    setColor(gpar, RED,   v);
                    rotateRight(gpar, v);
                }
            } else {
                Node* uncle = getLeft(gpar, v);
                if (getColor(uncle, v) == RED) {
                    setColor(par,   BLACK, v);
                    setColor(uncle, BLACK, v);
                    setColor(gpar,  RED,   v);
                    z = gpar;
                } else {
                    if (z == getLeft(par, v)) {
                        z = par;
                        rotateRight(z, v);
                        par  = getParent(z, v);
                        gpar = getParent(par, v);
                    }
                    setColor(par,  BLACK, v);
                    setColor(gpar, RED,   v);
                    rotateLeft(gpar, v);
                }
            }
        }
        setColor(roots[v], BLACK, v);
    }

    // ── transplant versionado ─────────────────────────────────

    void transplant(Node* u, Node* rep, int v) {
        Node* uPar = getParent(u, v);
        if      (uPar == NIL)             roots[v] = rep;
        else if (u == getLeft(uPar, v))   setLeft(uPar,  rep, v);
        else                               setRight(uPar, rep, v);
        setParent(rep, uPar, v);
    }

    // ── deleteFixUp ───────────────────────────────────────────

    void deleteFixUp(Node* x, Node* xpar, int v) {
        while (x != roots[v] && getColor(x, v) == BLACK) {
            Node* par = (x != NIL) ? getParent(x, v) : xpar;
            if (par == NIL) break;

            if (x == getLeft(par, v)) {
                Node* w = getRight(par, v);
                if (getColor(w, v) == RED) {
                    // caso 1
                    setColor(w,   BLACK, v);
                    setColor(par, RED,   v);
                    rotateLeft(par, v);
                    xpar = par;
                    par  = (x != NIL) ? getParent(x, v) : xpar;
                    w    = getRight(par, v);
                }
                if (getColor(getLeft(w,  v), v) == BLACK &&
                    getColor(getRight(w, v), v) == BLACK) {
                    // caso 2
                    setColor(w, RED, v);
                    x    = par;
                    xpar = getParent(x, v);
                } else {
                    if (getColor(getRight(w, v), v) == BLACK) {
                        // caso 3
                        setColor(getLeft(w, v), BLACK, v);
                        setColor(w, RED, v);
                        rotateRight(w, v);
                        par = (x != NIL) ? getParent(x, v) : xpar;
                        w   = getRight(par, v);
                    }
                    // caso 4
                    setColor(w, getColor(par, v), v);
                    setColor(par, BLACK, v);
                    setColor(getRight(w, v), BLACK, v);
                    rotateLeft(par, v);
                    x = roots[v];
                }
            } else {
                Node* w = getLeft(par, v);
                if (getColor(w, v) == RED) {
                    setColor(w,   BLACK, v);
                    setColor(par, RED,   v);
                    rotateRight(par, v);
                    xpar = par;
                    par  = (x != NIL) ? getParent(x, v) : xpar;
                    w    = getLeft(par, v);
                }
                if (getColor(getRight(w, v), v) == BLACK &&
                    getColor(getLeft(w,  v), v) == BLACK) {
                    setColor(w, RED, v);
                    x    = par;
                    xpar = getParent(x, v);
                } else {
                    if (getColor(getLeft(w, v), v) == BLACK) {
                        setColor(getRight(w, v), BLACK, v);
                        setColor(w, RED, v);
                        rotateLeft(w, v);
                        par = (x != NIL) ? getParent(x, v) : xpar;
                        w   = getLeft(par, v);
                    }
                    setColor(w, getColor(par, v), v);
                    setColor(par, BLACK, v);
                    setColor(getLeft(w, v), BLACK, v);
                    rotateRight(par, v);
                    x = roots[v];
                }
            }
        }
        setColor(x, BLACK, v);
    }

    // ── utilidades ────────────────────────────────────────────

    Node* makeNode(int key) {
        Node* n       = new Node();
        n->key        = key;
        n->origLeft   = NIL;
        n->origRight  = NIL;
        n->origParent = NIL;
        n->origColor  = RED;
        allNodes.push_back(n);
        return n;
    }

    Node* searchVer(Node* r, int value, int v) const {
        while (r != NIL) {
            if      (value == r->key) return r;
            else if (value <  r->key) r = getLeft(r, v);
            else                       r = getRight(r, v);
        }
        return NIL;
    }

    Node* minVer(Node* x, int v) const {
        while (getLeft(x, v) != NIL) x = getLeft(x, v);
        return x;
    }

    int clamp(int v) const {
        if (v < 0) return 0;
        return (v > curVer) ? curVer : v;
    }

    void inorder(Node* n, int depth, int v, std::ostringstream& buf) const {
        if (n == NIL) return;
        inorder(getLeft(n, v), depth + 1, v, buf);
        buf << n->key << "," << depth << "," << (getColor(n, v) == RED ? "R" : "N") << " ";
        inorder(getRight(n, v), depth + 1, v, buf);
    }

    void succHelper(Node* n, int x, int v, int& best) const {
        if (n == NIL) return;
        if (n->key > x) {
            if (n->key < best) best = n->key;
            succHelper(getLeft(n, v), x, v, best);
        } else {
            succHelper(getRight(n, v), x, v, best);
        }
    }

    void emit(const std::string& s, std::ostream& fileOut) const {
        fileOut   << s << "\n";
        std::cout << s << "\n";
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

    PersistentRBTree tree;
    std::string line;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "INC") {
            int val; ss >> val;
            tree.insert(val);
        } else if (cmd == "REM") {
            int val; ss >> val;
            tree.remove(val);
        } else if (cmd == "IMP") {
            int ver;
            if (ss >> ver) tree.printVersion(ver, outFile);
            else           tree.printVersion(tree.currentVersion(), outFile);
        } else if (cmd == "SUC") {
            int val, ver; ss >> val >> ver;
            tree.successor(val, ver, outFile);
        }
    }

    return 0;
}
