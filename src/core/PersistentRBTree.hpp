#pragma once
#include "FatNodeStore.hpp"
#include "RBTreeEngine.hpp"
#include <ostream>

struct ITreeCommands {
    virtual void insert(int value) = 0;
    virtual void remove(int value) = 0;
    virtual void printVersion(int v, std::ostream& out) = 0;
    virtual void successor(int x, int v, std::ostream& out) = 0;
    virtual int  currentVersion() const = 0;
    virtual ~ITreeCommands() = default;
};

class PersistentRBTree : public ITreeCommands {
public:
    PersistentRBTree();

    void insert(int value)                          override;
    void remove(int value)                          override;
    void printVersion(int v, std::ostream& out)     override;
    void successor(int x, int v, std::ostream& out) override;
    int  currentVersion() const                     override;

private:
    FatNodeStore store_;
    RBTreeEngine engine_;
    Node*        roots_[MAX_VERSIONS];
    int          curVer_;

    int clamp(int v) const;
};
