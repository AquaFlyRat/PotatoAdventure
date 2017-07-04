#ifndef RACE_INCLUDED
#define RACE_INCLUDED

#include "generated.h"
#include <string>

namespace Backend
{
	class Race : public Generated {
	private:
		std::string name;

		std::string GenerateName();
	public:
		virtual void Generate();
	};
}

#endif