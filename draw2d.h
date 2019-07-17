/* ************************************************************************************* */
/*
void setPixel(int x, int y, Colour c) {
	if (x < 0 || x >= screen.width) { return; }
	if (y < 0 || y >= screen.height) { return; }

	screen.pixels[screen.stride * y + x] = getColour(c);
}
*/
void swapInt(int *a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}

void drawLine(HDC dc, int x1, int y1, int x2, int y2, COLORREF c) {
//void drawLine(int x1, int y1, int x2, int y2, Colour c) { // include screen param ?
//fprintf(stderr, "line (%d, %d)-(%d, %d)\n", x1, y1, x2, y2);
//	int colour = getColour(c);
	bool steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep) { swapInt(&x1, &y1); swapInt(&x2, &y2); }
	if (x1 > x2) { swapInt(&x1, &x2); swapInt(&y1, &y2); }
	int dx = x2 - x1;
	int dy = abs(y2 - y1);
	int err = dx / 2;
	int ystep = (y1 < y2)? 1 : -1;

	int y = y1;
	int trueX, trueY;
    int x;
	for (x = x1; x <= x2; x++) {
		// Get the values
		trueX = (steep?y:x);
		trueY = (steep?x:y);
		// Update for the next pixel
		if ((err -= dy) < 0) {
			y += ystep;
			err += dx;
		}
		// Check boundaries and skip if not within
//		if (trueX < 0 || trueX >= screen.width) { continue; }
//		if (trueY < 0 || trueY >= screen.height) { continue; }
		// Draw
		SetPixel(dc, trueX, trueY, c);
//		screen.pixels[screen.stride * trueY + trueX] = colour;
	}
}

void drawThickLine(HDC dc, int thickness, int x1, int y1, int x2, int y2, COLORREF c) {
//void drawThickLine(int thickness, int x1, int y1, int x2, int y2, Colour c) {
	// Draw the middle line
	drawLine(dc, x1, y1, x2, y2, c);
	if (thickness > 1) {
		int offset = 0;
        int i;
		if (abs(y2 - y1) > abs(x2 - x1)) {
			// Steep line.. alternate between right and left
			for (i = 1; i < thickness; i++) {
				// Offset = [1 -1 2 -2 3 -3 4 -4]...
				if ((offset *= -1) >= 0) { offset++; }
				drawLine(dc, x1 + offset, y1, x2 + offset, y2, c);
			}
		} else {
			// Alternate between down and up
			for (i = 1; i < thickness; i++) {
				// Offset = [1 -1 2 -2 3 -3 4 -4]...
				if ((offset *= -1) >= 0) { offset++; }
				drawLine(dc, x1, y1 + offset, x2, y2 + offset, c);
			}
		}
		// TODO: Draw a circle at each point to round the corners
		//drawSolidCircle(newPoint(x1, y1), thickness / 2, c);
		//drawSolidCircle(newPoint(x2, y2), thickness / 2, c);
	}
}
/*
arr xymeshx = { NULL, 0 }, xymeshy = { NULL, 0 };
arr yxmeshx = { NULL, 0 }, yxmeshy = { NULL, 0 };
arr xzmeshx = { NULL, 0 }, xzmeshy = { NULL, 0 };
arr zxmeshx = { NULL, 0 }, zxmeshy = { NULL, 0 };
arr yzmeshx = { NULL, 0 }, yzmeshy = { NULL, 0 };
arr zymeshx = { NULL, 0 }, zymeshy = { NULL, 0 };
arr xyherex = { NULL, 0 }, xyherey = { NULL, 0 };
arr xzherex = { NULL, 0 }, xzherey = { NULL, 0 };
arr yzherex = { NULL, 0 }, yzherey = { NULL, 0 };
*/
double zpp = 100.0, zc = -100.0, zcx = 0.0, zcy = 0.0;
double ypp = 100.0, yc = -100.0, ycx = 0.0, ycz = 0.0;
double xpp = 100.0, xc = -100.0, xcy = 0.0, xcz = 0.0;

