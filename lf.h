// #include "code.h"

//#define LNMAXBOUND 1000000000000000000LL // less is 18 digits
#define LNOVERFLOW  9111111111111111111LL // 19 digits
#define LFOVERFLOW  (LF){ LNOVERFLOW, LNOVERFLOW };
#define LNOUTOFRANGE  9222222222222222222LL // 19 digits
#define LFOUTOFRANGE  (LF){ LNOUTOFRANGE, LNOUTOFRANGE };

#define LFDIGITLIMIT	18
#define LFMFRACLIMIT	17 // because 0.234567890123456781* 8 = 19 digits..
// also, when adding 18 digits, the next scale variable will be 19 digits wide..
// .. may need to handle a special case to allow 18 width and recover maybe 2 of the 3 digits..
// currently, 18 digit numbers fail so limit of 17 is used ....

//#define LFMFRACLIMIT	18
// 99 * 9 = 189 --- cannot exceed the 9 in the uppermost 19th digit
// so it should work without cutting any digits off ....


// -------------------------------------- LN SIZE/SCALE QUERY FUNCTIONS -------

IN totaldigits(LN value) {
  IN digits = 0;
  WI (value NQ 0) {
    IN digit = value % 10;
    INC digits;
    value DIVS 10;
  } ; RT digits;
}
IN scaledigits(LN value) {
  IN digits = 0;
  WI (value NQ 0) { // should work for negative
    IF (value % 10 EQ 0) {
      INC digits;
      value DIVS 10;
    } EL { BK; }
  } ;  RT digits;
}
LN unscale(LN value) {
  WI (value NQ 0) {
    IF (value % 10 EQ 0)
      { value = value / 10; }
    EL { RT value; }
  } ; RT value;
}
CS zeros = "00000000000000000000"; // 20 digits per 64-bit long (uses less)
CS scalestring(LN value) {
  CS zero = zeros;
  WI (*zero) { INC zero; }
  RT zero - scaledigits(value);
}
LN scalevalue(LN value) {
// lnfscalevalue - long fraction scale value
// lnnscalevalue - long number scale value
// in both cases, scale 0s are stored at the low end
// 1000.0001 -> { 1000, 1000 } -> scale == 3, 3.. returns 1000
  LN scale = 1;
  WI (value NQ 0) {
    IF (value % 10 EQ 0) {
      value DIVS 10;
      scale MULS 10;
    } EL { RT scale; }
  } ; RT scale;
}
//LN valuescale(LN value) {
LN lnvaluescale(LN value) {
  // scalevalue gives the scale of the scale digits only
  // lnvaluescale gives the scale of the full value
  // valuescale of 123000 is 1000000.. next digit is 7...
  LN scale = 1;
  WI (value NQ 0) {
    value DIVS 10;
    scale MULS 10;
  }
  RT scale;
}
LN lnpow(LN value, IN exponent) {
  LN result = 1; // 10^1 is 10, 10^0 is 1... 9^1 is 9, 9^0 is 1...
  WI (DEC exponent GQ 0)
    { result MULS value; }
  RT result;
}

// -------------------------------------- LF POLARITY QUERY FUNCTIONS ---------

LF lfinvert(LF value) {
  IF (value.numb LT 0 OR value.frac LT 0) {
    IF (value.numb LT 0)
      { value.numb = 0 - value.numb; }
    IF (value.frac LT 0)
      { value.frac = 0 - value.frac; }
  } EL { // neg to pos if either neg, else pos to neg
    IF (value.numb EQ 0)
      { value.frac = 0 - value.frac; }
    EL { value.numb = 0 - value.numb; }
  }
  RT value;
}
LF lfpositive(LF value) {
  IF (value.numb GQ 0 AND value.frac GQ 0)
    { RT value; }
  EL { RT lfinvert(value); }
}
LF lfnegative(LF value) {
  IF (value.numb GQ 0 AND value.frac GQ 0)
    { RT lfinvert(value); }
  EL { RT value; }
}
CH lfispositive(LF value) {
  IF (value.numb GT 0 AND value.frac GQ 0) { RT 1; } // 1.0, 2.1
  EF (value.numb EQ 0 AND value.frac GT 0) { RT 1; } // 0.2
  EL { RT 0; } // all others considered non-positive
//  RT (value.numb GT 0 AND value.frac GQ 0) ? 1 : 0;
} // note that 0 is not positive but 1.0 is
CH lfiszero(LF value) {
  RT (value.numb EQ 0 AND value.frac EQ 0) ? 1 : 0;
} // note that 0.0 is zero but 1.0 is not
CH lfnotzero(LF value) {
  RT (value.numb NQ 0 OR value.frac NQ 0) ? 1 : 0;
}
CH lfisunsigned(LF value) {
  RT (value.numb GQ 0 AND value.frac GQ 0) ? 1 : 0;
} // unsigned if positive or zero
CH lfisnegative(LF value) {
  RT (value.numb GQ 0 AND value.frac GQ 0) ? 0 : 1;
} // negative if either component is negative
CH lfissigned(LF value) {
  RT lfisnegative(value);
}


// ----------------------------------------------- LF DEBUG FUNCTIONS ---------


// moved from later in document for debugging access
VD printlf(LF lf, CS prefix, CS suffix) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  IF (totaldigits(numval) GT 18) {
    // overflow numbers will be red
    printf("%s%s%lld.%s%lld%s", RFGC, neg, numval, fracpad, fracval, OFFC);
  } EL {
    // presumed valid number gets normal formatting
    printf("%s%s%lld.%s%lld%s", prefix, neg, numval, fracpad, fracval, suffix);
  }
}
VD printlfnf(LF lf, CS prefix, CS suffix, IN ndigits, IN fdigits) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  WI (totaldigits(fracval) GT fdigits)
    { fracval DIVS 10; } // hopefully not infinite
  IF (totaldigits(numval) GT 18) {
    // overflow numbers will be red
    printf("%s%s%lld.%s%lld%s", RFGC, neg, numval, fracpad, fracval, OFFC);
  } EL {
    // presumed valid number gets normal formatting
    CH outformat[30];
    sprintf(outformat, "%%s%%s%%%dlld.%%s%%-%dlld%%s", ndigits, fdigits);
    printf(outformat, prefix, neg, numval, fracpad, fracval, suffix);
  }
}
VD printlfn(LF lf, CS prefix, CS suffix, IN ndigits) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  IF (totaldigits(numval) GT 18) {
    // overflow numbers will be red
    printf("%s%s%lld.%s%lld%s", RFGC, neg, numval, fracpad, fracval, OFFC);
  } EL {
    // presumed valid number gets normal formatting
    CH outformat[30];
    sprintf(outformat, "%%s%%s%%%dlld.%%s%%lld%%s", ndigits);
    printf(outformat, prefix, neg, numval, fracpad, fracval, suffix);
  }
}


