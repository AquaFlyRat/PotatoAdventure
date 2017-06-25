#include "city_generator.h"
#include "../backend_utils.h"
#include "gen_config.h"
#include "text.h"
#include "city.h"
#include <stdio.h>
#include "entity_names_gen.h"
using namespace Backend;

void CityGenerator::GenerateAll()
{
    ConfigFile *config = ConfigFile::Instance();
    auto doc = config->GetXMLDocument();
    auto root = doc->first_node("GenData");
    rapidxml::xml_node<> *node = root->first_node("CityData")->first_node("CityNumberRange");

    int min = atoi(node->first_attribute("min")->value());
    int max = atoi(node->first_attribute("max")->value());

    city_count = Random::Instance()->RandomInt32(min, max);
    for (int i = 0; i < city_count; ++i)
    {
        City::Trade trade = static_cast<City::Trade>(Random::Instance()->RandomInt32(0, 4));
        this->citys.push_back(City(trade));
    }

    EntityNamesGenerator gen;
    std::string n1 = gen.GenerateName(Gender::Male);
    std::string n2 = gen.GenerateName(Gender::Female);
    std::string n3 = gen.GenerateName(Gender::Male);
    std::string n4 = gen.GenerateName(Gender::Female);
}
