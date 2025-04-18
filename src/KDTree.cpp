#include "KDTree.h"
#include <algorithm>
#include <stdexcept>

KDTree::KDTree() : root(nullptr) {}

void KDTree::buildBalanced(std::vector<CollisionEvent>& events) {
    // Sort events by restEnergyOut since kineticEnergyIn is constant
    std::sort(events.begin(), events.end(), [](const CollisionEvent& a, const CollisionEvent& b) {
        return a.restEnergyOut < b.restEnergyOut;
    });
    root = buildRecursive(events, 0);
}

std::unique_ptr<Node> KDTree::buildRecursive(std::vector<CollisionEvent>& events, int depth) {
    if (events.empty()) return nullptr;
    if (events.size() <= bucketSize) {
        auto node = std::make_unique<Node>();
        node->events = std::move(events);
        return node;
    }

    int dim = 1; // Always split on restEnergyOut since kineticEnergyIn is constant
    size_t median = events.size() / 2;
    std::nth_element(events.begin(), events.begin() + median, events.end(),
                     [](const CollisionEvent& a, const CollisionEvent& b) {
                         return a.restEnergyOut < b.restEnergyOut;
                     });

    auto node = std::make_unique<Node>();
    node->dimension = dim;
    node->splitValue = events[median].restEnergyOut;

    std::vector<CollisionEvent> leftEvents(events.begin(), events.begin() + median);
    std::vector<CollisionEvent> rightEvents(events.begin() + median + 1, events.end());

    node->left = buildRecursive(leftEvents, depth + 1);
    node->right = buildRecursive(rightEvents, depth + 1);

    return node;
}

void KDTree::insert(const CollisionEvent& event) {
    // Kept for compatibility, but not used with buildBalanced
    if (!root) {
        root = std::make_unique<Node>();
        root->events.push_back(event);
    }
}

std::vector<CollisionEvent> KDTree::range_query(float minRestEnergy, float maxRestEnergy) {
    std::vector<CollisionEvent> result;
    rangeQueryRecursive(root.get(), minRestEnergy, maxRestEnergy, result);
    return result;
}

void KDTree::rangeQueryRecursive(const Node* node, float minRestEnergy, float maxRestEnergy,
                                 std::vector<CollisionEvent>& result) {
    if (!node) return;
    if (node->left || node->right) {
        if (node->splitValue >= minRestEnergy)
            rangeQueryRecursive(node->left.get(), minRestEnergy, maxRestEnergy, result);
        if (node->splitValue <= maxRestEnergy)
            rangeQueryRecursive(node->right.get(), minRestEnergy, maxRestEnergy, result);
    } else {
        for (const auto& e : node->events) {
            if (e.restEnergyOut >= minRestEnergy && e.restEnergyOut <= maxRestEnergy)
                result.push_back(e);
        }
    }
}

CollisionEvent KDTree::find_max_efficiency() {
    return findMaxEfficiencyRecursive(root.get());
}

CollisionEvent KDTree::findMaxEfficiencyRecursive(const Node* node) {
    if (!node) throw std::runtime_error("Empty tree");
    if (!node->left && !node->right) {
        return *std::max_element(node->events.begin(), node->events.end(),
                                 [](const auto& a, const auto& b) { return a.efficiency < b.efficiency; });
    }
    CollisionEvent leftMax = findMaxEfficiencyRecursive(node->left.get());
    CollisionEvent rightMax = findMaxEfficiencyRecursive(node->right.get());
    return (leftMax.efficiency > rightMax.efficiency) ? leftMax : rightMax;
}