#define LOGlf(lf)          printlf(lf, "", "")
#define RLOGlf(lf)         printlf(lf, RFGC,  OFFC)
#define RLOGlfn(lf, n)     printlfn(lf, RFGC,  OFFC, n)
#define RLOGlfnf(lf, n, f) printlfnf(lf, RFGC,  OFFC, n, f)
#define DRLOGlf(lf)        printlf(lf, DRFGC, OFFC)
#define YLOGlf(lf)         printlf(lf, YFGC,  OFFC)
#define YLOGlfn(lf, n)     printlfn(lf, YFGC,  OFFC, n)
#define YLOGlfnf(lf, n, f) printlfnf(lf, YFGC,  OFFC, n, f)
#define DYLOGlf(lf)        printlf(lf, DYFGC, OFFC)
#define GLOGlf(lf)         printlf(lf, GFGC,  OFFC)
#define GLOGlfn(lf, n)     printlfn(lf, GFGC,  OFFC, n)
#define GLOGlfnf(lf, n, f) printlfnf(lf, GFGC,  OFFC, n, f)
#define DGLOGlf(lf)        printlf(lf, DGFGC, OFFC)
#define CLOGlf(lf)         printlf(lf, CFGC,  OFFC)
#define CLOGlfn(lf, n)     printlfn(lf, CFGC,  OFFC, n)
#define CLOGlfnf(lf, n, f) printlfnf(lf, CFGC,  OFFC, n, f)
#define DCLOGlf(lf)        printlf(lf, DCFGC, OFFC)
#define BLOGlf(lf)         printlf(lf, BFGC,  OFFC)
#define DBLOGlf(lf)        printlf(lf, DBFGC, OFFC)
#define MLOGlf(lf)         printlf(lf, MFGC,  OFFC)
#define DMLOGlf(lf)        printlf(lf, DMFGC, OFFC)
#define KLOGlf(lf)         printlf(lf, KFGC,  OFFC)
#define DLOGlf(lf)         printlf(lf, DFGC,  OFFC)
#define LLOGlf(lf)         printlf(lf, LFGC,  OFFC)
#define WLOGlf(lf)         printlf(lf, WFGC,  OFFC)
#define Rlf(lf)            RLOGlf(lf)
#define Rlfn(lf, n)        RLOGlfn(lf, n)
#define Rlfnf(lf, n, f)    RLOGlfnf(lf, n, f)
#define DRlf(lf)       DRLOGlf(lf)
#define Ylf(lf)            YLOGlf(lf)
#define Ylfn(lf, n)        YLOGlfn(lf, n)
#define Ylfnf(lf, n, f)    YLOGlfnf(lf, n, f)
#define DYlf(lf)       DYLOGlf(lf)
#define Glf(lf)        GLOGlf(lf)
#define Glfn(lf, n)    GLOGlfn(lf, n)
#define DGlf(lf)       DGLOGlf(lf)
#define Clf(lf)            CLOGlf(lf)
#define Clfn(lf, n)        CLOGlfn(lf, n)
#define Clfnf(lf, n, f)    CLOGlfnf(lf, n, f)
#define DClf(lf)       DCLOGlf(lf)
#define Blf(lf)        BLOGlf(lf)
#define DBlf(lf)       DBLOGlf(lf)
#define Mlf(lf)        MLOGlf(lf)
#define DMlf(lf)       DMLOGlf(lf)
#define Klf(lf)        KLOGlf(lf)
#define Dlf(lf)        DLOGlf(lf)
#define Llf(lf)        LLOGlf(lf)
#define Wlf(lf)        WLOGlf(lf)

// ----------------------------------------------- LF UTILITY FUNCTIONS -------

