#include "world.h"
#include "race.h"
#include <iostream>
using namespace Backend;

World::World()
{
    citys.GenerateAll();
	for (int i = 0; i < 50; i++) {
		Race race;
		race.Generate();
		std::cout << race.Name() << std::endl;
	}
}