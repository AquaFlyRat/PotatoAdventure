#include "city.h"

#include "gen_config.h"
#include <vector>
#include "../backend_utils.h"

using namespace Backend;

City::City(Trade trade): trade(trade)
{
    const std::vector<NameSection> nameSections = ConfigFile::Instance()->Names();
    std::string name;

    std::vector<std::string> spaceOrNot = { " ", "" };

    Random *random = Random::Instance();

    switch (trade)
    {
    case Trade::Farm:
        std::string ia = random->Choose(spaceOrNot);
        std::string ib = " ";
        
        break;
    }
}