///init cities
citynum = 5;
factionnum = citynum;
popscale = 10;
for (i=0;i<=1000;i++){
    city[i] = -1;
    city_x[i] = -1;
    city_y[i] = -1;
    city_name[i] = ".";
    city_faction[i] = -1;
    city_race[i] = -1;
    city_god[i] = -1;
    city_trade[i] = -1;
    city_stat_population[i] = -1;    //by popscale
    city_stat_poptomil[i] = -1;    //percentage
    city_stat_military[i] = -1;      //by popscale
    city_stat_king[i] = -1;          //0-5 bad-good
    city_stat_wealth[i] = -1;       //0-5 bad-good
}
