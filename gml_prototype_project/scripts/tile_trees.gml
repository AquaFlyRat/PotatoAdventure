///tile_trees(x,y,csize)
var i = argument[0];
var j = argument[1];
var csize = argument[2];
var dps = -10; //depths
var wtr = -5; //water-beach
var snd = -2; //beach-grass
var fld = 5; //grass-hill
var hil = 10; //hill-high
var mtn = 15; //high-mtn
var trh = 0;
var t0h = 5;
//trees
if treemap[i,j]>=trh && map[i,j]<hil && map[i,j]>snd{
    tile_add(bg_map,7*csize,0,csize,csize,i*csize,j*csize,trdp);
}
