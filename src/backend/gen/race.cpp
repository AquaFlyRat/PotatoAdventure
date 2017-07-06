#include "race.h"
#include "entity_names_gen.h"
#include <stdlib.h>
using namespace Backend;

void Race::Generate()
{
	this->name = GenerateName();

	Random *rand = Random::Instance();
	this->general_height = static_cast<Height>(rand->RandomInt32(0, 3));
	this->ability = static_cast<Abilitys>(rand->RandomInt32(0, 4));

	int chance = rand->RandomInt32(0, 100);
	std::string ears;
	if (chance <= 25) {
		ears = "pointy";
	}
	if (!ears.empty())
	{
		this->physical_desc += "A " + ears + " folk ";
	}
	else {
		this->physical_desc + "They ";
	}

	int hair = rand->Choose<int>({ 1, 2, 3, 4, 5 });
	this->physical_desc += "typically found with ";
	if (hair == 1) {
		this->physical_desc += "black ";
	}
	else if(hair == 2) {
		this->physical_desc += "sandy ";
	}
	else if (hair == 3) {
		this->physical_desc += "brown ";
	}
	else if (hair == 4) {
		this->physical_desc + "no ";
	}
	else if (hair == 5) {
		this->physical_desc += "red ";
	}
	else {
		this->physical_desc += "<Unknown>(" + std::to_string(hair) + ")";
	}
	this->physical_desc += "hair.";
}

std::string Race::GenerateName()
{
	std::vector<NameFormat> format = { NameFormat::ConsonantVowelConsonant, NameFormat::VowelConsonantVowel };
	EntityNamesGenerator gen;
	Random *random = Random::Instance();
	return gen.GenericAlphaName(random->Choose(format), { "ite", "ton", "man", "iam" });
}

std::string Race::Describe()
{
	std::string out = "Race: The " + this->name + "\n";
	out += GenerateAbilityDescription();
	out += GenerateHeightDescription();
	out += "\nPhysical: " + this->physical_desc;
	return out;
}

std::string Race::GenerateAbilityDescription()
{
	std::string ability;

	switch (this->ability)
	{
	case Abilitys::MagicResistance:
		ability = "Magic Resistance";
		break;
	case Abilitys::Sneak:
		ability = "Sneak";
		break;
	case Abilitys::Archery:
		ability = "Archery";
		break;
	case Abilitys::CloseQuartersCombat:
		ability = "Close Quarters Combat";
		break;
	case Abilitys::Pursuasion:
		ability = "Pursuasion";
		break;
	default:
		break;
	}

	return "Ability: " + ability + "\n";
}

std::string Race::GenerateHeightDescription()
{
	std::string height;
	switch (this->general_height)
	{
	case Race::Height::Dwarf:
		height = "Dwarf";
		break;
	case Race::Height::Halfling:
		height = "Halfling";
		break;
	case Race::Height::Normal:
		height = "Normal sized";
		break;
	case Race::Height::Huge:
		height = "'Above average' (aka HUGE)";
		break;
	default:
		break;
	}

	return "Average size of the populous: " + height;
}
