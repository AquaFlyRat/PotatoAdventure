///city_get_trade(num);
var cn = argument[0];
//////////////////////////
//      NUM     TRADE
//      1       farm
//      2       fish/hunt
//      3       lumber
//      4       mining
//      5       smith
//      6       
//////////////////////////
var ax = city_x[cn];
var ay = city_y[cn];
var t1 = 0;
var t2 = 0;
var t3 = 0;
var t4 = 0;
var t5 = 0;
for (i=ax-2;i<ax+2;i++){
    for (j=ay-2;j<ay+2;j++){
        if i>0 && j>0 && i!=rsize && j!=rsize{
            if map[i,j]<wtr{t2++}
            if map[i,j]>=wtr && map[i,j]<snd{t1++;t3++;}
            if map[i,j]>=snd && map[i,j]<fld{t1++;t2++;}
            if map[i,j]>=fld && map[i,j]<hil{t3++;t5++;}
            if map[i,j]>=hil && map[i,j]<mtn{t4++;t2++;}
            if map[i,j]>=mtn {t4++}
        }
    }
}
            //bug: maximum of equal values equals both. city equal at t5 and t1 will default to t1
if max(t1,t2,t3,t4,t5)==t5{return(5)}
if max(t1,t2,t3,t4,t5)==t4{return(4)}
if max(t1,t2,t3,t4,t5)==t3{return(3)}
if max(t1,t2,t3,t4,t5)==t2{return(2)}
if max(t1,t2,t3,t4,t5)==t1{return(1)}
