#include "race.h"
#include "entity_names_gen.h"

using namespace Backend;

void Race::Generate()
{
	this->name = GenerateName();
}

std::string Race::GenerateName()
{
	std::vector<NameFormat> format = { NameFormat::ConsonantVowelConsonant, NameFormat::VowelConsonantVowel };
	EntityNamesGenerator gen;
	Random *random = Random::Instance();
	return gen.GenericAlphaName(random->Choose(format), { "ite", "ton", "man", "iam" });
}