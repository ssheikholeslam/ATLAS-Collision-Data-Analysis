#include <GridBucketing.h>
#include <algorithm>

GridBucketing::GridBucketing(float minRestEnergy, float maxRestEnergy, size_t size) :
    gridSize(size), minKinetic(13000), maxKinetic(13000), minRest(minRestEnergy), maxRest(maxRestEnergy),
    grid(size, std::vector<Cell>(size, {std::vector<CollisionEvent>(),
                                        std::priority_queue<CollisionEvent, std::vector<CollisionEvent>,
                                            std::function<bool(CollisionEvent, CollisionEvent)>>(
                                                [](const CollisionEvent& a, const CollisionEvent& b) {
                                                return a.efficiency < b.efficiency;
                                                })})) {}

std::pair<unsigned int, unsigned int> GridBucketing::getCellIndices(const CollisionEvent& event) {
    std::pair<unsigned int, unsigned int> indices = {0, 0};
    float bucketSize = (maxRest - minRest + 1e-6) / gridSize;
    int y = (event.restEnergyOut - minRest) / bucketSize;
    indices.second = std::clamp(y, 0, gridSize - 1);
    return indices;
}

void GridBucketing::insert(const CollisionEvent& event) {
    auto [i, j] = getCellIndices(event);
    Cell& cell = grid[i][j];
    cell.events.push_back(event);
    cell.maxHeap.push(event);
}