LF lfscaledown(LF value, IN digits) {
  IF (totaldigits(value.frac) GT 18) {
    Rs("lfscaledown fracvalue toobig");
    Yl(value.frac);
  }
  IN fracscaledigits = scaledigits(value.frac);
  IN fractotaldigits = totaldigits(value.frac);
  IN fracactivedigits = fractotaldigits - fracscaledigits;
  LN scale = lnpow(10, fracactivedigits); // 0.999 has scale 1000
  WI (DEC digits GQ 0) {
    LN digit = value.numb % 10LL;
    IF (digit EQ 0LL) { // this is a 0 ---------
      IF (value.frac EQ 0) {
        value.numb DIVS 10; // 100.0 -> 10.00 == 10.0 (eat the 0)
      } EF (totaldigits(value.frac) GT 18) {
        Rs("scaledownoverflow");
        Yl(value.frac);
        RT LFOVERFLOW;
        // value.frac DIVS 10;
      } EL { // EF (totaldigits(value.frac) EQ 18) {
        LN zeroscale = 10LL; // start with 1 scale digit (the one to add)
        WI (value.frac % 10LL EQ 0) { // eat scale digits
          IF (value.frac EQ 0) { BK; } // prevent infinite loops
          value.frac DIVS 10; // eat digit
          zeroscale MULS 10; // increase scale
        }
        IN newwidth = totaldigits(value.frac) + totaldigits(zeroscale) - 1;
        WI (newwidth GT LFDIGITLIMIT) {
          IF (value.frac EQ 0) { Rs("MASSIVEZEROSCALE"); BK; } // prevent infloops
          // if adding scale digits on makes the number too wide... need to
          // eat low-end digits until it fits.... as above .......
          value.frac DIVS 10; // eat a fraction digit (make room for the 0s)
          scale DIVS 10; // remove digit .... readd after calculation ........
          // actually won't readd because replacement digit is a 0 - not part of activedigits
          WI (value.frac % 10 EQ 0) { // eat valueless 0s from bottom.. eg 0.9(00)9
            IF (value.frac EQ 0) { BK; } // prevent infinite loops
            value.frac DIVS 10; // eat 0 digit
            scale DIVS 10; // one less digit in fraction
          }
          newwidth = totaldigits(value.frac) + totaldigits(zeroscale) - 1;
        }
        value.frac MULS zeroscale; // put 0s back on + 1 more
        value.numb DIVS 10; // eat the number digit (which was 0)
        // scal is not increased because zero digits are scale, not active digits
      }
    } EL { // this is not a 0 -----------------
      // before we add this number we need to check if any other 0s are about
      // 0s on foot need to be eaten and added to scale
      // but if the new number digit from above causes width 19 .......
      // truncate bottom digit ... frac DIVS 10 .....
      // but if bottom digit is a 0... need to eat 0s ... then eat bottom digit ...
      // then put top digit in but also with the 0s removed from the bottom....
      // 3, 123456780 = 3.012345678.. -> 0, 301234567 (0 at top, 8 truncated)
      IF (totaldigits(value.frac) GT LFDIGITLIMIT) {
        Rs("scaledownoverflowTWO");
        Wl(value.frac);
        RT LFOVERFLOW;
      } EL { // same proceeeesssss ..... F (totaldigits(value.frac) EQ 18) {
        LN zeroscale = 1LL; // start with no scale digits, * scale later
        WI (value.frac % 10 EQ 0) {
          IF (value.frac EQ 0) { BK; } // prevent infinite loops
          zeroscale MULS 10; // increase scale
          value.frac DIVS 10; // eat scaling (non-active) digit
        }
        IF (totaldigits(value.frac) EQ LFDIGITLIMIT) {
          value.frac DIVS 10; // eat lowest digit (after scale digits removed)
          scale DIVS 10; // compensate for removed digit
          // removing one digit might remove more than one e.g. 0.00099000009 removes 6
          // if there are any 0s at the bottom of frac.. they have no value
          WI (value.frac % 10 EQ 0) { // eat 0s from bottom.. eg 0.9(00)9
            IF (value.frac EQ 0) { BK; } // prevent infinite loops
            value.frac DIVS 10; // eat non-scaling (active) 0 digit
            scale DIVS 10; // one less digit in fraction
          }
        }
        // new digit scale could still push to above max width
        IN adddigitwidth = totaldigits(scale) + scaledigits(zeroscale);
        WI (adddigitwidth GT LFDIGITLIMIT) { // 18 should be fine .... !
          value.frac DIVS 10;      // eat lowest digit
          scale DIVS 10;           // lower scale to compensate
          // if there are 0s at the bottom... they now have no value.......
          WI (value.frac % 10 EQ 0) { // eat 0s from bottom.. eg 0.9(00)9
            IF (value.frac EQ 0) { BK; } // prevent infinite loops
            value.frac DIVS 10; // eat 0 digit
            scale DIVS 10; // one less digit in fraction
          }
          adddigitwidth = totaldigits(scale) + scaledigits(zeroscale);
        }
        LN adddigitscale = scale * zeroscale;
        LN adddigit = digit * adddigitscale;
        value.frac ADDS adddigit;
        scale MULS zeroscale;    // the zeros
        scale MULS 10;           // and the new digit
        value.numb DIVS 10;
      }
    }
  }
  RT value;
}
LF lfscaleup(LF value, IN digits) {
Rs("NOTIMPLEMENTED");
}

// ----------------------------------------------- LF ADDITION FUNCTION -------

LF lfadd(LF one, LF two) {
  LF three = { one.numb + two.numb, 0 };
  IF (one.frac EQ 0) { three.frac = two.frac; RT three; } // adding 0 eats padding
  IF (two.frac EQ 0) { three.frac = one.frac; RT three; } // easier to just skip
  LN f1value = one.frac * ((one.numb LT 0 AND one.frac GT 0) ? -1LL : 1LL);
  LN f2value = two.frac * ((two.numb LT 0 AND two.frac GT 0) ? -1LL : 1LL);
  IN f1totaldigits = totaldigits(f1value);
  IN f2totaldigits = totaldigits(f2value);
  IN f1scaledigits = scaledigits(f1value);
  IN f2scaledigits = scaledigits(f2value);
  IN abscale = MIN2(f1scaledigits, f2scaledigits);
  // -5555550 + 99900000 = 9434450 - 100000000 = -5655550
  IN f12scaledigits = abscale;
  WI (f12scaledigits GT 0) { // remove shared scale digits
    f1value DIVS 10;
    f2value DIVS 10;
    DEC f12scaledigits;
  }
  IN f1activedigits = f1totaldigits - f1scaledigits;
  IN f2activedigits = f2totaldigits - f2scaledigits;
  IN f2activeoffset = f1activedigits - f2activedigits;
  IN f2scaleoffset = 2 * (f1scaledigits - f2scaledigits);
  IN f2totaloffset = f2activeoffset + f2scaleoffset;
  IF (f2totaloffset LT 0) { // negative - offset A, truncate B
    WI (f2totaloffset LT 0) {
      IF (totaldigits(f1value) GT 18) {
        Rs("lfaddf1overflow");
        RT LFOVERFLOW;
      } EF (totaldigits(f1value) EQ 18) {
        f2value DIVS 10;
      } EL {
        f1value MULS 10;
      }
      INC f2totaloffset;
    }
  } EF (f2totaloffset GT 0) { // positive - offset B, truncate A
    WI (f2totaloffset GT 0) {
      IF (totaldigits(f2value) GT 18) {
        Rs("lfaddf2overflow");
        RT LFOVERFLOW;
      } EF (totaldigits(f2value) EQ 18) {
        f1value DIVS 10;
      } EL {
        f2value MULS 10;
      }
      DEC f2totaloffset;
    }
  } // <-- 0.0555555 - 4.999 to 555555(0) 9990000(0)
  // exscale removed ... check .outdated if it turns out to be relevant
  IN f12width = MAX2(totaldigits(f1value), totaldigits(f2value));
  IN f3overflow = 0;
  LN f3value = f1value + f2value; // default, including if either is 0
  IF (f1value LT 0 AND f2value LT 0) { // if both negative
    f3value = -((-f1value) + (-f2value)); // add, return negative
  } EF (f1value LT 0 AND f2value GT 0) { // if first negative
    f3value = -((-f1value) - f2value); // subtract, return negative
    IF (f3value GT 0 AND three.numb LT 0) { // if sign flips and A < 0 (-> 0)
      IF (f3value % 10 NQ 0 AND abscale LQ 0) // overflow only if unpadded
        { INC f3overflow; f3value SUBS lnpow(10, totaldigits(f3value)); }
    }
  } EF (f1value GT 0 AND f2value LT 0) { // if second negative
    f3value = (f1value - (-f2value)); // subtract (a - b), return positive
    IF (f3value LT 0 AND three.numb GT 0) { // if sign flips and A > 0 (0 <-)
      IF (f3value % 10 NQ 0 AND abscale LQ 0) // overflow only if unpadded
        { DEC f3overflow; f3value ADDS lnpow(10, totaldigits(f3value)); }
    }
  } // EL both are positive, use default
  IN f3width = totaldigits(f3value);
  IN overflowdigits = f3width - f12width;
  IN lessdigits = f12width - f3width;
  IN addscale = (f3width - 1) - totaldigits(f3value);
  IF (overflowdigits EQ 1) {
    IF (abscale LQ 0) {
      LN subval = lnpow(10, f12width);
      IF (f3value LT 0) {
        f3value ADDS subval;
        DEC f3overflow;
      } EL {
        f3value SUBS lnpow(10, f3width - 1); // 1977664 - 1000000 (10^f12width)
        INC f3overflow;
      }
      IN addscale = (f3width - 1) - totaldigits(f3value);
      abscale ADDS addscale; // in both cases ........
    } EL { DEC abscale; }
  }
  IF (lessdigits GT 0)
    { abscale ADDS lessdigits; }
  // trim excess lowside 0s
  WI (f3value NQ 0 AND f3value % 10 EQ 0)
    { f3value DIVS 10; }
  // re-add AB scale zeros
  WI (abscale GT 0)
    { f3value MULS 10; DEC abscale; }
  three.frac = f3value;
  IF (f3overflow)
    { three.numb ADDS f3overflow; }
  IF (three.numb GT 0 AND three.frac LT 0)
    { three.numb MULS -1; three.frac MULS -1; } // 1.-245 to -1.245
  EF (three.numb LT 0 AND three.frac LT 0)
    { three.frac MULS -1; } // -1.-245 to -1.245
  RT three;
}
#define lfADD(a, b)  lfadd(a, b)
#define LFADD(a, b) (lfADD(a, b))
// ----------------------------------------------- LF SUBTRACTION FUNCTION ----
LF lfsubtract(LF one, LF two)
  { RT lfADD(one, lfinvert(two)); }
