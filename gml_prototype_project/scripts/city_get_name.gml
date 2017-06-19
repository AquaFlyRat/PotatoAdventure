///city_get_name(num);
var c = argument[0];
/////////////////////////////
//      NUM     TRADE
//      1       farm
//      2       fish/hunt
//      3       lumber
//      4       mining
//      5       smith
//      6
//////////////////////////////
var ct = city_trade[c];
var name = "";
if ct ==1{
    var la = array_length_1d(name_farm_a)-1;
    var ia = choose(" ","");
    var lb = array_length_1d(name_farm_b)-1;
    var ib = " ";
    var lc = array_length_1d(name_farm_c)-1;
    name = string(name_farm_a[irandom(la)])+string(ia)+string(name_farm_b[irandom(lb)])+string(ib)+string(name_farm_c[irandom(lc)])
}
if ct ==2{
    var la = array_length_1d(name_hunt_a)-1;
    var ia = choose(" ","");
    var lb = array_length_1d(name_hunt_b)-1;
    var ib = " ";
    var lc = array_length_1d(name_hunt_c)-1;
    name = string(name_hunt_a[irandom(la)])+string(ia)+string(name_hunt_b[irandom(lb)])+string(ib)+string(name_hunt_c[irandom(lc)])
}
if ct ==3{
    var la = array_length_1d(name_mill_a)-1;
    var ia = choose(" ","");
    var lb = array_length_1d(name_mill_b)-1;
    var ib = " ";
    var lc = array_length_1d(name_mill_c)-1;
    name = string(name_mill_a[irandom(la)])+string(ia)+string(name_mill_b[irandom(lb)])+string(ib)+string(name_mill_c[irandom(lc)])
}
if ct ==4{
    var la = array_length_1d(name_mine_a)-1;
    var ia = choose(" ","");
    var lb = array_length_1d(name_mine_b)-1;
    var ib = " ";
    var lc = array_length_1d(name_mine_c)-1;
    name = string(name_mine_a[irandom(la)])+string(ia)+string(name_mine_b[irandom(lb)])+string(ib)+string(name_mine_c[irandom(lc)])
}
if ct ==5{
    var la = array_length_1d(name_smith_a)-1;
    var ia = choose(" ","");
    var lb = array_length_1d(name_smith_b)-1;
    var ib = " ";
    var lc = array_length_1d(name_smith_c)-1;
    name = string(name_smith_a[irandom(la)])+string(ia)+string(name_smith_b[irandom(lb)])+string(ib)+string(name_smith_c[irandom(lc)])
}
return(name);
show_debug_message("CNAME: "+string(c))
