///init_godstat();
////////////////////////////
//      Num     Godof
//      0       Life
//      1       death
//      2       love
//      3       war
//      4       fertility
//      5       hunt
//      6       light
//      7       fields
//      8       magic
//      9       time
////////////////////////////
god_stat_gender[0]="god";
god_stat_gender[1]="godess";
var n = 0;
god_stat_of[n]="life";n++;
god_stat_of[n]="death";n++;
god_stat_of[n]="love";n++;
god_stat_of[n]="war";n++;
god_stat_of[n]="fertility";n++;
god_stat_of[n]="the hunt";n++;
god_stat_of[n]="light";n++;
god_stat_of[n]="the fields";n++;
god_stat_of[n]="magic";n++;
god_stat_of[n]="the sea";n++;
god_stat_of[n]="time";n++;
///
for (i=0;i<array_length_1d(god_stat_of);i++){
    godjob[i] = 1;
}
