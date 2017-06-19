///init_gods()
init_godstat();
godnum = irandom_range(4,10);
var fatherlist = -1;
var firstgods = godnum div 2;
for (i=0;i<firstgods;i++){
    god[i] = 1;
    god_gender[i] = choose(0,1);
    god_parent[i] = choose("the stars","the darkness","the universe","nothingness")
    if i!=0{
        god_of[i] = irandom(array_length_1d(godjob)-1);
        while godjob[god_of[i]]== -1{
            god_of[i] = irandom(array_length_1d(godjob)-1);
        }
        godjob[god_of[i]] = -1;
    }else{
        god_of[i] = 0;
        godjob[0] = -1;
    }
    god_name[i] = gen_name(god_gender[i]);
    desc_add(string(god_name[i])+", "+string(god_stat_gender[god_gender[i]])+" of "+string(god_stat_of[god_of[i]])+" was born of "+string(god_parent[i])+".")
}
var fatherlist = firstgods-1;
for (i=firstgods;i<godnum;i++){
    god[i] = 1;
    god_gender[i] = choose(0,1);
    god_parent[i] = god_name[irandom(fatherlist)]
    if i!=0{
        god_of[i] = irandom(array_length_1d(godjob)-1);
        while godjob[god_of[i]]== -1{
            god_of[i] = irandom(array_length_1d(godjob)-1);
        }
        godjob[god_of[i]] = -1;
    }else{
        god_of[i] = 0;
        godjob[0] = -1;
    }
    god_name[i] = gen_name(god_gender[i]);
    desc_add(string(god_name[i])+", "+string(god_stat_gender[god_gender[i]])+" of "+string(god_stat_of[god_of[i]])+" was born of "+string(god_parent[i])+".")
}
desc_add("");
