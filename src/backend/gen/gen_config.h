#ifndef GEN_CONFIG_INCLUDED
#define GEN_CONFIG_INCLUDED

#include <RapidXML/rapidxml.hpp>
#include <vector>
#include "text.h"

namespace Backend
{
    class ConfigFile
    {
    private:
        rapidxml::xml_document<> *xmldoc;
        std::vector<char> src;
        std::vector<NameSection> names;

    public:
        ConfigFile();
        ~ConfigFile();

        rapidxml::xml_document<> *GetXMLDocument();

        static ConfigFile *Instance()
        {
            static ConfigFile file;
            return &file;
        }

        const std::vector<NameSection>& Names() const { return names; }
    };
}

#endif