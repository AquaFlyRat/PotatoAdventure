#ifndef WORLD_GENERATOR_INCLUDED
#define WORLD_GENERATOR_INCLUDED

#include <vector>

#include "world.h"

namespace Backend
{
    class WorldGenerator
    {
    private:
        std::vector<World*> worlds;
    public:
        World *GenerateWorld();
    };
}

#endif