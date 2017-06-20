#ifndef GEN_CONFIG_INCLUDED
#define GEN_CONFIG_INCLUDED

#include <RapidXML/rapidxml.hpp>

namespace Backend
{
    class ConfigFile
    {
    private:
        rapidxml::xml_document<> *xmldoc;

    public:
        ConfigFile();
        ~ConfigFile();

        rapidxml::xml_document<> *GetXMLDocument();

        static ConfigFile *Instance()
        {
            static ConfigFile file;
            return &file;
        }
    };
}

#endif