// mode is 1,2,3 for XY, YZ, XZ
void draw(HDC dc, int xpos, int ypos, double xscale, double yscale,
             char mode, char phase, mat xdata, mat ydata, mat zdata, int tlvl, int *trv) {
  COLORREF redcolour = RGB(255, 0, 0);
  COLORREF bluecolour = RGB(0, 0, 255);
  COLORREF blackcolour = RGB(0, 0, 0);
  COLORREF greycolour = RGB(150, 150, 150);
  COLORREF orangecolour = RGB(255, 204, 51);
  int row = -1;
  while (++row < xdata.rows && row < ydata.rows && row < zdata.rows) {
    double *tx = xdata.v[row];
    double *ty = ydata.v[row];
    double *tz = zdata.v[row];
    int xref = 0, yref = 0;
    xref = xref; yref = yref; // suppress set-but-not-used warning
    int col = -1;
    while (++col < xdata.cols && col < ydata.cols && col < zdata.cols) {
      double zcscale  =     (zc - zpp) / (zc - tz[col]);
      double zppscale = (zpp - tz[col]) / (zc - tz[col]);
      double ycscale  =     (yc - ypp) / (yc - ty[col]);
      double yppscale = (ypp - ty[col]) / (yc - ty[col]);
      double xcscale  =     (xc - xpp) / (xc - tx[col]);
      double xppscale = (xpp - tx[col]) / (xc - tx[col]);
      double xp = tx[col] * zcscale + zcx * zppscale;
      double yp = ty[col] * zcscale + zcy * zppscale;
      double xq = tx[col] * ycscale + ycx * yppscale;
      double yq = tz[col] * ycscale + ycz * yppscale;
      double xr = ty[col] * xcscale + xcy * xppscale;
      double yr = tz[col] * xcscale + xcz * xppscale;
      xp *= xscale; yp *= xscale;
      xq *= xscale; yq *= yscale;
    //  xq *= xscale; yq *= (yscale + 2.6) * 0.5;
      xr *= yscale; yr *= yscale;
      int xm = 0, ym = 0, ft = 0;
      if (mode == 1) { xm = (int)xp; ym = (int)yp; ft = (tx[col] < 0.0); }
      if (mode == 2) { xm = (int)xq; ym = (int)yq; ft = (ty[col] < 0.0); }
      if (mode == 3) { xm = (int)xr; ym = (int)yr; ft = (tz[col] < 0.0); }
      int xnew = xpos + xm, ynew = ypos + ym;
      if (phase == 3 && row == tlvl) { // xdata.rows - 1) { // bottom row of region
 //         printf("ROW %d COL %d\n", row, col);
          // order is TL TR BR BL TR.. so 0,1 2,3 6,7 4,5 ...
          // swap TR and BL ... 0,1 4,5 6,7 2,3 ....
          // upside down ... 6,7 2,3 0,1 4,5 ....
          // flip left and right .... 2,3 6,7 4,5 0,1 ....
          // nope that flipped up and down ... so, flip up and down
          // 0,1 4,5 6,7 2,3
        if (col == 0) { trv[4] = xnew; trv[5] = ynew; } // TL [1   ]
        if (col == 1) { trv[0] = xnew; trv[1] = ynew; } // TR [ 2  ]
        if (col == 2) { trv[2] = xnew; trv[3] = ynew; } // BR [   4]
        if (col == 3) { trv[6] = xnew; trv[7] = ynew; } // BL [  3 ]
      }
      if (col == 0) {
        MoveToEx(dc, xnew, ynew, NULL);
        xref = xnew; yref = ynew;
      } else {
//        COLORREF c = blackcolour;
        SelectObject(dc, GetStockObject(DC_PEN));
        if (phase == 1) { // latitude rings
          if (ft) { SetDCPenColor(dc, redcolour); }
          else { SetDCPenColor(dc, blackcolour); }
        } else if (phase == 2) { // longitude rings
          if (ft) { SetDCPenColor(dc, bluecolour); }
          else { SetDCPenColor(dc, greycolour); }
        } else if (phase == 3) { // selected region
          if (tlvl == row) {
            SetDCPenColor(dc, redcolour);
          } else { SetDCPenColor(dc, orangecolour); }
        } else if (phase == 4) { // disc ring
          if (col % 2 == 0) {
            SetDCPenColor(dc, orangecolour);
          } else { SetDCPenColor(dc, blackcolour); }
        } else if (phase == 5) { // disc arm
          if (col % 2 == 0) {
            SetDCPenColor(dc, bluecolour);
          } else { SetDCPenColor(dc, redcolour); }
        }
  //      if (ft) { c = bluecolour; } // SetDCPenColor(dc, bluecolour); }
  //        else { c = blackcolour; } // SetDCPenColor(dc, blackcolour); }
  //      } else if (phase == 2) { // longitude rings
  //        if (ft) { c = redcolour; } // { SetDCPenColor(dc, redcolour); }
  //        else { c = greycolour; } // { SetDCPenColor(dc, greycolour); }
  //      } else if (phase == 3) { // selected region
  //        c = orangecolour; // SetDCPenColor(dc, orangecolour);
  //      }
//        drawLine(dc, xref, yref, xnew, ynew, c);
        LineTo(dc, xnew, ynew);
        xref = xnew; yref = ynew;
      }
    }
  }
}

