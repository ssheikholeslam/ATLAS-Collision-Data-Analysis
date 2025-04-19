#include <GridBucketing.h>
#include <algorithm>

GridBucketing::GridBucketing(float minRestEnergy, float maxRestEnergy, size_t size) :
    gridSize(size), minKinetic(13000), maxKinetic(13000), minRest(minRestEnergy), maxRest(maxRestEnergy),
    grid(size, std::vector<Cell>(size, {std::vector<CollisionEvent>(),
                                        std::priority_queue<CollisionEvent, std::vector<CollisionEvent>,
                                            std::function<bool(CollisionEvent, CollisionEvent)>>(
                                                [](const CollisionEvent& a, const CollisionEvent& b) {
                                                return a.efficiency < b.efficiency;
                                                })})) {bucketRange = (maxRest - minRest + 1e-6) / gridSize;}

std::pair<unsigned int, unsigned int> GridBucketing::getCellIndices(float restEnergy) {
    std::pair<unsigned int, unsigned int> indices = {0, 0};
    int y = (restEnergy - minRest) / bucketRange;
    indices.second = std::clamp(y, 0, gridSize - 1);
    return indices;
}

void GridBucketing::insert(const CollisionEvent& event) {
    auto [i, j] = getCellIndices(event.restEnergyOut);
    Cell& cell = grid[i][j];
    cell.events.push_back(event);
    cell.maxHeap.push(event);
}

std::vector<CollisionEvent> GridBucketing::range_query(float minRestEnergy, float maxRestEnergy) {
    std::vector<CollisionEvent> result;
    auto minIndices = getCellIndices(minRestEnergy);
    auto maxIndices = getCellIndices(maxRestEnergy);
    for (unsigned int i = minIndices.first; i <= maxIndices.first; ++i) {
        Cell& cell = grid[minIndices.first][i];
        for (const CollisionEvent& event : cell.events) {
            if (minRestEnergy <= event.restEnergyOut && event.restEnergyOut <= maxRestEnergy)
                result.push_back(event);
        }
    }
    return result;
}