#define lfSUB(a, b)  lfsubtract(a, b)
#define LFSUB(a, b) (lfSUB(a, b))
// ----------------------------------------------- LF COMPARISON FUNCTIONS ----
CH lflessthan(LF va, LF vb)           // <
  { RT lfispositive(lfSUB(vb, va)); }
#define lfLT(a, b)  lflessthan(a, b)
#define LFLT(a, b)  (lfLT(a, b))
// -----------------------------------------------
CH lfmorethan(LF va, LF vb)           // >
  { RT lfispositive(lfSUB(va, vb)); }
#define lfGT(a, b)  lfmorethan(a, b)
#define LFGT(a, b)  (lfGT(a, b))
// -----------------------------------------------
CH lfequal(LF va, LF vb)              // ==
  { RT lfiszero(lfSUB(va, vb)); }
#define lfEQ(a, b)  lfequal(a, b)
#define LFEQ(a, b)  (lfEQ(a, b))
// -----------------------------------------------
CH lfnotequal(LF va, LF vb)
  { RT lfnotzero(lfSUB(va, vb)); }    // !=
#define lfNQ(a, b)  lfnotequal(a, b)
#define LFNQ(a, b)  (lfNQ(a, b))
// -----------------------------------------------
CH lflessthanorequal(LF va, LF vb)    // <=
  { RT lfisunsigned(lfSUB(vb, va)); }
#define lfLQ(a, b)  lflessthanorequal(a, b)
#define LFLQ(a, b)  (lfLQ(a, b))
// -----------------------------------------------
CH lfmorethanorequal(LF va, LF vb)    // >=
  { RT lfisunsigned(lfSUB(va, vb)); }
