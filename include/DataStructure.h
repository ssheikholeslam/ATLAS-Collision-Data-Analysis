// include/DataStructure.h
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include "CollisionEvent.h"
#include <vector>

/**
 * @class DataStructure
 * @brief Abstract base class (ABC) for data structures in the project (KD-Tree and Grid-Based Bucketing).
 *
 * Defines an interface for insertion, range queries, and extremum queries,
 * enabling polymorphic use of k-d tree and grid-based bucketing, allowing
 * for further performance insights.
 */
class DataStructure {
public:
    virtual void insert(const CollisionEvent& event) = 0;
    virtual std::vector<CollisionEvent> range_query(float minRestEnergy, float maxRestEnergy) = 0;
    virtual CollisionEvent find_max_efficiency() = 0;
    virtual ~DataStructure() = default;
};

#endif // DATA_STRUCTURE_H
