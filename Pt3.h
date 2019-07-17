
// send to: code.h
#define COUNTMEM(type, n)  (type *)malloc(sizeof(type) * n)
#define SIZEMEM(type, n)    COUNTMEM(type, n)


typedef struct _Pt3 {
  double x, y, z;
} Pt3;
#define PT3VAR(pv, xv, yv, zv) \
  pv.x = xv; pv.y = yv; pv.z = zv;
#define Pt3PT3VAR(pv, xv, yv, zv)  \
  Pt3 pv; PT3VAR(pv, xv, yv, zv);
double Pt3magnitude(Pt3 start, Pt3 stop) {
  double xdiff = stop.x - start.x;
  double ydiff = stop.y - start.y;
  double zdiff = stop.z - start.z;
  return sqrt(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
}
Pt3 Pt3yoffset(Pt3 point, double yoffset) {
  point.y += yoffset;
  return point;
}

typedef struct _Vector {
  Pt3 start;
  Pt3 stop;
  double length;
//  double angleXY, angleXZ, angleYZ;
} Vector;
#define VECTORVAR(vv, startpt, stoppt)  \
  vv.start = startpt; vv.stop = stoppt;  \
  vv.length = Pt3magnitude(vv.start, vv.stop);
#define VectorVECTORVAR(vv, startpt, stoppt)  \
  Vector vv; VECTORVAR(vv, startpt, stoppt);
Pt3 Pt3pctalongVector(Vector v, double offsetpct) {
  double xdist = (v.stop.x - v.start.x) * offsetpct;
  double ydist = (v.stop.y - v.start.y) * offsetpct;
  double zdist = (v.stop.z - v.start.z) * offsetpct;
  Pt3PT3VAR(newpt, v.start.x + xdist, v.start.y + ydist, v.start.z + zdist);
  return newpt;
}

Pt3 Pt3offsetalongVector(Vector v, double offsetval) {
  double offsetpct = offsetval / v.length;
  return Pt3pctalongVector(v, offsetpct);
}

double angleUsingLawOfCosines(double adjSide1, double adjSide2, double oppSide) {
  return RADTODEG * (acos((adjSide1 * adjSide1 + adjSide2 * adjSide2 -
                            oppSide * oppSide   ) / (2.0f * adjSide1 * adjSide2)));
}

double sideUsingLawOfCosines(double side1, double side2, double oppAngle) {
  return sqrt(side1 * side1 + side2 * side2 -
               2.0f * side1 * side2 * cos(DEGTORAD * oppAngle));
}


double hypUsingPythagoras(double opp, double adj) {
  return sqrt(opp * opp + adj * adj);
}

double oppUsingPythagoras(double hyp, double adj) {
  return sqrt(hyp * hyp - adj * adj);
}

double adjUsingPythagoras(double hyp, double opp) {
	return sqrt(hyp * hyp - opp * opp);
}

double angleSinOppHyp(double opp, double hyp) {
  return RADTODEG * asin(opp / hyp);
}

double angleCosAdjHyp(double adj, double hyp) {
  return RADTODEG * acos(adj / hyp);
}

double angleTanOppAdj(double opp, double adj) {
  return RADTODEG * atan(opp / adj);
}

double oppFromHyp(double hyp, double angle) {
  return hyp * sin(DEGTORAD * angle);
}

double adjFromHyp(double hyp, double angle) {
  return hyp * cos(DEGTORAD * angle);
}

double oppFromAdj(double adj, double angle) {
  return adj * tan(DEGTORAD * angle);
}

Pt3 Pt3angledoffsetalongVector(Vector vec, double riseangle, double offsetlen) {
  double distanceout = adjFromHyp(offsetlen, riseangle);
  double distanceup = oppFromHyp(offsetlen, riseangle);
  Pt3 newpoint = Pt3offsetalongVector(vec, distanceout);
  newpoint = Pt3yoffset(newpoint, -distanceup);
  return newpoint;
}










/*
Pt3 Pt3xrotatearoundref(Pt3 point, double yzangle, Pt3 offset) {
  double Yv = point.y - offset.y;
  double Zv = point.z - offset.z;
  double radangle = yzangle * DEGTORAD; // spin ...
  double Ynew = Yv * cos(radangle) - Zv * sin(radangle);
  double Znew = Yv * sin(radangle) + Zv * cos(radangle);
  Pt3PT3VAR(newpoint, point.x, Ynew + offset.y, Znew + offset.z);
  return newpoint;
}
*/
Pt3 Pt3xrotatearoundref(Pt3 point, double yzangle, Pt3 offset) {
	double dy = point.y - offset.y;
	double dz = point.z - offset.z;
	double veclength = sqrt(dy * dy + dz * dz);
	double oldangle = atan2(dz, dy) * RADTODEG - 90.0;
	double newangle = oldangle + yzangle; // INVERTED!!!
    Pt3 newpoint = point;
    newpoint.y = offset.y + (sin(newangle * DEGTORAD) * veclength);
	newpoint.z = offset.z - (cos(newangle * DEGTORAD) * veclength);
	return newpoint;
}

/*
Pt3 Pt3yrotatearoundref(Pt3 point, double xzangle, Pt3 offset) {
  double Xv = point.x - offset.x;
  double Zv = point.z - offset.z;
  double radangle = xzangle * DEGTORAD; // flip ...
  double Xnew = Zv * sin(radangle) + Xv * cos(radangle);
  double Znew = Zv * cos(radangle) - Xv * sin(radangle);
  Pt3PT3VAR(newpoint, Xnew + offset.x, point.y, Znew + offset.z);
  return newpoint;
}
*/
Pt3 Pt3yrotatearoundref(Pt3 point, double xzangle, Pt3 offset) {
	double dx = point.x - offset.x;
	double dz = point.z - offset.z;
	double veclength = sqrt(dx * dx + dz * dz);
	double oldangle = atan2(dz, dx) * RADTODEG + 90.0;
	double newangle = oldangle - xzangle; // INVERTED!!!!!
    Pt3 newpoint = point;
    newpoint.x = offset.x + (sin(newangle * DEGTORAD) * veclength);
	newpoint.z = offset.z - (cos(newangle * DEGTORAD) * veclength);
	return newpoint;
}
/*
Pt3 Pt3zrotatearoundref(Pt3 point, double xyangle, Pt3 offset) {
  double Xv = point.x - offset.x;
  double Yv = point.y - offset.y;
  double radangle = xyangle * DEGTORAD; // twist
  double Xnew = Xv * cos(radangle) - Yv * sin(radangle);
  double Ynew = Xv * sin(radangle) + Yv * cos(radangle);
  Pt3PT3VAR(newpoint, Xnew + offset.x, Ynew + offset.y, point.z);
  return newpoint;
}
*/
Pt3 Pt3zrotatearoundref(Pt3 point, double xyangle, Pt3 offset) {
	double dx = point.x - offset.x;
	double dy = point.y - offset.y;
	double veclength = sqrt(dx * dx + dy * dy);
	double oldangle = atan2(dy, dx) * RADTODEG - 90.0; // INVERTED Z!!!!!!!
	double newangle = oldangle - xyangle; // INVERTED!!!!!!!!!
    Pt3 newpoint = point;
    newpoint.x = offset.x + (sin(newangle * DEGTORAD) * veclength);
	newpoint.y = offset.y - (cos(newangle * DEGTORAD) * veclength);
	return newpoint;
}
/*
// Think the tip of a needle... you know the radius of it, return the tip location
static Point angledOffset(Point start, int length, float angle) {
	Point p;
	p.x = start.x + ROUND((sin(RAD(angle)) * length));
	p.y = start.y - ROUND((cos(RAD(angle)) * length));
	return p;
}

// Rotate a point around a reference point
static Point rotatePoint(Point p, Point ref, float angle) {
	// Find the current length and angle
	int dx = p.x - ref.x, dy = p.y - ref.y;
	int length = ROUND(Math::Sqrt(dx * dx + dy * dy));
	float originalAngle = (float)(Math::Atan2(dy, dx) / PI * 180.0 + 90.0);

	return angledOffset(ref, length, originalAngle + angle);
}
*/

Pt3 Pt3addvectordistance(Pt3 point, Vector vec) {
  point.x += vec.stop.x - vec.start.x;
  point.y += vec.stop.y - vec.start.y;
  point.z += vec.stop.z - vec.start.z;
  return point;
}
Pt3 Pt3addxzvectordistance(Pt3 point, Vector vec) {
  point.x += vec.stop.x - vec.start.x;
  point.z += vec.stop.z - vec.start.z;
  return point;
}

Pt3 Pt3addz(Pt3 point, double len) {
  point.z += len;
  return point;
}
Pt3 Pt3subz(Pt3 point, double len) {
  point.z -= len;
  return point;
}

Pt3 Pt3subvectordistance(Pt3 point, Vector vec) {
  point.x -= vec.stop.x - vec.start.x;
  point.y -= vec.stop.y - vec.start.y;
  point.z -= vec.stop.z - vec.start.z;
  return point;
}



/*
Pt3 Pt3yrotatearoundref(Pt3 point, double xzangle, Pt3 offset) {
	double dx = point.x - offset.x;
	double dz = point.z - offset.z;
	double veclength = sqrt(dx * dx + dz * dz);
	double oldangle = atan2(dz, dx) * RADTODEG + 90.0;
	double newangle = oldangle - xzangle; // INVERTED!!!!!
    Pt3 newpoint = point;
    newpoint.x = offset.x + (sin(newangle * DEGTORAD) * veclength);
	newpoint.z = offset.z - (cos(newangle * DEGTORAD) * veclength);
	return newpoint;
}
*/


Pt3 Pt3zrisealongxzref(Pt3 point, double riseangle, Pt3 origin) {
	double dx = point.x - origin.x;
	double dz = point.z - origin.z;
//	double xzveclength = sqrt(dx * dx + dz * dz);
	double xzangle = atan2(dz, dx) * RADTODEG; // + 90.0;
    Pt3 noxz = Pt3yrotatearoundref(point, -xzangle, origin);
	Pt3 newxy = Pt3zrotatearoundref(noxz, -riseangle, origin); // INVERTED
	Pt3 newxyz = Pt3yrotatearoundref(newxy, xzangle, origin);
	return newxyz; // offset maintained
}



Pt3 Pt3angledoffsetafterVector(Vector vec, double riseangle, double offsetlen) {
  // find rise angle ...........
  Pt3 newpoint = Pt3offsetalongVector(vec, vec.length + offsetlen);
  //Pt3 angledpoint = Pt3yrotatearoundxzref(newpoint, riseangle, vec.stop);
  // a z rise is a negative angle !!!!!!!!!!!!!!!!!!!! ?
//  printf("NEWPOINT: %f, %f, %f\n", newpoint.x, newpoint.y, newpoint.z);
//  printf("VECSTOP : %f, %f, %f\n", vec.stop.x, vec.stop.y, vec.stop.z);
//  printf("RISEANGLE: %f\n", riseangle);
  double actualangle = 180.0 - riseangle; // SEEMS TO WORK !
//  printf("ACTUALANGLE: %f\n", actualangle);
  Pt3 angledpoint = Pt3zrisealongxzref(newpoint, actualangle, vec.stop);
//  printf("ANGLEDPOINT: %f, %f, %f\n", angledpoint.x, angledpoint.y, angledpoint.z);
//  Pt3 angledpoint = Pt3yrotatearoundxzref(newpoint, riseangle, vec.stop);
//  double distanceout = adjFromHyp(offsetlen, riseangle);
//  double distanceup = oppFromHyp(offsetlen, riseangle);
//  Pt3 newpoint = Pt3offsetalongVector(vec, vec.length + distanceout);
//  newpoint = Pt3yoffset(newpoint, -distanceup);
//  return newpoint;
  return angledpoint;
}



// ========================================================================

typedef struct _Pt3Script {
  char *name;
  char *script;
  Pt3 *points;
} Pt3Script;
#define Pt3Script5(psv, scriptname, scriptstr, p0, p1, p2, p3, p4)  \
  Pt3Script psv; psv.name = scriptname; psv.points = COUNTMEM(Pt3, 5);                \
  psv.points[0] = p0; psv.points[1] = p1; psv.points[2] = p2;  \
  psv.points[3] = p3; psv.points[4] = p4; psv.script = scriptstr;
#define Pt3Script6(psv, scriptname, scriptstr, p0, p1, p2, p3, p4, p5)  \
  Pt3Script psv; psv.name = scriptname; psv.points = COUNTMEM(Pt3, 6);                \
  psv.points[0] = p0; psv.points[1] = p1; psv.points[2] = p2;  \
  psv.points[3] = p3; psv.points[4] = p4; psv.points[5] = p5;  \
  psv.script = scriptstr;
#define Pt3Script8(psv, scriptname, scriptstr, p0, p1, p2, p3, p4, p5, p6, p7)  \
  Pt3Script psv; psv.name = scriptname; psv.points = COUNTMEM(Pt3, 8);                \
  psv.points[0] = p0; psv.points[1] = p1; psv.points[2] = p2;  \
  psv.points[3] = p3; psv.points[4] = p4; psv.points[5] = p5;  \
  psv.points[6] = p6; psv.points[7] = p7; psv.script = scriptstr;
#define Pt3Scriptfreepoints(psv)  free(psv.points)
// Pt3Scriptupdate8 () ....

typedef struct _Pt3ScriptList {
  Pt3Script *script;
  _Pt3ScriptList *next;
} Pt3ScriptList;

Pt3Script *Pt3ScriptListlookupscriptname(Pt3ScriptList *ps, char *name) {
  // matches static name variables - does not check string content!
  Pt3ScriptList *p = ps;
  while (p) {
    Pt3Script *script = p->script;
    if (!script) { continue; }
    if (script->name == name)
      { return script; }
    p = p->next;
  }
  return NULL;
}

int Pt3Scriptnumpoints(Pt3Script *ps, int *numpoints) {
  int scriptlen = 0; // returns length of script string
  int thispointix = 0;
  int maxpointix = -1;
  while (1) { //(ps->script[scriptlen] != '\0') {
    char sc = ps->script[scriptlen];
    if (sc >= '0' && sc <= '9') {
      thispointix = (thispointix * 10) + (sc - '0');
    } else { // all other chars end number ... if (sc == ',' || sc == ';' || sc == '\0') {
      if (thispointix > maxpointix) { maxpointix = thispointix; }
      thispointix = 0;
    }
    if (sc == '\0') { break; } // break at end of script
    else { scriptlen++; }      // else keep counting chars
  } // numpoints (max + 1) is counted by reading the script
//    if (thispointix > maxpointix) { maxpointix = thispointix; }
  *numpoints = maxpointix + 1;
  return scriptlen;
}

Pt3ScriptList *copyallocPt3ScriptList(Pt3ScriptList psl) {
  int numitems = 0;
  Pt3ScriptList *p = &psl;
  while (p) { numitems++; p = p->next; }
  Pt3ScriptList *newlist = COUNTMEM(Pt3ScriptList, numitems);
  int newli = -1;
  p = &psl; // restart iterator
  while (p) {
    ++newli; // index of new scriptlist item
    Pt3Script *ps = p->script;
    int numpoints = 0;
    // int scriptlen =
    Pt3Scriptnumpoints(ps, &numpoints);
    Pt3Script *s = COUNTMEM(Pt3Script, 1);
    s->name = ps->name; // static name (easy to match in discupdate!)
    s->script = ps->script; // static script (easy to match in discupdate!)
//    s->script = COUNTMEM(char, scriptlen + 1);
//    strcpy(s->script, ps->script);
    if (numpoints > 0) {
      s->points = COUNTMEM(Pt3, numpoints);
      int sp = -1;
      while (++sp < numpoints)
        { s->points[sp] = p->script->points[sp]; }
    } else { s->points = NULL; printf("NOPOINTS\n"); }
    newlist[newli].script = s; // point to new item
    if (p->next) {
      newlist[newli].next = &newlist[newli + 1];
    } else { newlist[newli].next = NULL; }
    p = p->next;
  }
  return newlist; // address of list memory
}
void freePt3ScriptList(Pt3ScriptList *psl) {
  Pt3ScriptList *p = psl;
  while (p) {
    Pt3Script *s = p->script;
//    free(s->script); // static for discupdate matching !
    free(s->points);
    p = p->next;
//    free(s); <-- freeing list address frees all s items
  }
  free(psl);
}


// aroundref ......
void xaxisrotatePt3ScriptList(Pt3ScriptList *psl, double spindeg) {
  Pt3PT3VAR(xaxis, 0.0, 0.0, 0.0);
  Pt3ScriptList *p = psl;
  while (p) {
    Pt3Script *s = p->script;
    int numpoints = 0;
    //int scriptlen =
    Pt3Scriptnumpoints(s, &numpoints);
    int ptix = -1;
    while (++ptix < numpoints) {
      s->points[ptix] = Pt3xrotatearoundref(s->points[ptix], spindeg, xaxis);
    }
    p = p->next;
  }
}

// aroundref ......
void yaxisrotatePt3ScriptList(Pt3ScriptList *psl, double flipdeg) {
  Pt3PT3VAR(yaxis, 0.0, 0.0, 0.0);
  Pt3ScriptList *p = psl;
  while (p) {
    Pt3Script *s = p->script;
    int numpoints = 0;
    //int scriptlen =
    Pt3Scriptnumpoints(s, &numpoints);
    int ptix = -1;
    while (++ptix < numpoints) {
      s->points[ptix] = Pt3yrotatearoundref(s->points[ptix], flipdeg, yaxis);
    }
    p = p->next;
  }
}

// aroundref ......
void zaxisrotatePt3ScriptList(Pt3ScriptList *psl, double twistdeg) {
  Pt3PT3VAR(zaxis, 0.0, 0.0, 0.0);
  Pt3ScriptList *p = psl;
  while (p) {
    Pt3Script *s = p->script;
    int numpoints = 0;
    //int scriptlen =
    Pt3Scriptnumpoints(s, &numpoints);
    int ptix = -1;
    while (++ptix < numpoints) {
      s->points[ptix] = Pt3zrotatearoundref(s->points[ptix], twistdeg, zaxis);
    }
    p = p->next;
  }
}

// ------------------------------------------------------------------------
void yPt3rotaterowcol(mat X, mat Y, mat Z, int row, int col, Pt3 offset, double flipdeg) {
  int r = row;
  int c = col;
  double Xv = X.v[r][c] - offset.x;
  double Zv = Z.v[r][c] - offset.z;
  double radangle = flipdeg * DEGTORAD;
  double Xnew = Zv * sin(radangle) + Xv * cos(radangle);
  double Znew = Zv * cos(radangle) - Xv * sin(radangle);
  X.v[r][c] = Xnew + offset.x;
  Y.v[r][c] = Y.v[r][c];
  Z.v[r][c] = Znew + offset.z;
}
void yPt3rotaterow(mat X, mat Y, mat Z, int row, Pt3 offset, double flipdeg) {
  int r = row, c = -1;
  while (++c < X.cols && c < Y.cols && c < Z.cols)
    { yPt3rotaterowcol(X, Y, Z, r, c, offset, flipdeg); }
}
void yPt3rotate(mat X, mat Y, mat Z, Pt3 offset, double flipdeg) {
  int r = -1;
  while (++r < X.rows && r < Y.rows && r < Z.rows)
    { yPt3rotaterow(X, Y, Z, r, offset, flipdeg); }
}

char normalboxscript[] = "C0,1,2,3,0;C0,4;C1,5;C2,6;C3,7;C4,5,6,7,4"; // 8 points
Pt3Script Pt3Scriptxzbox(Vector range, char *name, double width, double height) {
  double halfwidth = width / 2.0;
  double halfheight = height / 2.0;
//  Pt3 boxhalfwidth = Pt3offsetalongVector(range, halfwidth);
//  Pt3 boxmbl = Pt3yrotatearoundref(boxhalfwidth, -90.0, range.start);
//  Pt3 boxmbr = Pt3zrotatearoundref(boxhalfwidth,  90.0, range.start);
//  Pt3 box
//  printf("BOXMBR: %f %f %f\n", boxmbr.x, boxmbr.y, boxmbr.z);
//  VectorVECTORVAR(halfboxwidth, range.start, boxmbr);
  Pt3 boxhalfheight = Pt3offsetalongVector(range, halfheight);
  Pt3 boxubm = Pt3zrisealongxzref(boxhalfheight, 90.0, range.start);
  VectorVECTORVAR(halfboxheight, range.start, boxubm);
  Pt3 boxfullheight = Pt3offsetalongVector(halfboxheight, height);
  VectorVECTORVAR(fullboxheight, range.start, boxfullheight);
  Pt3 boxubl = Pt3subz(boxubm, halfwidth); //Pt3subvectordistance(boxubm, halfboxwidth);
  Pt3 boxubr = Pt3addz(boxubm, halfwidth); //Pt3addvectordistance(boxubm, halfboxwidth);
  Pt3 boxutl = Pt3addvectordistance(boxubl, range);
  Pt3 boxutr = Pt3addvectordistance(boxubr, range);
  Pt3 boxlbl = Pt3subvectordistance(boxubl, fullboxheight);
  Pt3 boxlbr = Pt3subvectordistance(boxubr, fullboxheight);
  Pt3 boxltl = Pt3subvectordistance(boxutl, fullboxheight);
  Pt3 boxltr = Pt3subvectordistance(boxutr, fullboxheight);
  Pt3Script8(boxscript, name, normalboxscript,
             boxlbl, boxlbr, boxltr, boxltl,
             boxubl, boxubr, boxutr, boxutl);
  return boxscript;
}
/*
Pt3Script Pt3Scriptxzybox(Vector range, double width, double height) {
  double halfwidth = width / 2.0;
  double halfheight = height / 2.0;
  Pt3 boxhalfwidth = Pt3offsetalongVector(range, halfwidth);
  Pt3 boxlbl = Pt3yrotatearoundref(boxhalfwidth, -90.0, range.start);
  Pt3 boxlbr = Pt3yrotatearoundref(boxhalfwidth,  90.0, range.start);
  Pt3 boxltl = Pt3addvectordistance(boxlbl, range);
  Pt3 boxltr = Pt3addvectordistance(boxlbr, range);
  Pt3 boxubl = Pt3yoffset(boxlbl, -halfheight);
  Pt3 boxubr = Pt3yoffset(boxlbr, -halfheight);
  Pt3 boxutl = Pt3yoffset(boxltl, -halfheight);
  Pt3 boxutr = Pt3yoffset(boxltr, -halfheight);
  boxlbl = Pt3yoffset(boxlbl, halfheight);
  boxlbr = Pt3yoffset(boxlbr, halfheight);
  boxltl = Pt3yoffset(boxltl, halfheight);
  boxltr = Pt3yoffset(boxltr, halfheight);
  Pt3Script8(boxscript, normalboxscript,
             boxlbl, boxlbr, boxltr, boxltl,
             boxubl, boxubr, boxutr, boxutl);
  return boxscript;
}
*/