#define lfGQ(a, b)  lfmorethanorequal(a, b)
#define LFGQ(a, b)  (lfGQ(a, b))
// ------------------------------------------ LF MULTIPLICATION FUNCTION -- * -
// LFMFRACLIMIT defined above for lfscaledown .. could move back down
LF lfmultiplyfraction(LN onen, LN twof) {
  LF result = { 0, 0 };
  IN onedigits = totaldigits(onen);
  IF (onedigits GT LFDIGITLIMIT)
    { Rs("onen toobig"); RT LFOVERFLOW; }
  IN twodigits = totaldigits(twof);
  IF (twodigits GT LFDIGITLIMIT)
    { Rs("twof toobig"); RT LFOVERFLOW; }
  IN onedown = 0;                      // count how many digits are cut off (0, 1 or toobig error)
  WI (onedigits GT LFMFRACLIMIT) {     // multiply left by each digit of right
    onen DIVS 10;                      // should truncate 1 digit or warn above if more
    DEC onedigits; INC onedown;
  } // result digits = left + right (each fits)
  LN twofvalue = unscale(twof);      // .0012 becomes .12 -> result / 100 to balance
  IN twofscale = scaledigits(twof) - onedown; // 2 scale digits for lfscaledown, -1 for 1->0.1
  IN twoactive = totaldigits(twofvalue); // .12 (from .0012) has 2 active digits
  IN digitscale = twoactive;         // count down from max to 1 (how far to shift)
  WI (digitscale GT 0) { // shift by ... 4 3 2 1
    LN nextdigit = twofvalue % 10;
    twofvalue DIVS 10; // should hit 0 when digitscale hits 0
    LF nextresult = { onen * nextdigit, 0 };
    nextresult = lfscaledown(nextresult, digitscale + twofscale);
    DEC digitscale;
    result = lfadd(result, nextresult);
  }
  RT result;
}
// -----------------------------------------------
LF lfmultiplyfractions(LN onef, LN twof) {
  IN onescale = scaledigits(onef);
  LN onen = unscale(onef); // 0.000123045 is 123045 000 (6+3)  
  IN onedigits = totaldigits(onen);
  LF fracmul = lfmultiplyfraction(onen, twof);
  LF result = lfscaledown(fracmul, onedigits + onescale); // totaldigits ?
  RT result;
}
// -----------------------------------------------
LF lfmultiply(LF one, LF two) {
  CH isnegative = lfisnegative(one) ^ lfisnegative(two);
  LF posone = lfpositive(one);
  LF postwo = lfpositive(two); // 100*100 = 10000 999*999 = 998001
  IN onendigits = totaldigits(posone.numb);
  IN twondigits = totaldigits(postwo.numb);
  IF (onendigits + twondigits GT LFDIGITLIMIT)
    { Rs("(!*)"); RT LFOVERFLOW; }
  LF nn = { posone.numb * postwo.numb, 0 };
  LF nf = lfmultiplyfraction(posone.numb, postwo.frac);
  LF fn = lfmultiplyfraction(postwo.numb, posone.frac);
  LF ff = lfmultiplyfractions(posone.frac, postwo.frac);
  LF nnff = lfadd(nn, ff);
  LF nffn = lfadd(nf, fn);
  LF result = lfadd(nnff, nffn);
  IF (isnegative)
    { result = lfnegative(result); }
  RT result;
}
#define lfMUL(a, b)  lfmultiply(a, b)
#define LFMUL(a, b)  (lfMUL(a, b))
// --------------------------------------------- LF DIVISION FUNCTION ----- / -
LF lfdivide(LF one, LF two) {
  CH isnegative = lfisnegative(one) ^ lfisnegative(two);
  LF posone = lfpositive(one);
  LF postwo = lfpositive(two);
  LF lften = { 10, 0 };
  LF lftenth = { 0, 1 };
  IN twoscale = 1;
  WI (postwo.frac GT 0) {
    postwo = lfMUL(postwo, lften);
    twoscale MULS 10;
  }
  LF onescale = { twoscale, 0 };
  posone = lfMUL(posone, onescale);
  LF result = { 0, 0 }; // numb added in first iteration // { posone.numb / postwo.numb, 0 };
  LN remainder = posone.numb - (result.numb * postwo.numb);
  LF fracscale = { 0, 1 };
  WI (remainder NQ 0LL AND totaldigits(result.frac) LQ LFDIGITLIMIT) { // presumably always positive ...
    posone = lfMUL(posone, lften);
    LF nextdigit = { posone.numb / postwo.numb, 0 };
    LF fracdigit = lfMUL(nextdigit, fracscale);
    IF (fracdigit.frac LT 0) { eRs("*FDOF"); BK; }  // frac digit overflow
    LF nextresult = lfADD(result, fracdigit);
    IF (nextresult.frac LT 0) { eMs("*NROF"); BK; } // next result overflow
    result = nextresult;
    fracscale = lfMUL(fracscale, lftenth);
    IF (fracscale.frac LQ 0) { eYs("*FSOF"); BK; }  // frac scale overflow
    LF digitvalue = { nextdigit.numb * postwo.numb, 0 };
    posone = lfSUB(posone, digitvalue);  
    remainder = posone.numb - (result.numb * postwo.numb);
  }
  IF (isnegative)
    { result = lfnegative(result); }
  RT result;
}
#define lfDIV(a, b)  lfdivide(a, b)
#define LFDIV(a, b)  (lfDIV(a, b))
// ----------------------------------------------- LF MODULUS FUNCTION ---- % -
LF lfmodulus(LF one, LF two) {
  LF result = one;
  WI (lfGT(result, two))
    { result = lfSUB(result, two); }
  RT result;
}
#define lfMOD(a, b)  lfmodulus(a, b)
#define LFMOD(a, b)  (lfMOD(a, b))
// -------------------------------- LF INCREMENT/DECREMENT FUNCTIONS -- ++/-- -
#define LFINC(a)     (lfadd(a, (LF){ 1, 0 }))
#define LFDEC(a)     (lfsubtract(a, (LF){ 1, 0 }))
// ^ cannot use macro lfADD due to , in definition
// ----------------------------------------------- LF READ FUNCTIONS ----------

// lfreadfslf regards EOF as a broken packet
// lf read fs's lf (read lf from fs)
LF lfreadfslf(FS fs, INP nextch) { // sets char after lf match;
  INP ch = nextch;
  IN nch = NUL;
  IF (ch EQNULL) { ch = &nch; }
  // nextch pointer should contain NUL at start
  // if not, it will be used as the first character
  // if NULL given, internal pointer will be used
  IF (*ch EQ NUL)
    { *ch = GETFCH(fs); } // get first char if none given
  LF lf = { 0, 0 };
  CH neg = An;
  IFEQEOF(*ch) { RT (LF){ 0, 0 }; }
  EFEQ2(*ch, Asemicolon, Anewline) { RT lf; } // nothing           -> 0
  EFEQ(*ch, Adash) { neg = Ay; } // continue below
  EF INRANGE(*ch, A0, A9) // have to process first char
    { lf.numb = (lf.numb * 10) + (*ch - A0); }
  WINQ3(*ch, Asemicolon, Anewline, Adot) {
    *ch = GETFCH(fs);
    IFEQEOF(*ch) { RT (LF){ 0, 0 }; }
    EFEQ2(*ch, Asemicolon, Anewline)
      { IFEQ(neg, Ay) { lf.numb = 0 - lf.numb; } RT lf; } // -123   ->
    EFEQ(*ch, Adot) { BK; } // decimal part
    EF INRANGE(*ch, A0, A9)
      { lf.numb = (lf.numb * 10) + (*ch - A0); }
    EL { BK; } // (break on bad char.. no mantissa will be noticed)
  }
  LN scale = 1;
  LN pad = 1;
  IFEQ(*ch, Adot) {
    *ch = GETFCH(fs);   // ...surely needed
    IFEQ2(*ch, Asemicolon, Anewline)
      { IFEQ(neg, Ay) { lf.numb = 0 - lf.numb; } RT lf; } // -123.   ->
    WIEQ(*ch, A0)
      { scale MULS 10; *ch = GETFCH(fs); }
    IFEQ2(*ch, Asemicolon, Anewline)
      { IFEQ(neg, Ay) { lf.numb = 0 - lf.numb; } RT lf; } // -123.0000   ->
    IN fracdigits = 0;
    WI (INRANGE(*ch, A0, A9) AND (INC fracdigits LQ 18)) { // fracdigits + scaledigits LQ 17 maybe
      IF (*ch NQ A0) {
        lf.frac = (lf.frac * pad * 10) + (*ch - A0);
        pad = 1;
      } EL { pad MULS 10; }
      *ch = GETFCH(fs);
    }
    lf.frac MULS scale;
  } // *ch will be ; \n or the first invalid char
  IFEQ(neg, Ay) {
    IFEQ(lf.numb, 0) { lf.frac MULS -1; }
    EL { lf.numb MULS -1; }
  }
  RT lf; // -123,3450000 (-123.0000345) or 0,-3450000 (-0.0000345)
}  

