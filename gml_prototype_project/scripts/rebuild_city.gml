///rebuild_city(num)
var num = argument[0];
city_faction[num] = irandom(factionnum);
city_god[num] = irandom_range(1,godnum);
city_stat_population[num] = irandom_range(2,5);    //by popscale
city_stat_wealth[num] = random_range(1,5);    //0-5 bad-good
