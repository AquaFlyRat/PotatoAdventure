#ifndef ENTITY_NAMES_GEN_INCLUDED
#define ENTITY_NAMES_GEN_INCLUDED

#include <string>
#include <RapidXML/rapidxml.hpp>
#include "../backend_utils.h"
#include "text.h"
#include "gen_config.h"
#include <vector>

namespace Backend
{
	enum class NameFormat
	{
		VowelConsonantVowel,
		ConsonantVowelConsonant
	};

    class EntityNamesGenerator
    {
    public:
		std::string GenericAlphaName(NameFormat format, std::vector<std::string> suffixes = std::vector<std::string>())
		{
			Random *random = Random::Instance();


			rapidxml::xml_node<> *names_info = ConfigFile::Instance()->GetXMLDocument()->first_node("GenData")->first_node("TextInfo")->first_node("NamesInfo");

			NameSection name_section(names_info);

			std::string random_vowel_1 = random->Choose(name_section.sections[0].text);
			std::string random_vowel_2 = random->Choose(name_section.sections[0].text);
			
			std::string random_cons_1 = random->Choose(name_section.sections[1].text);
			std::string random_cons_2 = random->Choose(name_section.sections[1].text);
			
			std::string gen_name;
			switch (format)
			{
			case NameFormat::ConsonantVowelConsonant:
				gen_name += random_cons_1 + random_vowel_1 + random_cons_2;
				break;
			case NameFormat::VowelConsonantVowel:
				gen_name += random_vowel_1 + random_cons_1 + random_vowel_2;
				break;
			default:
				gen_name = "<UndefinedNameGen>";
				break;
			}

			if(!suffixes.empty())
				gen_name += random->Choose(suffixes);

			return gen_name;
		}

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