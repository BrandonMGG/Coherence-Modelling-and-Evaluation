#include <iostream>
#include <vector>
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

class Cache {
private:
    std::vector<CacheLine> cacheLines;

public:
    Cache(int numLines, int lineSize) {
        cacheLines.reserve(numLines);
        for (int i = 0; i < numLines; ++i) {
            cacheLines.emplace_back(i, 0); // Initialize cache lines with index and tag
        }
    }

    // Retrieve a cache line by index
    CacheLine& getCacheLine(int index) {
        return cacheLines[index];
    }
};

int main() {
    Cache cache(4, 16); // Example cache with 4 lines and 16 bytes per line

    // Simulate cache line state transitions
    CacheLine& line1 = cache.getCacheLine(0);
    line1.read();   // Initial read in Invalid state (read miss)
    line1.setData(42); // Set some data
    line1.transitionToState(new SharedState());
    line1.read();   // Read in Shared state (read hit)
    line1.write();  // Write in Shared state (transition to Invalid)

    CacheLine& line2 = cache.getCacheLine(1);
    line2.transitionToState(new OwnedState());
    line2.read();   // Read in Owned state (read hit)
    line2.write();  // Write in Owned state (write hit)

    // Print cache state
    for (int i = 0; i < 4; ++i) {
        CacheLine& line = cache.getCacheLine(i);
        std::cout << "Cache Line " << i << " Index: " << line.getIndex() << ", Tag: " << line.getTag() << ", Data: " << line.getData() << std::endl;
    }

    return 0;
}