Pt2 calc2dPt3(Pt3 point, int xpos, int ypos, double xscale, double yscale, char mode) {
  double zcscale  =     (zc - zpp) / (zc - point.z);
  double zppscale = (zpp - point.z) / (zc - point.z);
  double ycscale  =     (yc - ypp) / (yc - point.y);
  double yppscale = (ypp - point.y) / (yc - point.y);
  double xcscale  =     (xc - xpp) / (xc - point.x);
  double xppscale = (xpp - point.x) / (xc - point.x);
  double xp = point.x * zcscale + zcx * zppscale;
  double yp = point.y * zcscale + zcy * zppscale;
  double xq = point.x * ycscale + ycx * yppscale;
  double yq = point.z * ycscale + ycz * yppscale;
  double xr = point.y * xcscale + xcy * xppscale;
  double yr = point.z * xcscale + xcz * xppscale;
  xp *= xscale; yp *= xscale;
  xq *= xscale; yq *= yscale;
  //  xq *= xscale; yq *= (yscale + 2.6) * 0.5;
  xr *= yscale; yr *= yscale;
  int xm = 0, ym = 0, ft = 0;
  if (mode == 1) { xm = (int)xp; ym = (int)yp; ft = (point.x < 0.0); }
  if (mode == 2) { xm = (int)xq; ym = (int)yq; ft = (point.y < 0.0); }
  if (mode == 3) { xm = (int)xr; ym = (int)yr; ft = (point.z < 0.0); }
  ft = ft; // ft is true if point is in positive range of the mode's axis
  Pt2PT2VAR(point2d, xpos + xm, ypos + ym);
  return point2d;
}

void drawPt3ScriptList(HDC dc, int xpos, int ypos, double xscale, double yscale,
             char mode, Pt3ScriptList *psl) {
  COLORREF redcolour       = RGB(255,   0,   0);
  COLORREF yellowcolour    = RGB(255, 255,   0);
  COLORREF greencolour     = RGB(  0, 255,   0);
  COLORREF cyancolour      = RGB(  0, 255, 255);
  COLORREF bluecolour      = RGB(  0,   0, 255);
  COLORREF magentacolour   = RGB(255,   0, 255);
  COLORREF whitecolour     = RGB(250, 250, 250);
  COLORREF lightgreycolour = RGB(200, 200, 200);
  COLORREF darkgreycolour  = RGB(100, 100, 100);
  COLORREF blackcolour     = RGB(  0,   0,   0);
  COLORREF thiscolour      = lightgreycolour;
  Pt3ScriptList *p = psl;
  while (p) {
    char *script = p->script->script;
    Pt3 *points = p->script->points;
    Pt2PT2VAR(lastpt, 0, 0);
    int refpointix = -1;
    int thispointix = 0;
    char *scriptch = script;
    while (1) { // (*scriptch != '\0') {
      //if (*scriptch >= 'A' && *scriptch <= 'Z') {
      if (*scriptch >= '0' && *scriptch <= '9') {
        thispointix = (thispointix * 10) + (*scriptch - '0');
      } else { // if (*scriptch == ',' || *scriptch = ';' || *scriptch == '\0') {
        if      (*scriptch == 'R') { thiscolour = redcolour;       }
        else if (*scriptch == 'Y') { thiscolour = yellowcolour;    }
        else if (*scriptch == 'G') { thiscolour = greencolour;     }
        else if (*scriptch == 'C') { thiscolour = cyancolour;      }
        else if (*scriptch == 'B') { thiscolour = bluecolour;      }
        else if (*scriptch == 'M') { thiscolour = magentacolour;   }
        else if (*scriptch == 'W') { thiscolour = whitecolour;     }
        else if (*scriptch == 'L') { thiscolour = lightgreycolour; }
        else if (*scriptch == 'D') { thiscolour = darkgreycolour;  }
        else if (*scriptch == 'K') { thiscolour = blackcolour;     }
        // ^ allow colour changes to act as delimiters
        Pt2 thispt = calc2dPt3(points[thispointix], xpos, ypos, xscale, yscale, mode);
        if (refpointix == -1) { // first point in list of line points
          MoveToEx(dc, thispt.x, thispt.y, NULL);
          // if (drawpointmarkers) { drawapointmarker; }
          // ^ may draw point marker at point 0 when colour is changed
          //   if (thisisacolour) { add/draw to colour palette ? }
        } else {                // each next point will draw a line
          SelectObject(dc, GetStockObject(DC_PEN));
          SetDCPenColor(dc, thiscolour);
          LineTo(dc, thispt.x, thispt.y);
          // if (drawlinemarkers) { drawalinemarker; }
        }
        lastpt = thispt; // needed for line() but not LineTo()
//        if (*scriptch == ';') { refpointix = -1; } // ; endsline
//        else { refpointix = thispointix; } // colour change acts as ,
        if (*scriptch == ',') { refpointix = thispointix; } // , expects next
        else { refpointix = -1; } // anything else (; \0 colour change) resets
        thispointix = 0; // reset this for next point input
      } // else { invalidcharacterinscript! }
      if (*scriptch == '\0') { break; } // break at end of script
      else { scriptch++; }              // else next script char
    }
    p = p->next;
  }
}
