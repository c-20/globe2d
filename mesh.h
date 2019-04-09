
#define PI         3.1415926535897932
#define DEGTORAD   (PI / 180.0)

typedef struct _arr {
  double *v;
  int len;
} arr;

typedef struct _mat {
  double **v;
  int rows, cols;
} mat;

// since a % b does not work for double % double
double doublemod(double a, double b) {
  char isnegative = 0;
  if (a < 0 && b < 0) { a = -a; b = -b; } // -11 / -2 == 11 / 2
  else if (a < 0) { a = -a; isnegative = 1; } // -11 % 2 = -1
  else if (b < 0) { b = -b; isnegative = 1; } // 11 % -2 = -1
  while (a - b >= 0.0) { a -= b; }
  return (isnegative) ? -a : a;
}

void xaxisrotate(mat X, mat Y, mat Z, double spindeg) {
  int r = -1;
  while (++r < X.rows && r < Y.rows && r < Z.rows) {
    int c = -1;
    while (++c < X.cols && c < Y.cols && c < Z.cols) {
      double Yv = Y.v[r][c], Zv = Z.v[r][c];
      X.v[r][c] = X.v[r][c];
      Y.v[r][c] = Yv * cos(spindeg * DEGTORAD) - Zv * sin(spindeg * DEGTORAD);
      Z.v[r][c] = Yv * sin(spindeg * DEGTORAD) + Zv * cos(spindeg * DEGTORAD);
    }
  }
}
void yaxisrotate(mat X, mat Y, mat Z, double flipdeg) {
  int r = -1;
  while (++r < X.rows && r < Y.rows && r < Z.rows) {
    int c = -1;
    while (++c < X.cols && c < Y.cols && c < Z.cols) {
      double Xv = X.v[r][c], Zv = Z.v[r][c];
      X.v[r][c] = Zv * sin(flipdeg * DEGTORAD) + Xv * cos(flipdeg * DEGTORAD);
      Y.v[r][c] = Y.v[r][c];
      Z.v[r][c] = Zv * cos(flipdeg * DEGTORAD) - Xv * sin(flipdeg * DEGTORAD);
    }
  }
}
void zaxisrotate(mat X, mat Y, mat Z, double twistdeg) {
  int r = -1;
  while (++r < X.rows && r < Y.rows && r < Z.rows) {
    int c = -1;
    while (++c < X.cols && c < Y.cols && c < Z.cols) {
      double Xv = X.v[r][c], Yv = Y.v[r][c];
      X.v[r][c] = Xv * cos(twistdeg * DEGTORAD) - Yv * sin(twistdeg * DEGTORAD);
      Y.v[r][c] = Xv * sin(twistdeg * DEGTORAD) + Yv * cos(twistdeg * DEGTORAD);
      Z.v[r][c] = Z.v[r][c];
    }
  }
}

void linspacealloc(double start, double limit, int numsteps, arr *A) {
  A->v = (double *)calloc(numsteps, sizeof(double));
  A->len = numsteps;
  double value = start;
  double increment = (limit - start) / (double)(numsteps - 1);
  int i = -1;
  while (++i < numsteps) {
    A->v[i] = value;
    value += increment;
  }
}

void meshgridalloc(arr X, arr Y, mat *XM, mat *YM) {
  XM->rows = YM->rows = Y.len;
  XM->cols = YM->cols = X.len;
  XM->v = (double **)malloc(sizeof(double *) * XM->rows);
  YM->v = (double **)malloc(sizeof(double *) * YM->rows);
  int yi = -1;
  while (++yi < XM->rows && yi < YM->rows) {
    XM->v[yi] = (double *)malloc(sizeof(double) * XM->cols);
    YM->v[yi] = (double *)malloc(sizeof(double) * YM->cols);
    int xi = -1;
    while (++xi < XM->cols && xi < YM->cols) {
      XM->v[yi][xi] = X.v[xi];
      YM->v[yi][xi] = Y.v[yi];
    }
  }
}
void matalloc(mat *M, int rows, int cols) {
  M->rows = rows;
  M->cols = cols;
  M->v = (double **)calloc(rows, sizeof(double *));
  int yi = -1;
  while (++yi < rows) {
    M->v[yi] = (double *)calloc(cols, sizeof(double));
  }
}
void matcopyalloc(mat X, mat *NX) {
  matalloc(NX, X.rows, X.cols);
  int r = -1;
  while (++r < X.rows) {
    int c = -1;
    while (++c < X.cols) {
      NX->v[r][c] = X.v[r][c];
    }
  }
}
void copyalloc3mats(mat X, mat Y, mat Z, mat *NX, mat *NY, mat *NZ) {
  matcopyalloc(X, NX);
  matcopyalloc(Y, NY);
  matcopyalloc(Z, NZ);
}
void matfree(mat M) {
  int yi = -1;
  while (++yi < M.rows)
    { free(M.v[yi]); }
  free(M.v);
}
void matsetrow(mat M, int rowindex, double *rowvalue) {
  int ci = -1;
  while (++ci < M.cols)
    { M.v[rowindex][ci] = rowvalue[ci]; }
} // assumes rowvalue is same width as M.cols
void freemeshgrid(mat XM, mat YM) {
  matfree(XM);
  matfree(YM);
}
void free3mats(mat XM, mat YM, mat ZM) {
  matfree(XM);
  matfree(YM);
  matfree(ZM);
}
// should not free a fixed array...!!
arr fixedarr(double *values, int length) {
  arr fa;
  fa.v = values;
  fa.len = length;
  return fa;
}
void arralloc(arr *A, int length) {
  A->v = (double *)calloc(length, sizeof(double));
  A->len = length;
}
void arrfree(arr A) {
  free(A.v);
}
void free3arrs(arr A, arr B, arr C) {
  arrfree(A);
  arrfree(B);
  arrfree(C);
}

