#define WINDOWSTARGET     // target should be defined by main before include

#include "mesh.h"         // include matrix transformation routines
#include "draw2d.h"       // include perspective drawing routines

#include "discvars.h"     // include disc variables/trigger routines

//% GLOBE
double latdeginc = 15.0; // % HOW MANY LONG INCREMENTS WITH LAT ARCS
double latNtoSv[] = {0,15,30,45,60,75,90,105,120,135,150,165,180};
int latNtoScount = 13; // 13 arc angles from 0 to 180 inclusive (0 N to 0 S)
int latNtoSmiddle = 6;
arr latNtoS = fixedarr(latNtoSv, latNtoScount);
double amplitude = 20.0; // sin and cos 0-180 = half-circle
//latNtoS = 0:latdeginc:180; % N is 0, S is 180
double longdeginc = 15.0;
double longPMtoPMv[] = {0, 15, 30, 45, 60, 75, 90,105,120,135,150,165,180,
                          195,210,225,240,255,270,285,300,315,330,345,360 };
int longPMtoPMcount = 25; // 25 circle angles from 0 to 360 inclusive (0 to 360 E?)
int longPMtoPMmiddle = 12; // 13th circle is the middle one
arr longPMtoPM = fixedarr(longPMtoPMv, longPMtoPMcount);
double latminsperstep = 60.0; // % 1 hour per step (1 degree)
double latstepsperhour = 60.0 / latminsperstep;
double longminsperstep = 60.0; // % 1 hour per step (1 degree)
double longstepsperhour = 60.0 / longminsperstep;
//%gXsteps = longstepsperhour * longdeginc; % 10 minutes per step Xlong
//%gYsteps = latstepsperhour * latdeginc;  % 10 minutes per step Ylat
double gXsteps = latdeginc + 1; // % 1 degree per square (15 boxes, 16 points)
double gYsteps = longdeginc + 1; // % 1 degree per square (15 boxes, 16 points)

int rlevels = 20; // how many decimal digits of lat/long

double discamplitude = 60.0;

// draw a circle around each point of the arc
mat Xya, YAx, Yya, YAy, Zya, YAz;
mat longX, longY, longZ; // formerly XYZ
mat latX, latY, latZ; // formerly XYZequ
mat regionX, regionY, regionZ; //% grid levels 15-1-.(10), a TL TR BR BL TL per row
mat discX, discY, discZ; // disc ring
mat armX, armY, armZ; // disc arm

// --------------------------------------------------------- INIT   -------------
#include "globeinit.h"