// lf read fs's cs (read a cs from fs)
CS lfreadfscs(FS fs, CS str, INP nextch) {
  INP ch = nextch;
  IN nch = NUL;
  IF (ch EQNULL) { ch = &nch; }
  IF (!str) { RT NULL; } // cannot write
  WINQ3(*ch, Asemicolon, Anewline, NUL) {
    *ch = GETFCH(fs);
    IFEQEOF(*ch) { RT ""; }
    EFEQ3(*ch, Asemicolon, Anewline, NUL) { BK; }
    EL { *str = *ch; INC str; *str = NUL; }
  }
  RT str;
}

// lf read cs's lf (read an lf from cs)
LF lfreadcslf(CS *strp) {
  CS str = *strp;
  LF lf = { 0, 0 };
  CH neg = An;
  IF (EL0(str) EQ Adash)
    { neg = Ay; NEXT1(str); }
  WI (EL0IRA09(str)) {
    lf.numb = (lf.numb * 10) + (str[0] - A0);
    NEXT1(str);
  }
  LN scale = 1;
  LN pad = 1;
  IF (EL0(str) EQ Adot) {
    NEXT1(str);
    // consider allowing - after . or after .00 (neg fraction)
    WI (EL0(str) EQ A0)
      { scale MULS 10; NEXT1(str); eMs("S"); }
//Gl(scale);
//Yi(pad);
    IN fracdigits = 0;
    WI (EL0IRA09(str) AND (INC fracdigits LQ 18)) {
//Mc(EL0(str));
//Yi(fracdigits);
      IF (EL0(str) NQ A0) {
//Wl(lf.frac);
//Bl(pad);
//Cl(pad * 10);
//Mc(str[0]);
        lf.frac = (lf.frac * pad * 10) + (str[0] - A0);
//Wl(lf.frac);
        pad = 1; // only eats 0s with numbers following
      } EL { pad MULS 10; } // trailing padding discarded
      NEXT1(str);
    }
//Yl(lf.frac);
//Rl(scale);
    lf.frac MULS scale;
//Clf(lf);
  }
  IF (neg EQ Ay) {
    IF (lf.numb EQ 0) {
      lf.frac MULS -1;
    } EL { lf.numb MULS -1; }
//  IF (three.numb GT 0 AND three.frac LT 0)
//    { three.numb MULS -1; three.frac MULS -1; } // 1.-245 to -1.245
//  EF (three.numb LT 0 AND three.frac LT 0)
//    { three.frac MULS -1; } // -1.-245 to -1.245
//  IF (lf.numb EQ 0 AND neg EQ Ay)
//    { lf.frac MULS -1; } // no such thing as -0, store -0.009 as 0.-900
  }
  *strp = str;
  RT lf;
}

VD sprintlf(CS str, LF lf, CS prefix, CS suffix) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  sprintf(str, "%s%s%lld.%s%lld%s", prefix, neg, numval, fracpad, fracval, suffix);
}
/* defined earlier
VD printlf(LF lf, CS prefix, CS suffix) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  IF (totaldigits(numval) GT 18) {
    // overflow numbers will be red
    printf("%s%s%lld.%s%lld%s", RFGC, neg, numval, fracpad, fracval, OFFC);
  } EL {
    // presumed valid number gets normal formatting
    printf("%s%s%lld.%s%lld%s", prefix, neg, numval, fracpad, fracval, suffix);
  }
}
*/
VD fprintlf(FS file, LF lf, CS prefix, CS suffix) {
  CS neg = (lf.numb EQ 0 AND lf.frac LT 0) ? "-" : "";
  IN inv = (EL0(neg) EQ '-') ? -1 : 1;
  LN numval = lf.numb;
  CS fracpad = scalestring(lf.frac);
  LN fracval = unscale(lf.frac * inv);
  fprintf(file, "%s%s%lld.%s%lld%s", prefix, neg, numval, fracpad, fracval, suffix);
}
#define FPUTLF        fprintlf
//#define PUTLF         printlf
#define STRLF         sprintlf
/* redefined earlier
#define LOGlf(lf)     printlf(lf, "", "")
#define RLOGlf(lf)    printlf(lf, RFGC,  OFFC)
#define DRLOGlf(lf)   printlf(lf, DRFGC, OFFC)
#define YLOGlf(lf)    printlf(lf, YFGC,  OFFC)
#define DYLOGlf(lf)   printlf(lf, DYFGC, OFFC)
#define GLOGlf(lf)    printlf(lf, GFGC,  OFFC)
#define DGLOGlf(lf)   printlf(lf, DGFGC, OFFC)
#define CLOGlf(lf)    printlf(lf, CFGC,  OFFC)
#define DCLOGlf(lf)   printlf(lf, DCFGC, OFFC)
#define BLOGlf(lf)    printlf(lf, BFGC,  OFFC)
#define DBLOGlf(lf)   printlf(lf, DBFGC, OFFC)
#define MLOGlf(lf)    printlf(lf, MFGC,  OFFC)
#define DMLOGlf(lf)   printlf(lf, DMFGC, OFFC)
#define KLOGlf(lf)    printlf(lf, KFGC,  OFFC)
#define DLOGlf(lf)    printlf(lf, DFGC,  OFFC)
#define LLOGlf(lf)    printlf(lf, LFGC,  OFFC)
#define WLOGlf(lf)    printlf(lf, WFGC,  OFFC)
#define Rlf(lf)       RLOGlf(lf)
#define DRlf(lf)      DRLOGlf(lf)
#define Ylf(lf)       YLOGlf(lf)
#define DYlf(lf)      DYLOGlf(lf)
#define Glf(lf)       GLOGlf(lf)
#define DGlf(lf)      DGLOGlf(lf)
#define Clf(lf)       CLOGlf(lf)
#define DClf(lf)      DCLOGlf(lf)
#define Blf(lf)       BLOGlf(lf)
#define DBlf(lf)      DBLOGlf(lf)
#define Mlf(lf)       MLOGlf(lf)
#define DMlf(lf)      DMLOGlf(lf)
#define Klf(lf)       KLOGlf(lf)
#define Dlf(lf)       DLOGlf(lf)
#define Llf(lf)       LLOGlf(lf)
#define Wlf(lf)       WLOGlf(lf)
*/

