///gen_maps(rsize)
var rsize = argument[0];
//gen maps
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        map[i,j] = 0;
    }
}
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        treemap[i,j] = 0;
    }
}
for (i=0;i<rsize;i++){
    for(j=0;j<rsize;j++){
        civmap[i,j] = 0;
    }
}
