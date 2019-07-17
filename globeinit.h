
void globe2dinit() {
#include "disckeys.h"
  arr latarcx, latarcy, latarcz;
  arralloc(&latarcx, latNtoS.len);
  arralloc(&latarcy, latNtoS.len);
  arralloc(&latarcz, latNtoS.len);
  int li = -1;
  while (++li < latNtoS.len) {
    latarcx.v[li] = sin(latNtoS.v[li] * DEGTORAD) * globeamplitude;
    latarcy.v[li] = cos(latNtoS.v[li] * DEGTORAD) * globeamplitude;
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
  #include "discinit.h"
  // ^ uses Xya, Yya, Zya, YAx, YAy, YAz
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
