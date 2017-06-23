#include "gen_config.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

using namespace Backend;

ConfigFile::ConfigFile()
{

    std::ifstream t("assets/gen.xml");
    std::string gen_file((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());
    src = std::vector<char>(gen_file.c_str(), gen_file.c_str() + gen_file.size() + 1);
    xmldoc = new rapidxml::xml_document<>();
    xmldoc->parse<0>(&src[0]);

    rapidxml::xml_node<> *name_section = xmldoc->first_node()->first_node("TextInfo")->first_node("NameSection");
    while (name_section)
    {
        NameSection section(name_section);
        const char *name = section.place_type_name;
        names.insert(std::make_pair(std::string(name), section));
        name_section = name_section->next_sibling("NameSection");
    }
}

ConfigFile::~ConfigFile()
{
    delete xmldoc;
}

rapidxml::xml_document<> *ConfigFile::GetXMLDocument()
{
    return xmldoc;
}