// innerdiscamplitude is globeamplitude


char segmentAactuatorA[] = "Segment A Actuator A";
char segmentAactuatorAbox[] = "Segment A Actuator A Box";
char segmentAactuatorB[] = "Segment A Actuator B";
char segmentAactuatorBbox[] = "Segment A Actuator B Box";
char segmentAbox[] = "Segment A Box";
char segmentBbox[] = "Segment B Box";

double discamplitude = 60.0; // 60 units of amplitude
double discoffsetangle = 0.0; // could be adjustable .. not yet used ..
double discstartmarginpct = 0.1; // 10% from minimum
double discstopmarginpct  = 0.1; // 10% from maximum

double segmentAboxwidth  =  10.0; // amplitude units
double segmentAboxheight =   2.0;
// ^^^^^ why so narrow
//       if amplitude is 60

//double segserifwidthpct = 0.2; // -| serif width is 20% of segment A offset length

double segmentAoffsetlen = 10.0; // 1 units of amplitude
double segmentAactAbotlen = 3.0;
double segmentAactAboxlen = 7.0; // 6 units of amplitude
double segmentAactAvrangelen = 6.0; // 5 units of amplitude
double segmentAactAtoplen = 2.0; // 1 unit of amplitude
double segmentAactAboxwidth = 1.5;
double segmentAactAboxheight = 1.5;

double segmentAangleheight = 5.0;
double segmentBoffsetlen = 10.0;

double segmentAactBbotlen = 3.0;
double segmentAactBboxlen = 7.0; // 5 units of amplitude
double segmentAactBvrangelen = 6.0; // 6 units of amplitude
double segmentAactBtoplen = 2.0; // 2 units of amplitude
double segmentAactBboxwidth = 1.5;
double segmentAactBboxheight = 1.5;

double segmentBboxwidth = segmentAboxwidth;
double segmentBboxheight = segmentAboxheight;
double segmentBboxlength = 30.0; // length of first bending segment
//double segmentCwidth = segmentBwidth;
//double segmentCheight = segmentBheight;
//double segmentClen = 80.0; // length of second bending segment
//double segmentDwidth = segmentCwidth;
//double segmentDheight = segmenCAheight;
//double segmentDlen = 30.0; // length of target platform segment

char normalactuatorscript[] = "C0,1;L1,2;R2,3;C3,4";


Pt3Script *segmentscripts; // mem of each step

//Pt3Script segmentA, segmentAbox;
//Pt3Script segmentB, segmentBbox;
//Pt3Script segmentC, segmentCbox;
//Pt3Script segmentD, segmentDbox;
//char    segmentAscript[] = "C0,1;R1,2,3;R3,4;R3,5"; // 6 points
//char    segmentBscript[] = "C0,1;R1,2,3;R3,4;R3,5"; // 6 points
//char    segmentCscript[] = "C0,1;R1,2,3;R3,4;R3,5"; // 6 points
//char    segmentDscript[] = "C0,1;R1,2,3;R3,4;R3,5"; // 6 points
//char segmentAboxscript[] = "C0,1,2,3,0;C0,4;C1,5;C2,6;C3,7;C4,5,6,7,4"; // 8 points
Pt3ScriptList *segments;
/*
Pt3ScriptList armseg0box, armseg0vec;
Pt3ScriptList armseg1box, armseg1vec;
Pt3ScriptList armseg2box, armseg2vec;
Pt3ScriptList armseg3box, armseg3vec; // consider dynamic allocation
*/
// double
#define DBLVAR(varname, defval, minval, maxval, numincs)   \
  double varname##val = defval;                            \
  double varname##min = minval;                            \
  double varname##max = maxval;                            \
  double varname##inc = (maxval - minval) / (numincs - 1.0);
