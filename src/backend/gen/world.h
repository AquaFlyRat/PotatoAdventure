#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "city_generator.h"
#include <vector>

namespace Backend {
    class World {
    private:
        CityGenerator citys;

    public:
        World();

        const std::vector<City>& GetCitys() const { return citys.Citys(); }
    };
}

#endif