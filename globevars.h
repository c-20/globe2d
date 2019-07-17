//% GLOBE
double globeamplitude = 20.0; // sin and cos 0-180 = half-circle
double latdeginc = 15.0; // % HOW MANY LONG INCREMENTS WITH LAT ARCS
double latNtoSv[] = {0,15,30,45,60,75,90,105,120,135,150,165,180};
// TODO: double latNtoSv = { 0:15:180 };
int latNtoScount = 13; // 13 arc angles from 0 to 180 inclusive (0 N to 0 S)
int latNtoSmiddle = 6;
arr latNtoS = fixedarr(latNtoSv, latNtoScount);
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