//     varname        default        min       max   steps
//DBLVAR(globeceilingx,    0.0,      -10.0,     10.0,  100.0); // top XY offset          +-
//DBLVAR(globeceilingy,    0.0,      -10.0,     10.0,  100.0); //  (globe middle)        +|
//DBLVAR(discfloorx,       0.0,      -10.0,     10.0,  100.0); //   (disc middle)        +|
//DBLVAR(discfloory,       0.0,      -10.0,     10.0,  100.0); // DISTANCE FROM GLOBE  |--|
// Global Zoom = v b
DBLVAR(globalzoom,       1.0,        0.1,    100.0,  5000.0); // ZOOM IN / OUT
DBLVAR(tiltback,         0.0,      -90.0,     90.0,  1000.0); // TILT BACK / FORWARD
DBLVAR(tiltacross,       0.0,      -90.0,     90.0,  1000.0); // TILT LEFT / RIGHT
DBLVAR(shifthorizontal,  0.0,     -900.0,    900.0,  1000.0); // SHIFT LEFT / RIGHT
DBLVAR(shiftvertical,    0.0,     -900.0,    900.0,  1000.0); // SHIFT UP / DOWN
DBLVAR(rotatehorizontal, 0.0,      -90.0,     90.0,  1000.0); // ROTATE LEFT / RIGHT
DBLVAR(rotatevertical,   0.0,      -90.0,     90.0,  1000.0); // ROTATE UP / DOWN
// Disc Vertical Offset / Disc Radius Size = K L / k l
DBLVAR(discfloorz,      60.0,       20.0,     60.0,   100.0); // bottom XY offset       +-
DBLVAR(discradius,      20.0,       60.0,    100.0,   100.0); // disc radius            -|
// Disc Rotation = n m
DBLVAR(discrotangle,    45.0,     -180.0,    180.0,   360.0); // disc angle/SEG A ANGLE  <
// segAlength, segBlength, segClength, segDlength ..... 1 vs +1 vs ^1
// Segment A Offset / Segment B Offset = q w / e r
DBLVAR(segAactAextpct,      0.6,      0.0,      1.0,  100.0); // pos at % of segA
DBLVAR(segAactBextpct,      0.6,      0.0,      1.0,  100.0); // pos at % of segA
//DBLVAR(segBoffsetpct,      0.6,      0.0,      1.0,  100.0); // pos at % of segB
// Segment D Offset / Angle / Extend = t y / u i / o p
//DBLVAR(segDoffsetpct,      0.6,      0.0,      1.0,  100.0); // offset % of segD
//DBLVAR(segDanglepct,       0.6,      0.0,      1.0,  100.0); //  angle % of segD
//DBLVAR(segDextendpct,      0.6,      0.0,      1.0,  100.0); // extend % of segD


// ^ limit discsegAoff + discsegAlen to [outer-inner]discdistance
//DBLVAR(discsegApos,     20.0,       20.0,     60.0,  100.0); // SEGMENT A OFFSET       -|
// ^ limit max pos to discsegAlen - discsegAoff
// ^ min pos should be discsegAoff
//DBLVAR(discsegAlen,     20.0,       20.0,     60.0,  100.0); // SEGMENT A LENGTH       --
//DBLVAR(segABangle,  90.0,        0.0,    180.0,  100.0); // SEGMENT AB ANGLE       ->
//DBLVAR(segBpct,      0.0,        1.0,     50.0,  100.0);
//DBLVAR(discsegBpos,     20.0,       20.0,     60.0,  100.0); // SEGMENT B OFFSET       -|
//DBLVAR(discsegBlen,     20.0,       20.0,     60.0,  100.0); // segment 1 length       --
//DBLVAR(segBCangle,  90.0,      180.0,      0.0,  100.0); // SEGMENT BC ANGLE       ->
//DBLVAR(discsegClen,     20.0,       20.0,     60.0,  100.0); // segment 2 length       --

//DBLVAR(platformx,       40.0,       80.0,      0.0,  100.0);
//DBLVAR(platformy,       40.0,        0.0,     80.0,  100.0);
//DBLVAR(platformz,        0.0,        0.0,     10.0,  100.0);

