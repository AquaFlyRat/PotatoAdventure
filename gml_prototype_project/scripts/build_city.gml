///build_city(num,x,y)
var num = argument[0];
var xx = argument[1];
var yy = argument[2];
civmap[xx,yy]=1;
city_x[num] = xx;
city_y[num] = yy;
city[num] = 1;
city_race[num] = city_race[city_faction[num]];
city_god[num] = city_god[city_faction[num]];
city_trade[num] = city_get_trade(num);
city_stat_population[num] = 1;
city_stat_poptomil[num] = city_stat_poptomil[city_faction[num]];
city_stat_military[num] = city_stat_population[num]*city_stat_poptomil[num];
city_stat_king[num] = random_range(0,5);    //0-5 bad-good
city_stat_wealth[num] = city_stat_wealth[city_faction[num]];
city_name[num] = city_get_name(num)
show_debug_message("CGEN COMPLETE")
citynum+=1;