// --------------------------------------------------------- UPDATE -------------
int mouseinctrl = 0;
int mouseinshift = 0;
int mdx[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int mdy[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int mdlen = 28;
int mdix = 0;
int mouselastx = 0;
int mouselasty = 0;
double scaleshiftx = 0.0;
double scaleshifty = 0.0;
double scaledivisor = 1.0;
double scaledirection = 1.0;
double rotatex = 0.0;
double rotatey = 0.0;
double rotatedivisor = 1.0;
double rotatedivisorscalex = 1.0;
double rotatedivisorscaley = 1.0;
double scalex = 1.0;
double scaley = 1.0;
int xoffset = 0;
int yoffset = 0;
// globekeys included at top of file

void globe2dupdate(HDC dc, int clientw, int clienth) {
  #include "globectrl.h" // {
    // offsetx and offsety used to determine lat/long rotation
    // other vars to be added....................................

    // will it jump on divisor change? need to test .................
//    txt = sprintf('%f\n%f\n*1/%d\n', mousex, mousey, mousedivisor);
//    set(TH1, 'String', txt);
//
//
//%    cursor2 = get(AH, 'CurrentPoint');
//    myLong = mousex ./ ss(3) .* 360 - 180;
//    myLat = mousey ./ ss(4) .* 180 - 90;
//    %myLong = mod(cursor(1), 360) - 180;
//    %myLat = mod(cursor(2), 180) - 90;

//    double myLat = doublemod(mousey, 360.0) - 90.0;
//    double myLong = doublemod(mousex, 360.0) - 180.0;

//    double discLat = GETVARVALUE(discfloorz);
    double discLong = GETVARVALUE(discrotangle);



    double myLat = doublemod(offsety, 360.0) - 90.0;
    double myLong = doublemod(offsetx, 360.0) - 180.0;

    // if myLat is 27.5, minLat - maxLat is 15-30
    double nsLat = 90.0 - myLat; // 90->0,0->90,-90->180 (= 0-180 N-S)
//%minX = (nsLat - mod(nsLat, latdeginc)) / latdeginc;
//%maxX = (nsLat - mod(nsLat, latdeginc) + latdeginc) / latdeginc;
    if (nsLat < 0) { nsLat += 360.0; } // handle backface with positive angles
    double minLat = nsLat - doublemod(nsLat, latdeginc);
    double minLatColX = minLat / latdeginc; // + 1.0; // % +1 for (1)
    double minLatOffX = floor(nsLat - minLat); // + 1.0; // % +1 for (1)
    double maxLat = minLat + latdeginc;
    double maxLatColX = maxLat / latdeginc; // + 1.0;
    double maxLatOffX = minLatOffX + 1.0; // % 1 sq high (15sq has 16 l)
    double latSubX = nsLat - minLat - minLatOffX; //(minLatOffX - 1.0); // % decimal part of lat
    // if myLong is 127.5, minLong-maxLong is 120-135
    double weLong = myLong + 180.0; // % -180 -> 0, 0 -> 180, 180 -> 360 (check ewLR)
    double minLong = weLong - doublemod(weLong, longdeginc);
    double minLongRowY = minLong / longdeginc; // + 1.0; // % +1 for (1)
    double minLongOffY = floor(weLong - minLong); // + 1.0; // % +1 for (1)
    double maxLong = minLong + longdeginc;
    double maxLongRowY = maxLong / longdeginc; // + 1.0;
    double maxLongOffY = minLongOffY + 1.0; // % 1 sq wide (15 sq has 16 l)
    double longSubY = weLong - minLong - minLongOffY; //(minLongOffY - 1.0);

    int minX = (int)abs(doublemod(minLatColX, latNtoScount)); // + 1.0; cancel (1) offset
    int maxX = (int)abs(doublemod(maxLatColX, latNtoScount)); // + 1.0; cancel (1) offset
    int minY = (int)abs(doublemod(minLongRowY, longPMtoPMcount)); // + 1.0; cancel (1) offset
    int maxY = (int)abs(doublemod(maxLongRowY, longPMtoPMcount)); // + 1.0; cancel (1) offset

    //% element [0] is 0 degrees = North, -25 lat would be 115 from N
    //% -- Selected region ----------- TL TR BL BR --------------- maybe unused ---
//    double selX[] = { longX.v[minY][minX], longX.v[minY][maxX], longX.v[maxY][minX], longX.v[maxY][maxX] };
//    double selY[] = { longY.v[minY][minX], longY.v[minY][maxX], longY.v[maxY][minX], longY.v[maxY][maxX] };
//    double selZ[] = { longZ.v[minY][minX], longZ.v[minY][maxX], longZ.v[maxY][minX], longZ.v[maxY][maxX] };

    //% -- Scaling selected region ------------------------------------
    //% map and add the outer (15 degree) square
    double regionTLX = longX.v[minY][minX], regionTRX = longX.v[minY][maxX];
    double regionTLY = longY.v[minY][minX], regionTRY = longY.v[minY][maxX];
    double regionTLZ = longZ.v[minY][minX], regionTRZ = longZ.v[minY][maxX];
    double regionBLX = longX.v[maxY][minX], regionBRX = longX.v[maxY][maxX];
    double regionBLY = longY.v[maxY][minX], regionBRY = longY.v[maxY][maxX];
    double regionBLZ = longZ.v[maxY][minX], regionBRZ = longZ.v[maxY][maxX];
    double regionX0[] = { regionTLX, regionTRX, regionBRX, regionBLX, regionTLX };
    double regionY0[] = { regionTLY, regionTRY, regionBRY, regionBLY, regionTLY };
    double regionZ0[] = { regionTLZ, regionTRZ, regionBRZ, regionBLZ, regionTLZ };
    //% create T L B R edge rulers to find the square
    arr regionTX, regionTY, regionTZ;
    arr regionBX, regionBY, regionBZ;
    arr regionLX, regionLY, regionLZ;
    arr regionRX, regionRY, regionRZ;
    linspacealloc(regionTLX, regionTRX, gXsteps, &regionTX); //%  -
    linspacealloc(regionTLY, regionTRY, gXsteps, &regionTY); //%  -  top
    linspacealloc(regionTLZ, regionTRZ, gXsteps, &regionTZ); //%  -
    linspacealloc(regionBLX, regionBRX, gXsteps, &regionBX); //%  _
    linspacealloc(regionBLY, regionBRY, gXsteps, &regionBY); //%  _  bottom
    linspacealloc(regionBLZ, regionBRZ, gXsteps, &regionBZ); //%  _
    linspacealloc(regionTLX, regionBLX, gYsteps, &regionLX); //% |
    linspacealloc(regionTLY, regionBLY, gYsteps, &regionLY); //% |   left
    linspacealloc(regionTLZ, regionBLZ, gYsteps, &regionLZ); //% |
    linspacealloc(regionTRX, regionBRX, gYsteps, &regionRX); //%   |
    linspacealloc(regionTRY, regionBRY, gYsteps, &regionRY); //%   | right
    linspacealloc(regionTRZ, regionBRZ, gYsteps, &regionRZ); //%   |
    // create T and B rulers from L to R to map square in square
    arr regionTLRX, regionTLRY, regionTLRZ;
    arr regionBLRX, regionBLRY, regionBLRZ;
    int rxmin = (int)minLatOffX, rxmax = (int)maxLatOffX;
    int rymin = (int)minLongOffY, rymax = (int)maxLongOffY;
//    if (rymin < 0) { rymin += longdeginc; }
//    if (rymax < 0) { rymax += longdeginc; }
//printf("%d %d, %d %d ...\n", rxmin, rxmax, rymin, rymax);
//  printf("%f %f %f %f\n", minLat, minLatColX, minLatOffX, latSubX);

    linspacealloc(regionLX.v[rymin], regionRX.v[rymin], gXsteps, &regionTLRX);
    linspacealloc(regionLY.v[rymin], regionRY.v[rymin], gXsteps, &regionTLRY);
    linspacealloc(regionLZ.v[rymin], regionRZ.v[rymin], gXsteps, &regionTLRZ);
    linspacealloc(regionLX.v[rymax], regionRX.v[rymax], gXsteps, &regionBLRX);
    linspacealloc(regionLY.v[rymax], regionRY.v[rymax], gXsteps, &regionBLRY);
    linspacealloc(regionLZ.v[rymax], regionRZ.v[rymax], gXsteps, &regionBLRZ);
    //linspace(regionTX[rxmin], regionBX[rxmin], gYsteps, &regionLTBX);
    //linspace(regionTY[rxmin], regionBY[rxmin], gYsteps, &regionLTBY);
    //linspace(regionTZ[rxmin], regionBZ[rxmin], gYsteps, &regionLTBZ);
    //linspace(regionTX[rxmax], regionBX[rxmax], gYsteps, &regionRTBX);
    //linspace(regionTY[rxmax], regionBY[rxmax], gYsteps, &regionRTBY);
    //linspace(regionTZ[rxmax], regionBZ[rxmax], gYsteps, &regionRTBZ);
    // map corners of this 1 degree region to prepare for next .1 region
    regionTLX = regionTLRX.v[rxmin]; // TLR
    regionTLY = regionTLRY.v[rxmin];
    regionTLZ = regionTLRZ.v[rxmin];
    regionTRX = regionTLRX.v[rxmax]; // --------|  |----------
    regionTRY = regionTLRY.v[rxmax];
    regionTRZ = regionTLRZ.v[rxmax];
    regionBLX = regionBLRX.v[rxmin]; // BLR
    regionBLY = regionBLRY.v[rxmin];
    regionBLZ = regionBLRZ.v[rxmin];
    regionBRX = regionBLRX.v[rxmax]; // --------|  |----------
    regionBRY = regionBLRY.v[rxmax];
    regionBRZ = regionBLRZ.v[rxmax];
    double nextregionX0[] = { regionTLX, regionTRX, regionBRX, regionBLX, regionTLX };
    double nextregionY0[] = { regionTLY, regionTRY, regionBRY, regionBLY, regionTLY };
    double nextregionZ0[] = { regionTLZ, regionTRZ, regionBRZ, regionBLZ, regionTLZ };
    // int rlevels = 10; // declared global
    matsetrow(regionX, 0, regionX0);
    matsetrow(regionX, 1, nextregionX0);
    matsetrow(regionY, 0, regionY0);
    matsetrow(regionY, 1, nextregionY0);
    matsetrow(regionZ, 0, regionZ0);
    matsetrow(regionZ, 1, nextregionZ0);
    //% magnify.......
    double latdigit = floor(latSubX * 10.0);
    double latdigits = (latSubX * 10.0) - latdigit;
    double longdigit = floor(longSubY * 10.0);
    double longdigits = (longSubY * 10.0) - longdigit;
    //latdigit  = (latdigit < 0 ) ? 9 + latdigit  : latdigit;
    longdigit = (longdigit < 0) ? 9 + longdigit : longdigit;
    rxmin = (int)abs(latdigit), rxmax = rxmin + 1;
    rymin = (int)abs(longdigit), rymax = rymin + 1;
    int rlevel = 1;
    while (rlevel <= rlevels) {
      //% update T L B R edge rulers to find the next square
//    free3arrs(regionTX, regionTY, regionTZ);
//    free3arrs(regionBX, regionBY, regionBZ);
//    linspace(regionTLX, regionTRX, 11, &regionTX); //%  -   10 grid squares,
//    linspace(regionTLY, regionTRY, 11, &regionTY); //%  -  top
//    linspace(regionTLZ, regionTRZ, 11, &regionTZ); //%  -
//    linspace(regionBLX, regionBRX, 11, &regionBX); //%  _   11 points
//    linspace(regionBLY, regionBRY, 11, &regionBY); //%  _  bottom
//    linspace(regionBLZ, regionBRZ, 11, &regionBZ); //%  _
      free3arrs(regionLX, regionLY, regionLZ);
      free3arrs(regionRX, regionRY, regionRZ);
      linspacealloc(regionTLX, regionBLX, 11, &regionLX); //% |    10 grid squares,
      linspacealloc(regionTLY, regionBLY, 11, &regionLY); //% |   left
      linspacealloc(regionTLZ, regionBLZ, 11, &regionLZ); //% |
      linspacealloc(regionTRX, regionBRX, 11, &regionRX); //%   |  11 points
      linspacealloc(regionTRY, regionBRY, 11, &regionRY); //%   | right
      linspacealloc(regionTRZ, regionBRZ, 11, &regionRZ); //%   |
      free3arrs(regionTLRX, regionTLRY, regionTLRZ);
      free3arrs(regionBLRX, regionBLRY, regionBLRZ);
      linspacealloc(regionLX.v[rymin], regionRX.v[rymin], 11, &regionTLRX);
      linspacealloc(regionLY.v[rymin], regionRY.v[rymin], 11, &regionTLRY);
      linspacealloc(regionLZ.v[rymin], regionRZ.v[rymin], 11, &regionTLRZ);
      linspacealloc(regionLX.v[rymax], regionRX.v[rymax], 11, &regionBLRX);
      linspacealloc(regionLY.v[rymax], regionRY.v[rymax], 11, &regionBLRY);
      linspacealloc(regionLZ.v[rymax], regionRZ.v[rymax], 11, &regionBLRZ);
      //% map the inner (.1, .01, .001, .0001 degree) square
      regionTLX = regionTLRX.v[rxmin];
      regionTLY = regionTLRY.v[rxmin];
      regionTLZ = regionTLRZ.v[rxmin];
      regionTRX = regionTLRX.v[rxmax];
      regionTRY = regionTLRY.v[rxmax];
      regionTRZ = regionTLRZ.v[rxmax];
      regionBLX = regionBLRX.v[rxmin];
      regionBLY = regionBLRY.v[rxmin];
      regionBLZ = regionBLRZ.v[rxmin];
      regionBRX = regionBLRX.v[rxmax];
      regionBRY = regionBLRY.v[rxmax];
      regionBRZ = regionBLRZ.v[rxmax];
      //% the left in the top.............etc
      //% add square to list
      double nextregionX[] = { regionTLX, regionTRX, regionBRX, regionBLX, regionTLX };
      double nextregionY[] = { regionTLY, regionTRY, regionBRY, regionBLY, regionTLY };
      double nextregionZ[] = { regionTLZ, regionTRZ, regionBRZ, regionBLZ, regionTLZ };
      matsetrow(regionX, rlevel + 1, nextregionX);
      matsetrow(regionY, rlevel + 1, nextregionY);
      matsetrow(regionZ, rlevel + 1, nextregionZ);
      //% get next digit of lat.nnnnnn and long.nnnnnn
      latdigit = floor(latdigits * 10.0);
      latdigits = (latdigits * 10.0) - latdigit;
      longdigit = floor(longdigits * 10.0);
      longdigits = (longdigits * 10.0) - longdigit;
      latdigit  = (latdigit < 0 ) ? 9 + latdigit  : latdigit;
      longdigit = (longdigit < 0) ? 9 + longdigit : longdigit;
      rxmin = (int)abs(latdigit),  rxmax = rxmin + 1;
      rymin = (int)abs(longdigit), rymax = rymin + 1;
      //% ready for the next iteration (region will be added)
      rlevel++;
    }
    free3arrs(regionTX, regionTY, regionTZ);
    free3arrs(regionBX, regionBY, regionBZ);
    free3arrs(regionLX, regionLY, regionLZ);
    free3arrs(regionRX, regionRY, regionRZ);
    free3arrs(regionTLRX, regionTLRY, regionTLRZ);
    free3arrs(regionBLRX, regionBLRY, regionBLRZ);

    mat LoX, LoY, LoZ; // uses init values as reference
    mat LaX, LaY, LaZ; // uses init values as reference
    mat ReX, ReY, ReZ; // recalculated each frame anyway
    copyalloc3mats(longX, longY, longZ, &LoX, &LoY, &LoZ);
    copyalloc3mats(latX, latY, latZ, &LaX, &LaY, &LaZ);
    copyalloc3mats(regionX, regionY, regionZ, &ReX, &ReY, &ReZ);
    mat DiX, DiY, DiZ; // uses init values as reference
    mat ArX, ArY, ArZ; // uses init values as reference
    copyalloc3mats(discX, discY, discZ, &DiX, &DiY, &DiZ);
    copyalloc3mats(armX, armY, armZ, &ArX, &ArY, &ArZ);

    #include "discarm.h"

    //% align N-S vertically (0 is along Y around X)
    double tovertical = 90.0; //% + offset
    xaxisrotate(LoX, LoY, LoZ, tovertical);
    xaxisrotate(LaX, LaY, LaZ, tovertical);
    xaxisrotate(ReX, ReY, ReZ, tovertical);
    xaxisrotate(DiX, DiY, DiZ, tovertical);
    xaxisrotate(ArX, ArY, ArZ, tovertical);
    //double longwidth = maxLong - minLong;
    double spintozero = (180.0 - weLong); // % - (longwidth ./ 2); % minY * latdeginc - 90;
    double disctozero = (180.0 - discLong);
    zaxisrotate(LoX, LoY, LoZ, spintozero);
    zaxisrotate(LaX, LaY, LaZ, spintozero);
    zaxisrotate(ReX, ReY, ReZ, spintozero);
    zaxisrotate(DiX, DiY, DiZ, disctozero);
    zaxisrotate(ArX, ArY, ArZ, disctozero);
    //
    double latheight = maxLat - minLat;
    double spinup = (nsLat - 90.0) + latheight + (latheight / 2.0); // % mod(cursor(1), 360);
    spinup += rotatex;
    double discup = spinup; // rotate with globe
    yaxisrotate(LoX, LoY, LoZ, spinup);
    yaxisrotate(LaX, LaY, LaZ, spinup);
    yaxisrotate(ReX, ReY, ReZ, spinup);
    yaxisrotate(DiX, DiY, DiZ, discup);
    yaxisrotate(ArX, ArY, ArZ, discup);
    //
    double spintofront = 90;
    zaxisrotate(LoX, LoY, LoZ, spintofront);
    zaxisrotate(LaX, LaY, LaZ, spintofront);
    zaxisrotate(ReX, ReY, ReZ, spintofront);
    zaxisrotate(DiX, DiY, DiZ, spintofront);
    zaxisrotate(ArX, ArY, ArZ, spintofront);
    //
    double spinright = 30 + rotatey; // % mod(cursor(1), 360);
    double discright = spinright; // 30 + discLong; // ; // disc spins with keyboard
    xaxisrotate(LoX, LoY, LoZ, spinright);
    xaxisrotate(LaX, LaY, LaZ, spinright);
    xaxisrotate(ReX, ReY, ReZ, spinright);
    xaxisrotate(DiX, DiY, DiZ, discright);
    xaxisrotate(ArX, ArY, ArZ, discright);
    //
//    char drawmode = 1; // mode is 1,2,3 for XY, YZ, XZ
//    int xpos = clientw / 4;
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 1, LoX, LoY, LoZ);
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 2, LaX, LaY, LaZ);
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 3, ReX, ReY, ReZ);
    char drawmode = 2; // mode is 1,2,3 for XY, YZ, XZ
    int ycentre = (clienth / 2); // + (int)scaleshifty;
    int xcentre = (clientw / 2); // + (int)scaleshiftx;
    int trv[] = {  0,0,  0,0,  0,0,  0,0  };
    int xpos = xcentre + xoffset;
    int ypos = ycentre + yoffset;
    draw(dc, xpos, ypos, scalex, scaley, drawmode, 1, LoX, LoY, LoZ, -1, NULL);
    draw(dc, xpos, ypos, scalex, scaley, drawmode, 2, LaX, LaY, LaZ, -1, NULL);
    draw(dc, xpos, ypos, scalex, scaley, drawmode, 3, ReX, ReY, ReZ, mdix, trv);
    draw(dc, xpos, ypos, scalex, scaley, drawmode, 4, DiX, DiY, DiZ, -1, NULL);
    draw(dc, xpos, ypos, scalex, scaley, drawmode, 5, ArX, ArY, ArZ, -1, NULL);

    free3mats(LoX, LoY, LoZ);
    free3mats(LaX, LaY, LaZ);
    free3mats(ReX, ReY, ReZ);
    free3mats(DiX, DiY, DiZ);
    free3mats(ArX, ArY, ArZ);

    return;   // do not perform shift or rotate to centre the cursor


    // ------------------------ INCOMPLETE CODE BELOW --------------------------