//DBLVAR(platformangle,    0.0,      -45.0,     45.0,  100.0); // platform angle         ->
//DBLVAR(platformpos,      0.0,       -5.0,      5.0,  100.0); // WRIST OFFSET           -|
//DBLVAR(platformoff,    -10.0,        0.0,    -20.0,  100.0); // wrist platform offset  -
//DBLVAR(platformlen,     20.0,       20.0,     40.0,  100.0); // wrist platform length  --
//         _ _|_ _|_ _ disc|segment1|segment2 radius
//         Q W E R T Y(U I|O P)   (globe centre Y|X)
//          A S D F G(H J|K L)     (disc centre Y|X)
//           Z X C V B(N M),       (disc distance Z)
//  platform^^^^    ^^disc angle
// offset,angle ^^^^segment 1,2 angle
char    decletterkeysUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char    decletterkeysLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char    incletterkeysUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char    incletterkeysLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettervalsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettervalsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettervalsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettervalsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *decletterminsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *decletterminsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *incletterminsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *incletterminsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettermaxsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettermaxsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettermaxsUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettermaxsLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *decletterstepUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *decletterstepLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *incletterstepUC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *incletterstepLC[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define ISUC(ch)        (ch >= 'A' && ch <= 'Z')
#define ISLC(ch)        (ch >= 'a' && ch <= 'z')
#define ISLETTER(ch)    (ISUC(ch) || ISLC(ch))
#define VARLETTERKEYS(VarName, DecKey, IncKey)        \
  if ISLETTER(IncKey) {                               \
    if ISUC(IncKey) {                                 \
      incletterkeysUC[IncKey - 'A'] = IncKey;           \
      inclettervalsUC[IncKey - 'A'] = &VarName##val;    \
      incletterminsUC[IncKey - 'A'] = &VarName##min;    \
      inclettermaxsUC[IncKey - 'A'] = &VarName##max;    \
      incletterstepUC[IncKey - 'A'] = &VarName##inc;    \
    } else if ISLC(IncKey) {                          \
      incletterkeysLC[IncKey - 'a'] = IncKey;           \
      inclettervalsLC[IncKey - 'a'] = &VarName##val;    \
      incletterminsLC[IncKey - 'a'] = &VarName##min;    \
      inclettermaxsLC[IncKey - 'a'] = &VarName##max;    \
      incletterstepLC[IncKey - 'a'] = &VarName##inc;    \
    }                                                   \
  } ; if ISLETTER(DecKey) {                             \
    if ISUC(DecKey) {                                 \
      decletterkeysUC[DecKey - 'A'] = DecKey;           \
      declettervalsUC[DecKey - 'A'] = &VarName##val;    \
      decletterminsUC[DecKey - 'A'] = &VarName##min;    \
      declettermaxsUC[DecKey - 'A'] = &VarName##max;    \
      decletterstepUC[DecKey - 'A'] = &VarName##inc;    \
    } else if ISLC(DecKey) {                            \
      decletterkeysLC[DecKey - 'a'] = DecKey;           \
      declettervalsLC[DecKey - 'a'] = &VarName##val;    \
      decletterminsLC[DecKey - 'a'] = &VarName##min;    \
      declettermaxsLC[DecKey - 'a'] = &VarName##max;    \
      decletterstepLC[DecKey - 'a'] = &VarName##inc;    \
    }                                                   \
  }
// varkeys initialised in globemouseinit
#define PRESSVARLETTERKEY(Key)                                               \
  if ISLETTER(Key) {                                                         \
    if ISUC(Key)                                                             \
    { if (incletterkeysUC[Key - 'A'] == Key)                                 \
      { *inclettervalsUC[Key - 'A'] += *incletterstepUC[Key - 'A'];          \
        if (*inclettervalsUC[Key - 'A'] >= *inclettermaxsUC[Key - 'A'])      \
          { *inclettervalsUC[Key - 'A'] = *inclettermaxsUC[Key - 'A']; } }   \
      if (decletterkeysUC[Key - 'A'] == Key)                                 \
      { *declettervalsUC[Key - 'A'] -= *decletterstepUC[Key - 'A'];          \
        if (*declettervalsUC[Key - 'A'] <= *decletterminsUC[Key - 'A'])      \
          { *declettervalsUC[Key - 'A'] = *decletterminsUC[Key - 'A']; } } } \
    else if ISLC(Key)                                                        \
    { if (incletterkeysLC[Key - 'a'] == Key)                                 \
      { *inclettervalsLC[Key - 'a'] += *incletterstepLC[Key - 'a'];          \
        if (*inclettervalsLC[Key - 'a'] >= *inclettermaxsLC[Key - 'a'])      \
          { *inclettervalsLC[Key - 'a'] = *inclettermaxsLC[Key - 'a']; } }   \
      if (decletterkeysLC[Key - 'a'] == Key)                                 \
      { *declettervalsLC[Key - 'a'] -= *decletterstepLC[Key - 'a'];          \
        if (*declettervalsLC[Key - 'a'] <= *decletterminsLC[Key - 'a'])      \
          { *declettervalsLC[Key - 'a'] = *decletterminsLC[Key - 'a']; } } } \
  } // of UC/LC, increment inc key, then/or decrement dec key ...
// varkeys pressed in globemouseupdate
#define GETVARVALUE(VarName)    VarName##val
#define VARVAL(VarName)         GETVARVALUE(VarName)
