// src/DataLoader.cpp
#include "CollisionEvent.h"
#include <fstream>
#include <vector>
#include <stdexcept>

/**
 * @brief Loads collision events from the generated binary file.
 * @param filename Path to the binary file.
 * @return Vector of CollisionEvent objects.
 *
 * Reads data parsed from ROOT files into memory-efficient structures
 */
std::vector<CollisionEvent> loadData(const std::string& filename) {
    std::vector<CollisionEvent> events;
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("File not found");

    while (file) {
        CollisionEvent e;
        char inParts[32], outParts[256];
        file.read(reinterpret_cast<char*>(&e.eventId), sizeof(int));
        file.read(inParts, 32);
        file.read(outParts, 256);
        file.read(reinterpret_cast<char*>(&e.kineticEnergyIn), sizeof(float));
        file.read(reinterpret_cast<char*>(&e.restEnergyOut), sizeof(float));
        file.read(reinterpret_cast<char*>(&e.efficiency), sizeof(float));
        if (file) {
            e.incomingParticles = std::string(inParts, 32);
            e.outgoingParticles = std::string(outParts, 256);
            events.push_back(e);
        }
    }
    return events;
}