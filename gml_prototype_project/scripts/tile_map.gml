///tile_map(x,y,csize)
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
var res = 0;
if map[i,j]<=dps{res = 0;}
if map[i,j]>dps && map[i,j]<=wtr {res = 1;}
if map[i,j]>wtr && map[i,j]<=snd {res = 2;}
if map[i,j]>snd && map[i,j]<=fld {res = 3;}
if map[i,j]>fld && map[i,j]<=hil {res = 4;}
if map[i,j]>hil && map[i,j]<=mtn {res = 5;}
if map[i,j]>mtn {res = 6;}
tile_add(bg_map,(res)*csize,0,csize,csize,i*csize,j*csize,0);
tile_trees(i,j,csize);
