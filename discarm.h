    if (1) { // if displaydiscarm
      // disc rotation angle applied later
      int ri = -1;
      while (++ri < DiX.cols && ri < DiY.cols && ri < DiZ.cols) {
          DiY.v[0][ri] += GETVARVALUE(discfloorz);
          DiY.v[1][ri] += GETVARVALUE(discfloorz);
      }
      ri = -1;
      while (++ri < ArX.cols) {
        ArY.v[0][ri] += GETVARVALUE(discfloorz);
      } // all arm sections are relative to disc floor
      ArX.v[0][1] *= GETVARVALUE(discarmAlength) / discamplitude;
      ArZ.v[0][1] *= GETVARVALUE(discarmAlength) / discamplitude;
////      ArY.v[0][2] -= segBop todo: build arm
    }
