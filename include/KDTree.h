#ifndef KD_TREE_H
#define KD_TREE_H

#include "DataStructure.h"
#include <memory>
#include <vector>
#include <algorithm>

/**
 * @class KDTree
 * @brief k-d tree for efficient multi-dimensional range queries.
 *
 * Partitions data along kinetic and rest energy dimensions. Useful for
 * querying events in specific energy ranges, e.g., to study resonance production.
 *
 * Background: Efficient range queries help identify events with specific rest-mass
 * outputs, potentially linked to heavy particles like top quarks.
 */
struct Node {
    int dimension;  ///< 0: kineticEnergyIn, 1: restEnergyOut.
    float splitValue;  ///< Splitting threshold.
    std::vector<CollisionEvent> events;  ///< Leaf bucket.
    std::unique_ptr<Node> left, right;  ///< Child nodes.
};

class KDTree : public DataStructure {
public:
    KDTree();
    void buildBalanced(std::vector<CollisionEvent>& events);
    void insert(const CollisionEvent& event) override;
    std::vector<CollisionEvent> range_query(float minRestEnergy, float maxRestEnergy) override;
    CollisionEvent find_max_efficiency() override;
private:
    std::unique_ptr<Node> root;
    const size_t bucketSize = 10;  ///< Max events per leaf.

    std::unique_ptr<Node> buildRecursive(std::vector<CollisionEvent>& events, int depth);
    void rangeQueryRecursive(const Node* node, float minRestEnergy, float maxRestEnergy,
                             std::vector<CollisionEvent>& result);
    CollisionEvent findMaxEfficiencyRecursive(const Node* node);
};

#endif // KD_TREE_H