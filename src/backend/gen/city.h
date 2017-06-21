#ifndef CITY_INCLUDED
#define CITY_INCLUDED

#include <string>

namespace Backend
{
    class City
    {
    public:
        enum class Trade
        {
            Farm,
            Hunt,
            Lumber,
            Mining,
            Smithy
        };

    private:
        std::string name;
        Trade trade;

    public:
        City(Trade trade);
    };
}

#endif // CITY_INCLUDED