//    trv[0] -= xpos; trv[2] -= xpos; trv[4] -= xpos; trv[6] -= xpos;
//    trv[1] -= ypos; trv[3] -= ypos; trv[5] -= ypos; trv[7] -= ypos;
 //   int xmidleft = trv[0] - xpos + ((trv[2] - trv[0]) / 2); // TLx + ((TRx - TLx) / 2)
 //   int ymidleft = trv[1] - ypos + ((trv[5] - trv[1]) / 2); // TLy + ((BLy - TLx) / 2)
 //   int xmidtop = trv[0] - xpos + ((trv[2] - trv[0]) / 2); // TLx + ((TRx - TLx) / 2)
 //   int ymidtop = trv[1] - ypos + ((trv[5] - trv[1]) / 2); // TLy + ((BLy - TLy) / 2)
    double leftlimx   = (double)clientw / 4.0;
    double rightlimx  = leftlimx * 3.0;
    double toplimy    = (double)clienth / 4.0;
    double bottomlimy = toplimy * 3.0;
    /////////////////////////////////////////
//    if (trv[0] < llim || trv[4] < llim) {
//      double rotaterate =
//      rotatex += rotaterate;
    double margintop     = 0.2, marginbottom = 0.2;
    double marginleft    = 0.2, marginright  = 0.2;
    double mscalewidth   = (1.0 - marginleft - marginright) * (double)clientw;
    double mscaleheight  = (1.0 - margintop - marginbottom) * (double)clienth;
    double mleftheight   = fabs(trv[5] - trv[1]) + 1.0; // BLy - TLy
    double mrightheight  = fabs(trv[7] - trv[3]) + 1.0; // BRy - TRy
    double mtopwidth     = fabs(trv[2] - trv[0]) + 1.0; // TRx - TLx
    double mbottomwidth  = fabs(trv[6] - trv[4]) + 1.0; // BRx - BLx
    double mleftvaluex   = (double)(trv[0] + trv[4]) / 2.0; // avg TLx BLx
    double mrightvaluex  = (double)(trv[2] + trv[6]) / 2.0; // avg TRx BRx
    double mtopvaluey    = (double)(trv[1] + trv[3]) / 2.0; // avg TLy TRy
    double mbottomvaluey = (double)(trv[5] + trv[7]) / 2.0; // avg BLy BRy
    double mwidthvalue   = fabs(mrightvaluex - mleftvaluex);
    double mheightvalue  = fabs(mbottomvaluey - mtopvaluey);
    double scalexratio   = fabs(mscalewidth / ((mtopwidth + mbottomwidth) / 2.0));
    double scaleyratio   = fabs(mscaleheight / ((mleftheight + mrightheight) / 2.0));
  //  scalex = fabs(scalex);
  //  scaley = fabs(scaley);
