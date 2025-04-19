#ifndef GRID_BUCKETING_H
#define GRID_BUCKETING_H

#include "DataStructure.h"
#include <vector>
#include <queue>
#include <functional>

/**
 * @class GridBucketing
 * @brief Grid-based bucketing with heaps for fast queries.
 *
 * Divides data into a grid based on energy values, using heaps for O(1) extremum
 * queries. Ideal for identifying high-efficiency events (e.g., heavy particle production).
 *
 * Background: High efficiency may indicate quark-gluon plasma or Higgs boson events.
 */
struct Cell {
    std::vector<CollisionEvent> events;  ///< bucket
    std::priority_queue<CollisionEvent, std::vector<CollisionEvent>,                   ///< heap based on efficiency
                        std::function<bool(CollisionEvent, CollisionEvent)>> maxHeap;
};

class GridBucketing : public DataStructure {
public:
    GridBucketing(float minRestEnergy, float maxRestEnergy, size_t size = 100);
    void insert(const CollisionEvent& event) override;
    std::vector<CollisionEvent> range_query(float minRestEnergy, float maxRestEnergy) override;
    CollisionEvent find_max_efficiency() override;
private:
    std::vector<std::vector<Cell>> grid;
    size_t gridSize;
    float bucketRange;
    float minKinetic;
    float maxKinetic;
    float minRest;
    float maxRest;

    std::pair<unsigned int, unsigned int> getCellIndices(float restEnergy);
};

#endif // GRID_BUCKETING_H