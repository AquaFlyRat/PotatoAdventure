#include "world.h"
#include "race.h"
#include <iostream>
#include "../gui_api.h"
using namespace Backend;

World::World()
{
    citys.GenerateAll();
	for (int i = 0; i < 10; i++) {
		Race race;
		race.Generate();
		GUI::WriteLine("Race: ", race.Name());
		//std::cout << race.Name() << std::endl;
	}
}