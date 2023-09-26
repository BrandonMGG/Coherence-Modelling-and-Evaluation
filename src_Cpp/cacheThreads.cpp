#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

// Forward declarations
class Cache;

// Memory class to represent system memory
class Memory {
private:
    std::map<int, int> data; // Memory address to data mapping
    std::mutex memoryMutex;  // Mutex to protect access to memory

public:
    int read(int address) {
        std::lock_guard<std::mutex> lock(memoryMutex);
        // Simulate reading data from memory
        return data[address];
    }

    void write(int address, int value) {
        std::lock_guard<std::mutex> lock(memoryMutex);
        // Simulate writing data to memory
        data[address] = value;
    }
};

// CacheLine class representing cache lines
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
    std::mutex cacheMutex;

public:
    Cache(int numLines, Memory& mem) : memory(mem) {
        cacheLines.resize(numLines);
    }

    // Handle read operation
    int read(int address, int processorId) {
        int index = address % cacheLines.size();
        CacheLine& line = cacheLines[index];

        std::lock_guard<std::mutex> lock(cacheMutex);

        if (line.tag == address) {
            // Cache hit
            std::cout << "Cache " << processorId << " - Read HIT (Address " << address << "): \n" << line.data << std::endl;
        } else {
            // Cache miss
            std::cout << "Cache " << processorId << " - Read MISS (Address " << address << "): Fetching data from memory.\n" << std::endl;
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

        std::lock_guard<std::mutex> lock(cacheMutex);

        if (line.tag == address) {
            // Cache hit
            std::cout << "Cache " << processorId << " - Write HIT (Address " << address << "): Updating data.\n" << std::endl;
            line.data = value;
            line.state = CacheLine::MODIFIED;

            // Update other caches
            notifyOtherCaches(address, processorId);
        } else {
            // Cache miss
            std::cout << "Cache " << processorId << " - Write MISS (Address " << address << "): Writing data to memory.\n" << std::endl;
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
        std::cout << "Cache " << processorId << " notifying other caches of change (Address " << address << ").\n" << std::endl;
    }
};

int main() {
    Memory memory;
    Cache cacheA(4, memory);
    Cache cacheB(4, memory);
    Cache cacheC(4, memory);

    // Create threads for Cache A, Cache B, and Cache C
    std::thread threadA([&cacheA]() {
        cacheA.write(100, 42, 1);
        cacheA.read(100, 1);
    });

    std::thread threadB([&cacheB]() {
        cacheB.read(100, 2);
    });

    std::thread threadC([&cacheC]() {
        cacheC.read(100, 3);
    });

    // Join threads to wait for completion
    threadA.join();
    threadB.join();
    threadC.join();

    return 0;
}
