#include "city_generator.h"
#include "backend_utils.h"

#include "gen_config.h"

using namespace Backend;

void CityGenerator::GenerateAll()
{
    ConfigFile *config = ConfigFile::Instance();
    auto doc = config->GetXMLDocument();
    
}