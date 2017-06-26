#ifndef GEN_CONFIG_INCLUDED
#define GEN_CONFIG_INCLUDED

#include <RapidXML/rapidxml.hpp>
#include <vector>
#include <map>
#include "text.h"

namespace Backend
{
    class ConfigFile
    {
    private:
        rapidxml::xml_document<> *xmldoc;
        std::vector<char> src;
        std::map<std::string, NameSection> names;
        
    public:
        ConfigFile();
        ~ConfigFile();

        rapidxml::xml_document<> *GetXMLDocument();
        rapidxml::xml_node<> *GetRootNode();

        static ConfigFile *Instance()
        {
            static ConfigFile file;
            return &file;
        }

        const std::map<std::string, NameSection>& Names() const { return names; }
    };
}

#endif