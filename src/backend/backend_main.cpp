#include "../frontend/master.h"
#include "gen/world_generator.h"
#include "gen/city_generator.h"
#include "backend_utils.h"
#include <stdlib.h>

namespace Backend
{
    void Tick()
    {
    }

    void Start()
    {
        WorldGenerator worldGen;
        World* world = worldGen.GenerateWorld();
        printf("%i\n", world->GetCitys().size());
    }
}
