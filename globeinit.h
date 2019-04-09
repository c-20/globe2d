
void globe2dinit() {
  VARLETTERKEYS(globeceilingx,  'O', 'P');  // O <--   top centre X   ++> P
  VARLETTERKEYS(globeceilingy,  'U', 'I');  // U <--   top centre Y   ++> I
  VARLETTERKEYS(discfloorz,     'B', 'G');  // B <--   disc distance  ++> G
  VARLETTERKEYS(discfloorx,     'K', 'L');  // K <-- bottom centre X  ++> L
  VARLETTERKEYS(discfloory,     'H', 'J');  // H <-- bottom centre Y  ++> J
  VARLETTERKEYS(discrotangle,   'N', 'M');  // N <--   disc angle     ++> M
  VARLETTERKEYS(discarmABangle, 'V', 'F');  // V <-- segment 1 angle  ++> F
  VARLETTERKEYS(discarmBCangle, 'C', 'D');  // C <-- segment 2 angle  ++> D
  VARLETTERKEYS(discarmCDangle, 'X', 'S');  // X <--  platform angle  ++> S
  VARLETTERKEYS(discarmAlength, 'Q', 'W');  // Q <--   disc radius    ++> W
  VARLETTERKEYS(discarmBlength, 'E', 'R');  // E <-- segment 1 radius ++> R
  VARLETTERKEYS(discarmClength, 'T', 'Y');  // T <-- segment 2 radius ++> Y
  VARLETTERKEYS(discarmDlength, 'Z', 'A');  // Z <-- platform offset  ++> A
  arr latarcx, latarcy, latarcz;
  arralloc(&latarcx, latNtoS.len);
  arralloc(&latarcy, latNtoS.len);
  arralloc(&latarcz, latNtoS.len);
  int li = -1;
  while (++li < latNtoS.len) {
    latarcx.v[li] = sin(latNtoS.v[li] * DEGTORAD) * amplitude;
    latarcy.v[li] = cos(latNtoS.v[li] * DEGTORAD) * amplitude;
    latarcz.v[li] = 0; // latNtoS[li] * 0;
  } // poles from x around yz
  meshgridalloc(latarcx, longPMtoPM, &Xya, &YAx);
  meshgridalloc(latarcy, longPMtoPM, &Yya, &YAy);
  meshgridalloc(latarcz, longPMtoPM, &Zya, &YAz);
  matalloc(&longX, Xya.rows, Xya.cols);
  matalloc(&longY, Yya.rows, Yya.cols);
  matalloc(&longZ, Zya.rows, Zya.cols);
  int ri = -1; // build long rings
  while (++ri < longX.rows && ri < longY.rows && ri < longZ.rows) {
    int ci = -1;
    while (++ci < longX.cols && ci < longY.cols && ci < longZ.cols) {
      longX.v[ri][ci]  = Zya.v[ri][ci] * sin(YAz.v[ri][ci] * DEGTORAD);
      longX.v[ri][ci] += Xya.v[ri][ci] * cos(YAx.v[ri][ci] * DEGTORAD);
      longY.v[ri][ci]  = Yya.v[ri][ci];
      longZ.v[ri][ci]  = Zya.v[ri][ci] * cos(YAz.v[ri][ci] * DEGTORAD);
      longZ.v[ri][ci] -= Xya.v[ri][ci] * sin(YAx.v[ri][ci] * DEGTORAD);
    } // create long rings
  } // Z and X spin around XZ (Y Angle)
  matalloc(&latX, longX.cols, longX.rows);
  matalloc(&latY, longY.cols, longY.rows);
  matalloc(&latZ, longZ.cols, longZ.rows);
  ri = -1; // build lat rings ... arcs ?
  while (++ri < latX.rows && ri < latY.rows && ri < latZ.rows) {
    int ci = -1;
    while (++ci < latX.cols && ci < latY.cols && ci < latZ.cols) {
      latX.v[ri][ci] = longX.v[ci][ri];
      latY.v[ri][ci] = longY.v[ci][ri];
      latZ.v[ri][ci] = longZ.v[ci][ri];
    } // create lat rings -- maybe need arcs ?
      // should maybe only draw half of the points ?
  }
  // declare region selection memory
  matalloc(&regionX, rlevels + 2, 5); // 15, 1, and 10 digits
  matalloc(&regionY, rlevels + 2, 5);
  matalloc(&regionZ, rlevels + 2, 5);
  // create disc/arm
  // disc is a copy of the equatorial long ring
  if (1) { // displaydisc
    matalloc(&discX, 2, Xya.rows);
    matalloc(&discY, 2, Yya.rows);
    matalloc(&discZ, 2, Zya.rows);
    int ri = -1;
    while (++ri < discX.cols && ri < discY.cols && ri < discZ.cols) {
      discX.v[0][ri]  = Zya.v[ri][latNtoSmiddle] * sin(YAz.v[ri][latNtoSmiddle] * DEGTORAD);
      discX.v[0][ri] += Xya.v[ri][latNtoSmiddle] * cos(YAx.v[ri][latNtoSmiddle] * DEGTORAD);
      discY.v[0][ri]  = -amplitude; // align with top // Yya.v[ri][latNtoSmiddle];
      discZ.v[0][ri]  = Zya.v[ri][latNtoSmiddle] * cos(YAz.v[ri][latNtoSmiddle] * DEGTORAD);
      discZ.v[0][ri] -= Xya.v[ri][latNtoSmiddle] * sin(YAx.v[ri][latNtoSmiddle] * DEGTORAD);
      discX.v[1][ri]  = discX.v[0][ri] * discamplitude / amplitude;
      discY.v[1][ri]  = discY.v[0][ri]; // * discamplitude / amplitude;
      discZ.v[1][ri]  = discZ.v[0][ri] * discamplitude / amplitude;
//      printf("discXYZ: %f, %f, %f\n", )
    } // create disc ring - Z and X spin around XZ (Y Angle)
    matalloc(&armX, 1, 5);
    matalloc(&armY, 1, 5);
    matalloc(&armZ, 1, 5);
    armX.v[0][0] = discX.v[0][0];
    armY.v[0][0] = discY.v[0][0];
    armZ.v[0][0] = discZ.v[0][0];
    armX.v[0][1] = discX.v[1][0];
    armY.v[0][1] = discY.v[1][0];
    armZ.v[0][1] = discZ.v[1][0];

  }
  // clean up local allocs
  free3arrs(latarcx, latarcy, latarcz);
  free3mats(Xya, Yya, Zya);
  free3mats(YAx, YAy, YAz);
}

void globe2dfree() {
  free3mats(longX, longY, longZ);
  free3mats(latX, latY, latZ);
  free3mats(regionX, regionY, regionZ);
  free3mats(discX, discY, discZ);
  free3mats(armX, armY, armZ);
}
