#include "world_generator.h"

using namespace Backend;

World *WorldGenerator::GenerateWorld()
{
    World *out = new World();
    worlds.push_back(out);
    return out;
}