#include <GridBucketing.h>

GridBucketing::GridBucketing(size_t size) : gridSize(size), minKinetic(13000), maxKinetic(13000), minRest(0), maxRest(0),
                                            grid(size, std::vector<Cell>(size, {std::vector<CollisionEvent>(),
                                                                                std::priority_queue<CollisionEvent, std::vector<CollisionEvent>,
                                                                                    std::function<bool(CollisionEvent, CollisionEvent)>>(
                                                                                        [](const CollisionEvent& a, const CollisionEvent& b) {
                                                                                        return a.efficiency < b.efficiency;
                                                                                        })})) {}