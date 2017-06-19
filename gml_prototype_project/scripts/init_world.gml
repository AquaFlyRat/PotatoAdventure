///init_world()
//init
csize = 8;
rsize = 32;
height = 15;
global.ssn = choose(0,1,2,3);
dps = -10; //depths
wtr = -5; //water-beach
snd = -2; //beach-grass
fld = 5; //grass-hill
hil = 10; //hill-high
mtn = 15; //high-mtn
trh = 0;
t0h = 5;
//heights
trdp = -10;
var h;
//init room
room_width = csize*rsize;
room_height= csize*rsize;
//create map
gen_maps(rsize);
fill_maps(rsize,csize,height);
csize = 8
/*
for (i=0;i<rsize;i++){
    for (j = 0;j<rsize;j++){
        tile_map(i,j,csize);
    }
}
*/
desc_add("")
desc_add("Press Enter to Continue")