//printf("scaleratio: %f, %f, scale: %f, %f\n", scalexratio, scaleyratio, scalex, scaley);
//printf("BOUNDS %f %f %f %f\n", mleftvaluex, mrightvaluex, mtopvaluey, mbottomvaluey);
//printf("TL %d,%d  TR %d,%d  ", trv[0], trv[1], trv[2], trv[3]);
//printf("BL %d,%d  BR %d,%d\n", trv[4], trv[5], trv[6], trv[7]);
    // shift by .2% of needed change
    double rotatedivisor = pow(10.0, mdix);
    double rotateshiftx = 0.0, rotateshifty = 0.0;
//    if (mleftvaluex < 0 || mrightvaluex < 0 || mtopvaluey < 0 || mbottomvaluey < 0) {
    if (mwidthvalue > mscalewidth || mheightvalue > mscaleheight) {
      scalex *= 0.999; // zoom out fast
      scaley *= 0.999; // zoom out fast
      printf("BOTHOUT .999%%\n");
    }// else {
      if (mleftvaluex < leftlimx) { // too far left
        if (mrightvaluex > rightlimx) { // + too far right ?
          scaleshiftx = -(scalexratio * 0.1); // zoom out
          printf("scaleshiftx: %f\n", scaleshiftx);
  //        ypp -= 0.1; yc += 0.1;
        } else { // shift right
          xoffset += fabs(leftlimx - mleftvaluex) / 10.0; //++;
          rotateshiftx = -0.01 / rotatedivisor;
          printf("rotateshiftx: %f\n", rotateshiftx);
        }
      } else if (mrightvaluex > rightlimx) { // too far right?
        xoffset -= fabs(mrightvaluex - rightlimx) / 10.0; // --; // cancels out if both <llim and >rlim
        rotateshiftx = 0.01 / rotatedivisor;
        printf("rotateshiftx: %f\n", rotateshiftx);
      } else {
        scaleshiftx = 0.01 * rotatedivisor; // scalexratio * 0.1 * rotatedivisor;
        printf("scaleshiftx: %f\n", scaleshiftx);
  //      ypp += 0.1; yc -= 0.1; // neither? zoom in
      }
      if (mtopvaluey < toplimy) {
        if (mbottomvaluey > bottomlimy) {
          scaleshifty = -(scaleyratio * 0.1); // zoom out
          printf("scaleshifty: %f\n", rotateshifty);
 //         ypp -= 0.1; yc += 0.1;
        } else {
          yoffset += fabs(toplimy - mtopvaluey) / 10.0; //++;
          rotateshifty = -0.01 / rotatedivisor;
          printf("rotateshifty: %f\n", rotateshifty);
        }
      } else if (mbottomvaluey > bottomlimy) {
        yoffset -= fabs(mbottomvaluey - bottomlimy) / 10.0; // --; // cancels out if both <tlim and >blim
        rotateshifty = 0.01 / rotatedivisor;
        printf("rotateshifty: %f\n", rotateshifty);
      } else {
        scaleshifty = 0.01 * rotatedivisor; //scaleyratio * 0.1 * rotatedivisor;
        printf("scaleshifty: %f\n", scaleshifty);
  //      ypp += 0.1; yc -= 0.1;
      }
    //}
    scalex += scaleshiftx;
    scaley += scaleshifty;
    rotatex += rotateshiftx;
    rotatey += rotateshifty;
