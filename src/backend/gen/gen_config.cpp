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
    std::vector<char> cstr(gen_file.c_str(), gen_file.c_str() + gen_file.size() + 1);
    xmldoc = new rapidxml::xml_document<>();
    xmldoc->parse<0>(&cstr[0]);
}

ConfigFile::~ConfigFile()
{
    delete xmldoc;
}

rapidxml::xml_document<> *ConfigFile::GetXMLDocument()
{
    return xmldoc;
}