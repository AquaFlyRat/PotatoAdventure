#ifndef CITY_INCLUDED
#define CITY_INCLUDED

#include <string>
#include "../backend_utils.h"

namespace Backend
{
    class City
    {
    public:
        enum class Trade
        {
            Farm = 0,
            Hunt = 1,
            Lumber = 2,
            Mining = 3,
            Smithy = 4
        };

    private:
        std::string name;
        Trade trade;
        
        GoodnessRating king_rating;
        GoodnessRating wealth_rating;

        std::string GenerateName();

    public:
        City(Trade trade);
    };
}

#endif // CITY_INCLUDED