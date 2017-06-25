#ifndef TEXT_NAMES_INCLUDED
#define TEXT_NAMES_INCLUDED

#include <RapidXML/rapidxml.hpp>
#include <cstdlib>
#include <string>
#include <vector>

namespace Backend
{
    struct TextSection
    {
        std::vector<std::string> text;
        int order;
        const char *name;
    };

    struct NameSection
    {
        const char *place_type_name;
        std::vector<TextSection> sections;

        // DO NOTHING! HUAHAHAH
        NameSection() {}

        NameSection(rapidxml::xml_node<> *name_section_node)
        {
            place_type_name = static_cast<const char*>(name_section_node->first_attribute("name")->value());

            rapidxml::xml_node<> *text_section = name_section_node->first_node("TextSection");
            while (text_section != NULL)
            {
                rapidxml::xml_attribute<> *order_attrib = text_section->first_attribute("order");
                int order = -1;
                if (order_attrib != nullptr)
                {
                    order = atoi(order_attrib->value());
                }
                TextSection section;
                section.order = order;
                rapidxml::xml_attribute<> *name_attrib = text_section->first_attribute("name");
                if (name_attrib != nullptr)
                {
                    section.name = name_attrib->value();
                }

                rapidxml::xml_node<> *text = text_section->first_node("Text");
                while (text != NULL)
                {
                    std::string txt = std::string(text->value());
                    section.text.push_back(txt);
                    text = text->next_sibling("Text");
                }
                sections.push_back(section);
                text_section = text_section->next_sibling("TextSection");
            }
        }
    };
}

#endif // TEXT_NAMES_INCLUDED