//    if (trv[0] < llim || trv[4] < llim) { rotatex -= 0.01 / rotatedivisor; }
//    if (trv[2] > rlim || trv[6] > rlim) { rotatex += 0.01 / rotatedivisor; }
//    if (trv[1] < tlim || trv[3] < tlim) { rotatey -= 0.01 / rotatedivisor; }
//    if (trv[5] > blim || trv[7] > blim) { rotatey += 0.01 / rotatedivisor; }

//printf("scaledivisor: %f\n", scaledivisor);
//printf("rotatedivisor: %f\n", rotatedivisor);
//    if (trv[0] < xpos) { rotatex -= 0.1 / scaledivisor; }
//    if (trv[2] > xpos) { rotatex += 0.1 / scaledivisor; }
//    if (trv[1] < ypos) { rotatey -= 0.1 / scaledivisor; } // may flicker
//    if (trv[5] > ypos) { rotatey += 0.1 / scaledivisor; } // back and forth
//    if (trv[0] < xpos) { rotatex -= 0.05 / (rotatedivisor / scaledivisor); }
//    if (trv[2] > xpos) { rotatex += 0.05 / (rotatedivisor / scaledivisor); }
//    if (trv[1] < ypos) { rotatey -= 0.05 / (rotatedivisor / scaledivisor); } // may flicker
//    if (trv[5] > ypos) { rotatey += 0.05 / (rotatedivisor / scaledivisor); } // back and forth

//    if ()


//printf("MIDDLEXY  %d %d ", xmidoff, ymidoff);



//    drawmode = 3; // mode is 1,2,3 for XY, YZ, XZ
//    xpos = 3 * (clientw / 4);
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 1, LoX, LoY, LoZ);
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 2, LaX, LaY, LaZ);
//    draw(dc, xpos, ypos, scalex, scaley, drawmode, 3, ReX, ReY, ReZ);
  } // else nothing happens if no mouse pos
}