typedef struct _LFList {
  LF value;
  CS csvalue;
  struct _LFList *next;
} LFList;

typedef struct _Parameter {
  CS name;
  LFList *values;
  struct _Parameter *subvalues;
  struct _Parameter *next;
} Parameter;


LFList *lfaddvalue(LFList *values, LF value, CS csvalue) {
  IF (!values) {
    values = (LFList *)MEM(sizeof(LFList));
    values->value = value;
    values->csvalue = csvalue;
    values->next = NULL;
  } EL {
    LFList *v = values;
    WI (v->next) { v = v->next; }
    v->next = (LFList *)MEM(sizeof(LFList));
    v->next->value = value;
    v->next->csvalue = csvalue;
    v->next->next = NULL;
  }
  RT values; // return pointer to list
}

LF lfgetlastlfvalue(LFList *values) {
  LFList *v = values;
  IF (!v) { RT (LF){ 0, 0 }; }
  WI (v->next) {
    v = v->next;
  }
  RT v->value;
}

CS lfgetlastcsvalue(LFList *values) {
  LFList *v = values;
  IF (!v) { RT ""; } // (LF){ 0, 0 }; }
  WI (v->next) {
    v = v->next;
  }
  RT v->csvalue;
}

LF lfvaluesmin(LFList *values) {
  LF min = { 99999999999999, 99999999999999 };
  LFList *v = values;
  IF (v->csvalue[0] EQ '~' AND !v->next) {
    min = v->value; // no values, use default ~ at 0
    RT min;
  } EL {
    WI (v) {
//Wlf(v->value);
//Cc(','); // Cc ,
//Ms(v->csvalue);
      IF (v->csvalue AND v->csvalue[0] EQ '~')
        { v = v->next; CT; } // skip ~ values
      IF LFLT(v->value, min)
        { min = v->value; }
      v = v->next;
    }
  }
M2("MIN: %lld.%lld", min.numb, min.frac);
  RT min; // returns ultra-positive if no list values
}
LF lfvaluesmax(LFList *values) {
  LF max = { -99999999999999, 99999999999999 };
//Clf(max);
  LFList *v = values;
  IF (v->csvalue[0] EQ '~' AND !v->next) {
    max = v->value; // no values, use default ~ at 0
    RT max;
  } EL {
    WI (v) {
//Wlf(v->value);
//Cc(','); // Cc ,
//Ms(v->csvalue);
      IF (v->csvalue AND v->csvalue[0] EQ '~')
        { v = v->next; CT; } // skip ~ values
      IF LFGT(v->value, max)
        { max = v->value; }
      v = v->next;
    }
  }
M2("MAX: %lld.%lld", max.numb, max.frac);
  RT max; // returns ultra-negative if no values available
}
LF lfvaluesavg(LFList *values) {
  LF avg = { 0, 0 };
  LF count = { 0, 0 };
  LFList *v = values;
  WI (v) {
    avg = LFADD(avg, v->value);
    count = LFINC(count);
    v = v->next;
  }
  RT LFDIV(avg, count);
}
LF lfvaluespan(LFList *values) {
  //if no values, neg - pos = double neg overflow .. 
  RT LFSUB(lfvaluesmax(values), lfvaluesmin(values));
}
IN lfvaluescount(LFList *values) {
  IN count = 0;
  LFList *v = values;
  WI (v) {
    INC count;
    v = v->next;
  }
  RT count;
}
LF lfvalueatindex(LFList *values, IN index) {
  IN count = 0;
  LFList *v = values;
  WI (v) {
    IF (index EQ count)
      { RT v->value; }
    INC count;
    v = v->next;
  }
  RT LFOUTOFRANGE;
}
VD lflistvalues(LFList *values) {
  LFList *v = values;
  WI (v) {
    IF (v NQ values)
      { Mc(','); }
    IF (v->csvalue) {
      Ys(v->csvalue);
    }
    Glf(v->value);
    v = v->next;
  }
}

VD lflistparams(Parameter *params) {
  Wc('[');
  Parameter *p = params;
  WI (p) {
    Cs(p->name);
    IF (p->subvalues)
      { lflistparams(p->subvalues); }
    Wc('=');
    lflistvalues(p->values);
    IF (p->next) {
      p = p->next;
      Wc(',');
    } EL { BK; }
  }
  Wc(']');
}

LFList *lfgetparam(Parameter *params, CS pname) {
  IF (!params) {
    RT NULL;
  } EL {
    CS delim = pname;
    WINQ2(*delim, Adot, NUL)
      { INC delim; } // find .
    IFEQ(*delim, Adot) {
      Parameter *p = params;
      WI (p) {
        IN mi = 0;
        WI (p->name[mi] EQ pname[mi]) { INC mi; }
        IF (p->name[mi] EQNUL AND pname[mi] EQ Adot)
          { BK; } EL { p = p->next; }
      }
      IF (p) { // NAME in NAME.X found
        RT lfgetparam(p->subvalues, delim + 1);
      } EL { RT NULL; } // not found
    } EL { // no dots, this is where the value is
      Parameter *p = params;
      WI (p) {
        IF STREQ(p->name, pname) { BK; }
        p = p->next;
      }
      IF (p) { // X found
        RT p->values; // lfgetlastlfvalue(p->values);
      } EL { RT NULL; } // not found
    }
  }
}

LF lfgetlfparam(Parameter *params, CS pname) {
  IF (!params) {
    RT (LF){ 0, 0 };
  } EL {
    CS delim = pname;
    WINQ2(*delim, Adot, NUL)
      { INC delim; } // find .
    IFEQ(*delim, Adot) {
      Parameter *p = params;
      WI (p) {
        IN mi = 0;
        WI (p->name[mi] EQ pname[mi]) { INC mi; }
        IF (p->name[mi] EQNUL AND pname[mi] EQ Adot)
          { BK; } EL { p = p->next; }
      }
      IF (p) { // NAME in NAME.X found
        RT lfgetlfparam(p->subvalues, delim + 1);
      } EL { RT (LF){ 0, 0 }; } // not found
    } EL { // no dots, this is where the value is
      Parameter *p = params;
      WI (p) {
        IF STREQ(p->name, pname) { BK; }
        p = p->next;
      }
      IF (p) { // X found
        RT lfgetlastlfvalue(p->values);
      } EL { RT (LF){ 0, 0 }; } // not found
    }
  }
}

