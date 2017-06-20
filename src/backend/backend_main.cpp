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
        CityGenerator citys;
        citys.GenerateAll();
    }
}
