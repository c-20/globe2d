// double
#define DBLVAR(varname, defval, minval, maxval, numincs)   \
  double varname##val = defval;                            \
  double varname##min = minval;                            \
  double varname##max = maxval;                            \
  double varname##inc = (maxval - minval) / (numincs - 1.0);
//     varname        default        min       max   steps
DBLVAR(globeceilingx,    0.0,      -10.0,     10.0,  100.0); // top centre
DBLVAR(globeceilingy,    0.0,      -10.0,     10.0,  100.0);
DBLVAR(discfloorz,      60.0,       20.0,     60.0,  100.0); // vertical offset
DBLVAR(discfloorx,       0.0,      -10.0,     10.0,  100.0); // bottom centre
DBLVAR(discfloory,       0.0,      -10.0,     10.0,  100.0);
DBLVAR(discrotangle,    45.0,     -180.0,    180.0,  360.0); // disc angle
DBLVAR(discarmAlength,  20.0,       20.0,     60.0,  100.0); // disc radius
DBLVAR(discarmABangle,  90.0,        0.0,    180.0,  100.0); // segment 1 angle
DBLVAR(discarmBlength,  20.0,       20.0,     60.0,  100.0); // segment 1 length
DBLVAR(discarmBCangle,  90.0,      180.0,      0.0,  100.0); // segment 2 angle
DBLVAR(discarmClength,  20.0,       20.0,     60.0,  100.0); // segment 2 length
DBLVAR(discarmCDangle,   0.0,      -45.0,     45.0,  100.0); // platform angle
DBLVAR(discarmDlength,   0.0,       -5.0,      5.0,  100.0); // platform offset
//         _ _|_ _|_ _ disc|segment1|segment2 radius
//         Q W E R T Y(U I|O P)   (globe centre Y|X)
//          A S D F G(H J|K L)     (disc centre Y|X)
//           Z X C V B(N M),       (disc distance Z)
//  platform^^^^    ^^disc angle
// offset,angle ^^^^segment 1,2 angle
char    decletterkeys[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char    incletterkeys[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettervals[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettervals[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettermins[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettermins[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *declettermaxs[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *inclettermaxs[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *decletterstep[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double *incletterstep[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define VARLETTERKEYS(VarName, DecKey, IncKey)    \
  incletterkeys[IncKey - 'A'] = IncKey;           \
  inclettervals[IncKey - 'A'] = &VarName##val;    \
  inclettermins[IncKey - 'A'] = &VarName##min;    \
  inclettermaxs[IncKey - 'A'] = &VarName##max;    \
  incletterstep[IncKey - 'A'] = &VarName##inc;    \
  decletterkeys[DecKey - 'A'] = DecKey;           \
  declettervals[DecKey - 'A'] = &VarName##val;    \
  declettermins[DecKey - 'A'] = &VarName##min;    \
  declettermaxs[DecKey - 'A'] = &VarName##max;    \
  decletterstep[DecKey - 'A'] = &VarName##inc;
// varkeys initialised in globemouseinit
#define PRESSVARLETTERKEY(Key)                                        \
  if (Key >= 'A' && Key <= 'Z') {                                     \
    if (incletterkeys[Key - 'A'] == Key)                              \
    { *inclettervals[Key - 'A'] += *incletterstep[Key - 'A'];         \
      if (*inclettervals[Key - 'A'] >= *inclettermaxs[Key - 'A'])     \
        { *inclettervals[Key - 'A'] = *inclettermaxs[Key - 'A']; } }  \
    if (decletterkeys[Key - 'A'] == Key)                              \
    { *declettervals[Key - 'A'] -= *decletterstep[Key - 'A'];         \
      if (*declettervals[Key - 'A'] <= *declettermins[Key - 'A'])     \
        { *declettervals[Key - 'A'] = *declettermins[Key - 'A']; } }  \
  } // increment inc key, then/or decrement dec key ....
// varkeys pressed in globemouseupdate
#define GETVARVALUE(VarName)    VarName##val

