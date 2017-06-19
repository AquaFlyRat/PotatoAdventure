///gen_city(num)
var num = argument[0];
city[num] = 1;
city_race[num] = irandom_range(1,racenum);
city_god[num] = irandom_range(1,godnum);
city_trade[num] = city_get_trade(num);
city_stat_population[num] = irandom_range(3,5);    //by popscale
city_stat_poptomil[num] = random_range(.25,.50);
city_stat_military[num] = city_stat_population[num]*(city_stat_poptomil[num]);
city_stat_king[num] = random_range(0,5);    //0-5 bad-good
city_stat_wealth[num] = random_range(1,5);    //0-5 bad-good
if city_name[num]="." {city_name[num] = city_get_name(num)}
show_debug_message("CGEN COMPLETE")
desc_add(string(city_name[num])+"["+string(city_faction[num])+"]"+" was founded at ["+string(city_x[num])+","+string(city_y[num])+"]");
