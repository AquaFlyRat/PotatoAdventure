#ifndef RACE_INCLUDED
#define RACE_INCLUDED

#include "generated.h"
#include <string>

namespace Backend
{
	class Race : public Generated {
	public:
		enum class Height
		{
			Dwarf, Halfling, Normal, Huge
		};

		enum class Abilitys
		{
			MagicResistance,
			Sneak,
			Archery,
			CloseQuartersCombat,
			Pursuasion
		};

	private:
		std::string name;
		Height general_height;
		Abilitys ability;

		std::string GenerateName();
		std::string GenerateAbilityDescription();
		std::string GenerateHeightDescription();

	public:
		virtual void Generate();
		virtual std::string Describe();

		inline const std::string& Name() const { return name; }
		inline Height AverageHeight() const { return general_height; }
	};
}

#endif