///world_advance(years)
var ctnm = citynum;
repeat(argument[0]){
    desc_add("");
    desc_add("year: "+string(year));
    for (i=0;i<=ctnm;i++){
        //advance population
        var popinc = random_range(0,1);
        city_stat_population[i] += popinc;
        if city_stat_population[i]<=0{
            rebuild_city(i);
            desc_add(string(city_name[i])+" collapsed and rebuilt their city#and joined faction:"+string(city_faction[i]));
        }
        city_stat_military[i] += popinc*city_stat_poptomil[i];
        //advance affairs
        var ptcinc = random_range(-1,1);
        city_stat_king[i]+=ptcinc;
            city_stat_king[i] = clamp(city_stat_king[i],0,5);
        city_stat_wealth[i] +=(city_stat_king[i]-3)/5;
            city_stat_wealth[i] = clamp(city_stat_wealth[i],0,5);
        //check revolution
        if city_stat_wealth[i]<=4 && city_stat_king[i]<=4{
            var revchance = .25;
            revchance+=(3-city_stat_wealth[i])/5;
            revchance+=(3-city_stat_king[i])/5;
            revchance = clamp(revchance,0,1);
            revchance = revchance*2;
            if revchance = random(revchance){
                city_faction[i] = irandom(factionnum);
                city_stat_king[i] = random_range(3,5);
                desc_add(string(city_name[i])+"["+string(city_faction[i])+"]"+" overthrew their king#and joined faction:"+string(city_faction[i]));
            }
        }
        //check wars
        for (j=0;j<=citynum;j++){
            var ct = city_faction[j];
            if ct != city_faction[i]{
                var ax = city_x[i];
                var ay = city_y[i];
                var bx = city_x[j];
                var by = city_y[j];
                if distance(ax,ay,bx,by)<=5{
                    //war
                    var warchance = 0;
                    if city_race[i] != city_race[j]{warchance++}else{warchance--}
                    if city_god[i] != city_god[j]{warchance++}else{warchance-=2}
                    if city_trade[i] == city_trade[j]{warchance++}else{warchance--}
                    if city_stat_wealth[i]<city_stat_wealth[j]{warchance++}else{warchance--}
                    if city_stat_king[i]>city_stat_king[j]{warchance++}else{warchance--}
                    if city_stat_king[i]>city_stat_king[j]{warchance++}else{warchance--}
                    if city_stat_military[i]>city_stat_military[j]{warchance++}else{warchance--}
                    warchance = clamp(warchance,0,10);
                    warchance = 10-warchance;
                    if warchance = irandom(warchance){
                        //battle
                        desc_add(string(city_name[i])+"["+string(city_faction[i])+"]"+" went to war with "+string(city_name[j])+"["+string(city_faction[j])+"]");
                        var winchance = 0;
                        winchance+=sign(city_stat_wealth[i]-city_stat_wealth[j])
                        winchance+=sign(city_stat_military[i]-city_stat_military[j])
                        winchance+=sign(city_stat_king[i]-city_stat_king[j])
                        winchance = clamp(winchance,0,3);
                        winchance = 3-winchance;
                        if winchance = irandom(winchance){
                            desc_add(string(city_name[i])+"["+string(city_faction[i])+"]"+" won the war and sacked "+string(city_name[j])+"["+string(city_faction[j])+"]");
                            //win
                            var popdec = random_range(-1,0);
                            city_stat_population[i] += popdec;
                            city_stat_military[i] += popdec*city_stat_poptomil[i];
                            city_stat_wealth[i]+=random_range(1,3);
                                city_stat_wealth[i] = clamp(city_stat_wealth[i],0,5);
                            var loss = irandom_range(-3,-1);
                            var d = ""
                            if loss ==-1{d = "light"}
                            if loss ==-2{d = "moderate"}
                            if loss ==-3{d = "heavy"}
                            desc_add(string(city_name[j])+" took "+d+" casualties");
                            city_stat_population[j] += loss;
                                city_stat_population[j] = clamp(city_stat_population[j],1,25);
                            city_stat_military[j] += loss*city_stat_poptomil[j];
                            city_stat_king[j] = city_stat_king[i];
                            city_stat_wealth[j] = city_stat_wealth[i];
                            city_faction[j] = city_faction[i];
                        }else{
                            desc_add(string(city_name[j])+"["+string(city_faction[j])+"]"+" won the war and sacked "+string(city_name[i])+"["+string(city_faction[i])+"]");
                            //loss
                            var popdec = random_range(-1,0);
                            city_stat_population[j] += popdec;
                            city_stat_military[j] += popdec*city_stat_poptomil[j];
                            city_stat_wealth[j]+=random_range(1,3);
                                city_stat_wealth[j] = clamp(city_stat_wealth[j],0,5);
                            var loss = irandom_range(-3,-1);
                            var d = ""
                            if loss ==-1{d = "light"}
                            if loss ==-2{d = "moderate"}
                            if loss ==-3{d = "heavy"}
                            desc_add(string(city_name[i])+" took "+d+" casualties");
                            city_stat_population[i] += loss;
                                city_stat_population[i] = clamp(city_stat_population[i],1,25);
                            city_stat_military[i] += loss*city_stat_poptomil[i];
                            city_stat_king[i] = city_stat_king[i];
                            city_stat_wealth[i] = city_stat_wealth[j];
                            city_faction[i] = city_faction[j];
                        }
                        //check reset
                        if city_stat_population[i]<=0{
                            rebuild_city(i);
                            desc_add(string(city_name[i])+" collapsed and rebuilt their city#and joined faction:"+string(city_faction[i]));
                        }   
                        //check reset
                        if city_stat_population[j]<=0{
                            rebuild_city(j);
                            desc_add(string(city_name[j])+" collapsed and rebuilt their city#and joined faction:"+string(city_faction[j]));
                        }
                    }
                }
            }
        }
        //build new cities
        if city_stat_wealth[i]>=2 || city_stat_population[i]>=2{
            var cg = 0;
            while cg=0{
                var ax = city_x[i]+irandom_range(-5,5);
                var ay = city_y[i]+irandom_range(-5,5);
                if (ax>1 && ax<rsize-1 && ay>1 && ay<rsize-1){
                    var c = citynum+1;
                    if civmap[ax,ay]==-1 && map[ax,ay]>5{
                        city_faction[c] = city_faction[i];
                        build_city(c,ax,ay);
                        desc_add(string(city_name[c])+"["+string(city_faction[c])+"]"+" was founded at ["+string(ax)+","+string(ay)+"]");
                    }
                }
                cg++;
            } 
        }
    }
    year++;
}
desc_add(" ")
desc_add("   Total City Number:"+string(floor(citynum)))
desc_add("   Total World Population:"+string(floor(world_get_pop())))