CS lfgetcsparam(Parameter *params, CS pname) {
  IF (!params) {
    RT ""; // (LF){ 0, 0 };
  } EL {
    CS delim = pname;
    WINQ2(*delim, Adot, NUL)
      { INC delim; } // find .
    IFEQ(*delim, Adot) {
      Parameter *p = params;
      WI (p) {
        IN mi = 0; // cannot set static delim to NUL
        WI (p->name[mi] EQ pname[mi]) { INC mi; }
        IF (p->name[mi] EQNUL AND pname[mi] EQ Adot)
          { BK; } EL { p = p->next; } // match end .
      }
      IF (p) { // name in name.x found 
       RT lfgetcsparam(p->subvalues, delim + 1);
      } EL { // name in name.x not found
        RT ""; // (LF){ 0, 0 };
      }
    } EL { // no dots, this is where the value is
      Parameter *p = params;
      WI (p) {
        IF STREQ(p->name, pname) { BK; }
        p = p->next;
      }
      IF (p) { // x found
        RT lfgetlastcsvalue(p->values);
      } EL { // x not found
        RT ""; // (LF){ 0, 0 };
      }
    }
  }
}


// need to say c->parameters = addparam() -- returns address of list (of new list if NULL)
Parameter *lfaddnewparam(Parameter *params, CS pname, LF lfpvalue, CS cspvalue) {
  Parameter *param = NULL;
  IF (!params) {
    params = (Parameter *)MEM(sizeof(Parameter));
    param = params; // new first param
  } EL {
    // should check if it already exists? -- not necessary if addparam calls setparam
    // (cannot call addnewparam for MOUSE.Y if .X exists) -- call addparam/setparam
    Parameter *p = params; // params is presumed a something
    WI (p->next) { p = p->next; }
    p->next = (Parameter *)MEM(sizeof(Parameter));
    param = p->next; // new param at end of list
  }
  WI (pname[0] NQNUL) {
    CS delim = pname;
    WINQ2(*delim, Adot, NUL)
      { INC delim; } // find .
    IFEQ(*delim, Adot) {
      // can't with static strs *delim = NUL; // <- MOUSE.X becomes MOUSE
      CS pn = param->name = CHMEM((delim - pname) + 1); // includes NUL ...
      WI (*pname NQ Adot)
        { *pn = *pname; INC pn; INC pname; }
      *pn = NUL; // terminate new str at delim
      pname = delim + 1; // set to subvalue
      param->values = NULL; // MOUSE.X has no value in MOUSE
      param->subvalues = (Parameter *)MEM(sizeof(Parameter));
      param->next = NULL;
      param = param->subvalues;
      CT; // MOUSE.X creates MOUSE and re-loops with X as pname
    } EL {
      param->name = CHMEM((delim - pname) + 1); // no delims here
      STRF(param->name, "%s", pname); // this should be sans dots
      param->values = lfaddvalue(NULL, lfpvalue, cspvalue); // this is a new list
      param->subvalues = NULL;
      param->next = NULL;
      BK; // no (more) looping required
    }
  }
  RT params; // return pointer to start of the list
}

// need lfgetcsparam and lfgetlfparam
// consider lfgetcslfparam for lf to cs and lfgetlfcsparam for cs to lf

Parameter *lfsetparam(Parameter *params, CS pname, LF lfpvalue, CS cspvalue) {
  IF (!params) {
    RT lfaddnewparam(NULL, pname, lfpvalue, cspvalue);
  } EL {
    CS delim = pname;
    WINQ2(*delim, Adot, NUL) { INC delim; } // find .
    IFEQ(*delim, Adot) {
//      *delim = NUL; // <--------------------- can't for static strings
      Parameter *p = params;
      WI (p) {
        IN mi = 0;
        WI (p->name[mi] EQ pname[mi]) { INC mi; }
        IF (p->name[mi] EQNUL AND pname[mi] EQ Adot)
          { BK; } EL { p = p->next; }
      }
      IF (p) { // param found, recurse for subvalue name
        p->subvalues = lfsetparam(p->subvalues, delim + 1, lfpvalue, cspvalue); // may add
        RT params; // return parent list
      } EL { // param name not found
//        *delim = Adot; // restore full pname
        params = lfaddnewparam(params, pname, lfpvalue, cspvalue); // 
        RT params; // return starting pointer in all cases
      }
    } EL { // this is the full/final parameter name (the one with a value)
      Parameter *p = params;
      WI (p) {
        IF STREQ(p->name, pname) { BK; }
        p = p->next;
      }
      IF (p) { // name exists in list
        p->values = lfaddvalue(p->values, lfpvalue, cspvalue);
        RT params; // <---
      } EL { // name not found, add to list (could be sublist)
        params = lfaddnewparam(params, pname, lfpvalue, cspvalue);
        RT params; // <--- may be saving into p->subvalues
      } // parameter does not exist - value not set
    }
  }
}

Parameter *lfsetlfparam(Parameter *params, CS pname, LF lfpvalue) {
  // param should exist, but NULL should return safe values ({ 0, 0 } and "" preferably, maybe NULL)
  // do not carry over previous tag CS cspvalue = lfgetcsparam(params, pname);
  CS cspvalue = "+"; // new value added (non-"~")
  RT lfsetparam(params, pname, lfpvalue, cspvalue);
}

LF lfsetlfparamlf(Parameter *params, CS pname, LF lfpvalue) {
  // same as lfsetlfparam, but returns the LF value instead
  Parameter *p = lfsetlfparam(params, pname, lfpvalue);
  RT lfpvalue;
} // for cascading set x = y = lfsetparam()

Parameter *lfsetcsparam(Parameter *params, CS pname, CS cspvalue) {
//  LF lfpvalue = lfgetlfparam(params, pname);
  LF lfpvalue = { 0, 0 };
  RT lfsetparam(params, pname, lfpvalue, cspvalue);
}

Parameter *lfaddparam(Parameter *params, CS pname, LF lfpvalue, CS cspvalue) {
  // to add a parameter, call lfsetparam() first (in case of existing MOUSE.X in .Y eg)
  // lfsetparam will call lfaddnewparam if param or subparam does not exist
  RT lfsetparam(params, pname, lfpvalue, cspvalue);
}
