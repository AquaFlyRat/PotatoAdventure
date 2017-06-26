#include "city.h"

#include "gen_config.h"
#include <vector>
#include "../backend_utils.h"
#include <map>

using namespace Backend;
template <auto X> void func() {}
City::City(Trade trade): trade(trade)
{
    this->trade = trade;
    this->name = GenerateName();

    Random *random = Random::Instance();

    this->king_rating = static_cast<GoodnessRating>(random->RandomInt32(0, 4));
    this->wealth_rating = static_cast<GoodnessRating>(random->RandomInt32(0, 4));
}

std::string City::GenerateName()
{
    const std::map<std::string, NameSection> name_sections = ConfigFile::Instance()->Names();
    std::string name;

    std::vector<std::string> spaceOrNot = { " ", "" };

    Random *random = Random::Instance();

    auto find_trade_text = [=](std::string name, NameSection& section) {
        std::map<std::string, NameSection>::const_iterator it = name_sections.find(name);
        if (it != name_sections.end()) {
            section = it->second;
            return true;
        }
        return false;
    };

    std::map<Trade, std::string> trade_str_map;
    trade_str_map.insert(std::make_pair(Trade::Farm, "farm"));
    trade_str_map.insert(std::make_pair(Trade::Hunt, "hunt"));
    trade_str_map.insert(std::make_pair(Trade::Lumber, "mill"));
    trade_str_map.insert(std::make_pair(Trade::Mining, "mine"));
    trade_str_map.insert(std::make_pair(Trade::Smithy, "smithy"));

    std::string ia = random->Choose(spaceOrNot);
    std::string ib = " ";

    NameSection text_section;
    bool found = find_trade_text(trade_str_map[this->trade], text_section);

    if (!found) {
        printf("Cannot find text for generating city names!\n");
        assert(false);
    }

    std::string tmpa = random->Choose(text_section.sections[0].text);
    std::string tmpb = random->Choose(text_section.sections[1].text);
    std::string tmpc = random->Choose(text_section.sections[2].text);
    name = tmpa + ia + tmpb + ib + tmpc;
    return name;
}