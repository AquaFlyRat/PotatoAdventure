#ifndef CITY_GENERATOR_INCLUDED
#define CITY_GENERATOR_INCLUDED

#include <vector>

#include "city.h"

namespace Backend
{
    class CityGenerator
    {
    private:
        std::vector<City> citys;
        int               city_count;
    public:
        void GenerateAll();
        const std::vector<City>& Citys() const { return citys; }
    };
}

#endif // CITY_GENERATOR_INCLUDED