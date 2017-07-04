#include "world.h"
#include "race.h"
using namespace Backend;

World::World()
{
    citys.GenerateAll();
	Race race;
	race.Generate();
}