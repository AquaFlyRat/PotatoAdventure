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
            
            NameSection name_section(names_info);

            std::string random_vowel_1 = random->Choose(name_section.sections[0].text);
            std::string random_vowel_2 = random->Choose(name_section.sections[0].text);
            std::string random_vowel_3 = random->Choose(name_section.sections[0].text);

            std::string random_cons_1 = random->Choose(name_section.sections[1].text);
            std::string random_cons_2 = random->Choose(name_section.sections[1].text);
            std::string random_cons_3 = random->Choose(name_section.sections[1].text);

            std::string name;
            switch (gender)
            {
            case Gender::Male:
                if (random->RandomBoolean()) {
                    name = random_vowel_1 + random_cons_1 + random_vowel_2 + random_cons_2;
                }
                else {
                    name = random_cons_1 + random_vowel_1 + random_cons_2 + random_vowel_2 + random_cons_3;
                }
                break;
            case Gender::Female:
                if (random->RandomBoolean()) {
                    name = random_cons_1 + random_vowel_1 + random_cons_2 + random_vowel_2;
                }
                else {
                    name = random_vowel_1 + random_cons_1 + random_vowel_2 + random_cons_2 + random_vowel_3;
                }
                break;
            }

            return name;
        }
    };
}

#endif