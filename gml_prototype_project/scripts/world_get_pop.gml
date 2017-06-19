///world_get_pop();
var pop = 0;
for(j=0;j<=1000;j++){
    if city[j] ==-1{break;}else{
        pop+=city_stat_population[j];
    }
}
return(pop*popscale);
