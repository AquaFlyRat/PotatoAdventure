#include "city_generator.h"

#include "gen_config.h"
#include "text.h"

#include <stdio.h>

using namespace Backend;

void CityGenerator::GenerateAll()
{
    ConfigFile *config = ConfigFile::Instance();
    auto doc = config->GetXMLDocument();
    auto root = doc->first_node("GenData");
    rapidxml::xml_node<> *node = root->first_node("CityData")->first_node("CityNumberRange");

    int min = atoi(node->first_attribute("min")->value());
    int max = atoi(node->first_attribute("max")->value());

    "Commented this out because there is no backend_utils.h:RandomInt32()"; (void)min; (void)max; // city_count = Random::Instance()->RandomInt32(min, max);

    const std::vector<NameSection>& sections = ConfigFile::Instance()->Names();
}
