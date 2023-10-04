#include <iostream>
#include <string>

class CacheLineState {
public:
    virtual void read() = 0;
    virtual void write() = 0;
};

class InvalidState : public CacheLineState {
public:
    void read() override {
        std::cout << "Cache line is in Invalid state. Perform a read miss." << std::endl;
    }

    void write() override {
        std::cout << "Cache line is in Invalid state. Perform a write miss." << std::endl;
    }
};

class SharedState : public CacheLineState {
public:
    void read() override {
        std::cout << "Cache line is in Shared state. Perform a read hit." << std::endl;
    }

    void write() override {
        std::cout << "Cache line is in Shared state. Transitioning to Invalid state on write." << std::endl;
    }
};

class OwnedState : public CacheLineState {
public:
    void read() override {
        std::cout << "Cache line is in Owned state. Perform a read hit." << std::endl;
    }

    void write() override {
        std::cout << "Cache line is in Owned state. Perform a write hit." << std::endl;
    }
};

class ModifiedState : public CacheLineState {
public:
    void read() override {
        std::cout << "Cache line is in Modified state. Perform a read hit." << std::endl;
    }

    void write() override {
        std::cout << "Cache line is in Modified state. Perform a write hit." << std::endl;
    }
};

class CacheLine {
private:
    CacheLineState* currentState;
    int index;
    int tag;
    int data;

public:
    CacheLine(int index, int tag) : index(index), tag(tag), data(0) {
        currentState = new InvalidState();
    }

    void transitionToState(CacheLineState* newState) {
        std::cout << "Transitioning from " << typeid(*currentState).name() << " to " << typeid(*newState).name() << std::endl;
        delete currentState;
        currentState = newState;
    }

    void read() {
        currentState->read();
    }

    void write() {
        currentState->write();
    }

    int getIndex() const {
        return index;
    }

    int getTag() const {
        return tag;
    }

    int getData() const {
        return data;
    }

    void setData(int newData) {
        data = newData;
    }
};

int main() {
    CacheLine cacheLine(1, 123); // Example cache line with index 1 and tag 123

    // Simulate cache line state transitions
    cacheLine.read();   // Initial read in Invalid state (read miss)
    cacheLine.setData(42); // Set some data
    cacheLine.transitionToState(new SharedState());
    cacheLine.read();   // Read in Shared state (read hit)
    cacheLine.write();  // Write in Shared state (transition to Invalid)
    cacheLine.transitionToState(new OwnedState());
    cacheLine.read();   // Read in Owned state (read hit)
    cacheLine.write();  // Write in Owned state (write hit)

    std::cout << "Cache Line Index: " << cacheLine.getIndex() << std::endl;
    std::cout << "Cache Line Tag: " << cacheLine.getTag() << std::endl;
    std::cout << "Cache Line Data: " << cacheLine.getData() << std::endl;

    return 0;
}
