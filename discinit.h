// disc is a copy of the equatorial long ring
  if (1) { // displaydisc
    matalloc(&discX, 2, Xya.rows);
    matalloc(&discY, 2, Yya.rows);
    matalloc(&discZ, 2, Zya.rows);
    int ri = -1;
    while (++ri < discX.cols && ri < discY.cols && ri < discZ.cols) {
      discX.v[0][ri]  = Zya.v[ri][latNtoSmiddle] * sin(YAz.v[ri][latNtoSmiddle] * DEGTORAD);
      discX.v[0][ri] += Xya.v[ri][latNtoSmiddle] * cos(YAx.v[ri][latNtoSmiddle] * DEGTORAD);
      discY.v[0][ri]  = -globeamplitude; // align with top // Yya.v[ri][latNtoSmiddle];
      discZ.v[0][ri]  = Zya.v[ri][latNtoSmiddle] * cos(YAz.v[ri][latNtoSmiddle] * DEGTORAD);
      discZ.v[0][ri] -= Xya.v[ri][latNtoSmiddle] * sin(YAx.v[ri][latNtoSmiddle] * DEGTORAD);
      discX.v[1][ri]  = discX.v[0][ri] * discamplitude / globeamplitude;
      discY.v[1][ri]  = discY.v[0][ri]; // * discamplitude / amplitude;
      discZ.v[1][ri]  = discZ.v[0][ri] * discamplitude / globeamplitude;
//      printf("discXYZ: %f, %f, %f\n", )
    } // create disc ring - Z and X spin around XZ (Y Angle)
//    matalloc(&armX, 1, 5);
//    matalloc(&armY, 1, 5);
//    matalloc(&armZ, 1, 5);
    // does not have to be at 0 degrees ......
    Pt3PT3VAR(discrangestart, discX.v[0][0], discY.v[0][0], discZ.v[0][0]);
    Pt3PT3VAR(discrangestop,  discX.v[1][0], discY.v[1][0], discZ.v[1][0]);
    VectorVECTORVAR(discradius, discrangestart, discrangestop);

    Pt3 segAstart = Pt3pctalongVector(discradius, discstartmarginpct);
    Pt3 segAstop  = Pt3pctalongVector(discradius, 1.0 - discstopmarginpct);
    VectorVECTORVAR(segArange, segAstart, segAstop);
    // == SEGMENT A ======================================================
    segmentscripts = SIZEMEM(Pt3Script, 6); // segAbox + 2 actuators w/ box + segBbox
    double segAactAlensum = segmentAoffsetlen;
    Pt3 segAactAstart = Pt3offsetalongVector(segArange, segAactAlensum);
    segAactAlensum += segmentAactAbotlen;
    Pt3 segAactAboxstart = Pt3offsetalongVector(segArange, segAactAlensum);
    segAactAlensum += segmentAactAboxlen;
    Pt3 segAactAboxstop = Pt3offsetalongVector(segArange, segAactAlensum);
    segAactAlensum += segmentAactAvrangelen; // * VARVAL(pctextended)
    Pt3 segAactAextstop = Pt3offsetalongVector(segArange, segAactAlensum);
    segAactAlensum += segmentAactAtoplen;
    Pt3 segAactAstop = Pt3offsetalongVector(segArange, segAactAlensum);
    VectorVECTORVAR(segAactA, segAactAstart, segAactAstop);
    Pt3Script5(segAactAscript, segmentAactuatorA, normalactuatorscript, segAactAstart,
                     segAactAboxstart, segAactAboxstop, segAactAextstop, segAactAstop);
    segmentscripts[0] = segAactAscript;
    VectorVECTORVAR(segAactAbox, segAactAboxstart, segAactAboxstop);
    segmentscripts[1] = Pt3Scriptxzbox(segAactAbox, segmentAactuatorAbox,
                                       segmentAactAboxwidth, segmentAactAboxheight);

    Pt3 segAactBstart = Pt3yoffset(segAactAstart, -segmentAangleheight);
    double segAactBlensum = segmentAactBbotlen + segmentAactBboxlen +
                            segmentAactBvrangelen + segmentAactBtoplen;
    Pt3 segBstart = segAactAstop; // segment B starts at top pivot
    // Pt3 segBstop derived from offset using segAactBstop
    double segAactABlen = hypUsingPythagoras(segmentAangleheight, segAactA.length);
    double segABactABangle = angleUsingLawOfCosines(segAactABlen, segmentBoffsetlen, segAactBlensum);
    double segAactABangle = angleTanOppAdj(segmentAangleheight, segAactA.length);
    double segAsegBangle = segAactABangle + segABactABangle;
    double segAsegBriseangle = 180.0 - segAsegBangle;
    Pt3 segBactAstart = Pt3angledoffsetafterVector(segAactA, segAsegBriseangle, segmentBoffsetlen);
//    double segAsegBdistanceout = adjFromHyp(segmentBoffsetlen, segAsegBriseangle);
//    double segAsegBdistanceup = oppFromHyp(segmentBoffsetlen, segAsegBriseangle);
//    Pt3 segBactAstart = Pt3offsetalongVector(segAactA, segAactA.length + segAsegBdistanceout);
//    segBactAstart = Pt3yoffset(segBactAstart, -segAsegBdistanceup);

    VectorVECTORVAR(segAactB, segAactBstart, segBactAstart); // segAactB stops at segBactA
    double segAactBlensum2 = segmentAactBbotlen;
    Pt3 segAactBboxstart = Pt3offsetalongVector(segAactB, segAactBlensum2);
    segAactBlensum2 += segmentAactBboxlen;
    Pt3 segAactBboxstop = Pt3offsetalongVector(segAactB, segAactBlensum2);
    segAactBlensum2 += segmentAactBvrangelen; // VARVAL()
    Pt3 segAactBextstop = Pt3offsetalongVector(segAactB, segAactBlensum2);
    segAactBlensum2 += segmentAactBtoplen;
    Pt3 segAactBstop = Pt3offsetalongVector(segAactB, segAactBlensum2);
    Pt3Script5(segAactBscript, segmentAactuatorB, normalactuatorscript, segAactBstart,
                     segAactBboxstart, segAactBboxstop, segAactBextstop, segAactBstop);
    segmentscripts[2] = segAactBscript;
    VectorVECTORVAR(segAactBbox, segAactBboxstart, segAactBboxstop);
    segmentscripts[3] = Pt3Scriptxzbox(segAactBbox, segmentAactuatorBbox,
                                       segmentAactBboxwidth, segmentAactBboxheight);

    segmentscripts[4] = Pt3Scriptxzbox(segArange, segmentAbox,
                                       segmentAboxwidth, segmentAboxheight);
    VectorVECTORVAR(segBactAoffset, segAactAstop, segAactBstop);
    Pt3 segBstop = Pt3offsetalongVector(segBactAoffset, segmentBboxlength);
    VectorVECTORVAR(segBrange, segBstart, segBstop);
    segmentscripts[5] = Pt3Scriptxzbox(segBrange, segmentBbox,
                                       segmentBboxwidth, segmentBboxheight);

    segments = SIZEMEM(Pt3ScriptList, 6);
    segments[0].script = &segmentscripts[0];
    segments[1].script = &segmentscripts[1];
    segments[2].script = &segmentscripts[2];
    segments[3].script = &segmentscripts[3];
    segments[4].script = &segmentscripts[4];
    segments[5].script = &segmentscripts[5];
    segments[0].next = &segments[1];
    segments[1].next = &segments[2];
    segments[2].next = &segments[3];
    segments[3].next = &segments[4];
    segments[4].next = &segments[5];
    segments[5].next = NULL;

/*
//    segAoffactscript.
//    Pt3 segApos = Pt3pctalongvector(segArange, VARVAL(segAoffsetpct));
//    VectorVECTORVAR(segAoffset, segAstart, segApos);
//    Pt3 segAtoppos = Pt3yoffset(segApos, -segmentAheight);
//    Pt3 segAseriflen = Pt3pctalongvector(segArange, 1.0 - segserifwidthpct);
//    Pt3 segAseriftop = Pt3yrotatearoundref(segAseriflen,  90.0, segAtoppos);
//    Pt3 segAserifbot = Pt3yrotatearoundref(segAseriflen, -90.0, segAtoppos);
    Pt3Script6(segA, segmentAscript, segAstop, segAstart, segApos,
                           segAtoppos, segAserifbot, segAseriftop);
    Pt3 segBstart = segAtoppos;
//    Pt3 segBstop =
    segmentAbox = Pt3Scriptxzbox(segArange, segmentAwidth, segmentAheight);
    segmentA = segA; //segmentAbox = segAbox; // set globals
//    Pt3ScriptList armseg0box, armseg0vec; <-- globals
    armseg0box.script = &segmentAbox; // global for persistence
    armseg0box.next = &armseg0vec; // this is a global
    armseg0vec.script = &segmentA; // global for persistence
    armseg0vec.next = NULL;        // this is the end of the list
*/
//    armX.v[0][0] = discX.v[0][0];
//    armY.v[0][0] = discY.v[0][0];
//    armZ.v[0][0] = discZ.v[0][0];
//    armX.v[0][1] = discX.v[1][0];
//    armY.v[0][1] = discY.v[1][0];
//    armZ.v[0][1] = discZ.v[1][0];

  }
