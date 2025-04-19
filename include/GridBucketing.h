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
    std::vector<CollisionEvent> events;
    std::priority_queue<CollisionEvent, std::vector<CollisionEvent>,
                        std::function<bool(CollisionEvent, CollisionEvent)>> maxHeap;
};

#endif // GRID_BUCKETING_H