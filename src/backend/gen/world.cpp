#include "world.h"
#include "race.h"
#include <iostream>
#include "../gui_api.h"


using namespace Backend;

World::World()
{
    citys.GenerateAll();
	for (int i = 0; i < 3; i++) {
		Race race;
		race.Generate();
		GUI::WriteLine(race.Describe());
		GUI::WriteLine("");
	}
}