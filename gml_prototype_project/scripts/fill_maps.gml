///fill_maps(rsize,csize,height);
var rsize = argument[0];
var csize = argument[1];
var height = argument[2];
var h = height;
//////////////////////////////////////////////////////////////////MAIN MAP
randomize();
while(height>0){
//fill array
for (i=0;i<rsize+csize+1;i++){
    for(j=0;j<rsize+csize+1;j++){
        array[i,j] = random_range(height,-height);
    }
}
//interpolate horizontal
for (i=0;i<rsize+csize;i++){
    for(j=0;j<rsize+csize;j++){
        array[i,j] = array[i,j-j mod csize+csize]*((j mod csize)/csize)+array[i,(j-j mod csize)]*(1-((j mod csize)/csize));
    }
}
//interpolate vertical
for (i=0;i<rsize+csize;i++){
    for(j=0;j<rsize+csize;j++){
        array[i,j] = array[i-i mod csize+csize,j]*((i mod csize)/csize)+array[i-i mod csize,j]*(1-((i mod csize)/csize));
    }
}
//transfer to map
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        map[i,j] += array[i,j];
        //if map[i,j]>mtn+1{map[i,j] = floor(map[i,j])}
    }
}
height= height div 2;
csize = csize div 2;
show_debug_message("MMAP: "+string(height))
}
csize = 8;
height = h;
//////////////////////////////////////////////////////////////////TREES
randomize();
while(height>0){
//fill array
for (i=0;i<rsize+csize+1;i++){
    for(j=0;j<rsize+csize+1;j++){
        array[i,j] = random_range(height,-height);
    }
}
//interpolate horizontal
for (i=0;i<rsize+csize;i++){
    for(j=0;j<rsize+csize;j++){
        array[i,j] = array[i,j-j mod csize+csize]*((j mod csize)/csize)+array[i,(j-j mod csize)]*(1-((j mod csize)/csize));
    }
}
//interpolate vertical
for (i=0;i<rsize+csize;i++){
    for(j=0;j<rsize+csize;j++){
        array[i,j] = array[i-i mod csize+csize,j]*((i mod csize)/csize)+array[i-i mod csize,j]*(1-((i mod csize)/csize));
    }
}
//transfer to map
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        treemap[i,j] += array[i,j];
        //if map[i,j]>mtn+1{map[i,j] = floor(map[i,j])}
    }
}
height= height div 2;
csize = csize div 2;
show_debug_message("TMAP: "+string(height))
}
//////////////////////////////////////////////////////////////////CIV
randomize();
//fill array
for (i=0;i<rsize+csize+1;i++){
    for(j=0;j<rsize+csize+1;j++){
        array[i,j] = -1;
    }
}
var cnum = 0;
while cnum<=citynum{
    var ax = irandom(rsize-1);
    var ay = irandom(rsize-1);
    if array[ax,ay] ==-1 && map[ax,ay]>5{
        array[ax,ay] = 1
        show_debug_message("CMAP: "+string(ax)+","+string(ay));
        city_x[cnum] = ax;
        city_y[cnum] = ay;
        city_faction[cnum] = cnum;
        gen_city(cnum);
        cnum++;
    }
}
var cnum = 0;
while cnum<=10{
    var ax = irandom(rsize-1);
    var ay = irandom(rsize-1);
    if array[ax,ay] ==-1 && map[ax,ay]>5{
        array[ax,ay] = irandom_range(2,5)
        show_debug_message("CMAP: "+string(ax)+","+string(ay));
        cnum++;
    }
}
//transfer to map
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        civmap[i,j] += array[i,j];
    }
}
