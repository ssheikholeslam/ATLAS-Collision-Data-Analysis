// include/DataLoader.h
#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "CollisionEvent.h"
#include <vector>
#include <string>

std::vector<CollisionEvent> loadData(const std::string& filename);

#endif // DATA_LOADER_H
