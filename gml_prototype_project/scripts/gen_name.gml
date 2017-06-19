///gen_name(m/f)
var mf = argument[0];
var name = "";
var v1 = string(name_vow[irandom(array_length_1d(name_vow)-1)]);
var v2 = string(name_vow[irandom(array_length_1d(name_vow)-1)]);
var v3 = string(name_vow[irandom(array_length_1d(name_vow)-1)]);
var c1 = string(name_con[irandom(array_length_1d(name_con)-1)]);
var c2 = string(name_con[irandom(array_length_1d(name_con)-1)]);
var c3 = string(name_con[irandom(array_length_1d(name_con)-1)]);
//male
if mf ==0{
    var c = choose(0,1);
    if c = 0{   //vcvc
        name = v1+c1+v2+c2
    }
    if c = 1{   //cvcvc
        name = c1+v1+c2+v2+c3
    }
}
//female
if mf ==1{
    var c = choose(0,1);
    if c = 0{   //cvcv
        name = c1+v1+c2+v2
    }
    if c = 1{   //vcvcv
        name = v1+c1+v2+c2+v3
    }
}
return(string(name))
