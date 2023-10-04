#include <iostream>
#include <vector>
#include <map>
#include <iostream>

// Forward declarations
class Cache;

// Memory class to represent system memory
class Memory {
private:
    std::map<int, int> data; // Memory address to data mapping

public:
    int read(int address) {
        // Simulate reading data from memory
        return data[address];
    }

    void write(int address, int value) {
        // Simulate writing data to memory
        data[address] = value;
    }
};

// CacheLine class representing cache lines/Blocks
class CacheLine {
public:
    enum State { INVALID, SHARED, MODIFIED, EXCLUSIVE };

    State state;
    int tag;
    int data;

    CacheLine() : state(INVALID), tag(-1), data(0) {}
};

// Cache class representing a cache
class Cache {
private:
    std::vector<CacheLine> cacheLines;
    Memory& memory;

public:
    Cache(int numLines, Memory& mem) : memory(mem) {
        cacheLines.resize(numLines);
    }

    // Handle read operation
    int read(int address, int processorId) {
        int index = address % cacheLines.size();
        CacheLine& line = cacheLines[index];

        if (line.tag == address) {
            // Cache hit
            std::cout << "Cache " << processorId << " - Read HIT (Address " << address << "): " << line.data << std::endl;
        } else {
            // Cache miss
            std::cout << "Cache " << processorId << " - Read MISS (Address " << address << "): Fetching data from memory." << std::endl;
            line.tag = address;
            line.data = memory.read(address);

            // Update other caches
            line.state = CacheLine::SHARED;
            notifyOtherCaches(address, processorId);
        }

        return line.data;
    }

    // Handle write operation
    void write(int address, int value, int processorId) {
        int index = address % cacheLines.size();
        CacheLine& line = cacheLines[index];

        if (line.tag == address) {
            // Cache hit
            std::cout << "Cache " << processorId << " - Write HIT (Address " << address << "): Updating data." << std::endl;
            line.data = value;
            line.state = CacheLine::MODIFIED;

            // Update other caches
            notifyOtherCaches(address, processorId);
        } else {
            // Cache miss
            std::cout << "Cache " << processorId << " - Write MISS (Address " << address << "): Writing data to memory." << std::endl;
            memory.write(address, value);

            // Update other caches
            line.tag = address;
            line.data = value;
            line.state = CacheLine::MODIFIED;
            notifyOtherCaches(address, processorId);
        }
    }

    // Notify other caches of a change in memory
    void notifyOtherCaches(int address, int processorId) {
        // Simulated broadcast on a bus
        std::cout << "Cache " << processorId << " notifying other caches of change (Address " << address << ")." << std::endl;
    }
};

int main() {
    Memory memory;
    Cache cacheA(4, memory);
    Cache cacheB(4, memory);
    Cache cacheC(4, memory);

    // Simulate read and write operations in Cache A
    cacheA.write(100, 42, 1);
    cacheA.read(100, 1);

    // Simulate read operation in Cache B
    cacheB.read(100, 2);

    printf("Cache 2: Read again after a miss\n");
    // Read again after a miss
    cacheB.read(100, 2);
    //cacheB.write(100, 13, 2);

    // Simulate read operation in Cache C
    cacheC.read(100, 3);

    printf("Cache 3: Read again after a miss\n");
    cacheC.read(100, 3);

    return 0;
}
