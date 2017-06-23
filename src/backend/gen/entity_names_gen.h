#ifndef ENTITY_NAMES_GEN_INCLUDED
#define ENTITY_NAMES_GEN_INCLUDED

#include <string>
#include <RapidXML/rapidxml.hpp>
#include "../backend_utils.h"
#include "text.h"
#include "gen_config.h"

namespace Backend
{
    class EntityNamesGenerator
    {
    public:
        std::string GenerateName(Gender gender)
        {
            Random *random = Random::Instance();


            rapidxml::xml_node<> *names_info = ConfigFile::Instance()->GetXMLDocument()->first_node("GenData")->first_node("TextInfo")->first_node("NamesInfo");
            // Commented out to fix crash;
/*            NameSection name_section(names_info);

            std::string random_vowel_1 = random->Choose(name_section.sections[0].text);
            std::string random_vowel_2 = random->Choose(name_section.sections[0].text);
            std::string random_vowel_3 = random->Choose(name_section.sections[0].text);

            std::string random_cons_1 = random->Choose(name_section.sections[1].text);
            std::string random_cons_2 = random->Choose(name_section.sections[1].text);
            std::string random_cons_3 = random->Choose(name_section.sections[1].text);

            */
            switch (gender)
            {
            case Gender::Male:
                break;
            }
            return std::string();
        }
    };
}

#endif