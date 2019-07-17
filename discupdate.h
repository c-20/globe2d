
    if (1) { // if displaydiscarm
      // disc rotation angle applied later
      // ----- apply disc distance offset to disc and arm -----------
      double discdistance = GETVARVALUE(discfloorz);
      int ri = -1;
      while (++ri < DiX.cols) { // && ri < DiY.cols && ri < DiZ.cols)
        DiY.v[0][ri] += discdistance;
        DiY.v[1][ri] += discdistance;
      }
      // can only access one previous list item
      // otherwise add ->prev pointers or access as array or >>>>>> access by name <<<<<<
      Pt3ScriptList *p = seglist;
      int segindex = -1;
      // first pass: recalculate points
      while (p) {
        ++segindex;
        Pt3Script *s = p->script;
//        int numpoints = 0;
        //int scriptlen =
//        Pt3Scriptnumpoints(s, &numpoints);
        if (segindex == 0) { // SEGMENT A ACTUATOR
                // ^^ now just indexed .... seg A range is the first script
          Pt3 segAactAextstart = s->points[2];
          Pt3 segAactAextstop = s->points[3];
          VectorVECTORVAR(segAactAextrange, segAactAextstart, segAactAextstop);
          Pt3 segAactApos = Pt3pctalongVector(segAactAextrange, VARVAL(segAactAextpct));
          Pt3 segAactAstart = s->points[0];
          VectorVECTORVAR(segAactAoffset, segAactAstart, segAactApos);
          //Pt3 segAactAboxstart = s->points[1];
          double segAactAexttoppos = segAactAoffset.length + segmentAactAtoplen;
          Pt3 segAactAtoppos = Pt3offsetalongVector(segAactAoffset, segAactAexttoppos);
          s->points[3] = segAactApos;      // exttop becomes % of range
          s->points[4] = segAactAtoppos;   // toppos moves with pos
        } else if (segindex == 1) { // SEGMENT A ACTUATOR A BOX (does not move)
        // free segscripts[1] from init
//          segmentscripts[1] = Pt3Scriptxzbox(segAactAbox, segmentAactAboxwidth, segmentAactAboxheight);
        } else if (segindex == 2) { // SEGMENT A ACTUATOR B
            Pt3Script *SPsegAactA = Pt3ScriptListlookupscriptname(seglist, segmentAactuatorA);
            if (!SPsegAactA) { printf("Actuator B can't find Actuator A\n"); break; }
            VectorVECTORVAR(segAactA, SPsegAactA->points[0], SPsegAactA->points[4]);
            Pt3 segAactBstart = s->points[0]; // does not move .. add prior joint otherwisec
            double segAactBvaluelen = segmentAactBvrangelen * VARVAL(segAactBextpct);
            double segAactBlensum = segmentAactBbotlen + segmentAactBboxlen +
                                    segAactBvaluelen + segmentAactBtoplen;
            double segAactABlen = hypUsingPythagoras(segmentAangleheight, segAactA.length);
            double segAactABriseangle = angleTanOppAdj(segmentAangleheight, segAactA.length);
            double segABactABriseangle = angleUsingLawOfCosines(segAactABlen, segmentBoffsetlen, segAactBlensum);
            double segAsegBriseangle = segAactABriseangle + segABactABriseangle; // REVERSED!!
            double segAsegBouterriseangle = 180.0 - segAsegBriseangle; // angle between seg A and seg B
            Pt3 segAactBstop = Pt3angledoffsetafterVector(
                                    segAactA, segAsegBouterriseangle, segmentBoffsetlen);
            s->points[4] = segAactBstop; // segAactBstop !!
            VectorVECTORVAR(segAactB, segAactBstart, segAactBstop);
            double segAactBlensum2 = segmentAactBbotlen;
            s->points[1] = Pt3offsetalongVector(segAactB, segAactBlensum2);
            segAactBlensum2 += segmentAactBboxlen;
            s->points[2] = Pt3offsetalongVector(segAactB, segAactBlensum2);
            segAactBlensum2 += segAactBvaluelen; // .length; // segmentAactBvrangelen; // VARVAL()
            s->points[3] = Pt3offsetalongVector(segAactB, segAactBlensum2);
            segAactBlensum2 += segmentAactBtoplen;
//            Pt3 segAactBstop = Pt3offsetalongVector(segAactB, segAactBlensum2);
// ^ this should match segAactBstop / segBactAstart
//            Pt3Script5(segAactBscript, normalactuatorscript, segAactBstart,
//                            segAactBboxstart, segAactBboxstop, segAactBextstop, segAactBstop);
       //     segmentscripts[2] = segAactBscript;
        // ^ update segment script rather than realloc
        } else if (segindex == 3) { // SEGMENT A ACTUATOR B BOX

          Pt3Script *SPsegAactB = Pt3ScriptListlookupscriptname(seglist, segmentAactuatorB);
          if (!SPsegAactB) { printf("Actuator A Box can't find Actuator B\n"); break; }
          VectorVECTORVAR(segAactBbox, SPsegAactB->points[1], SPsegAactB->points[2]);
// need to recalculate box points
//////////////          VectorVECTORVAR(segAactBbox, segAactBboxstart, segAactBboxstop);
//          segmentscripts[3] = Pt3Scriptxzbox(segAactBbox, segmentAactBboxwidth, segmentAactBboxheight);
          Pt3Scriptfreepoints(segmentscripts[3]);
          segmentscripts[3] = Pt3Scriptxzbox(segAactBbox, segmentAactuatorBbox,
                                             segmentAactBboxwidth, segmentAactBboxheight);
        } else if (segindex == 4) { // SEGMENT A BOX (does not change)
//          Pt3Scriptfreepoints(segmentscripts[4]);
//          segmentscripts[4] = Pt3Scriptxzbox(segArange, segmentAboxwidth, segmentAboxheight);
        } else if (segindex == 5) { // SEGMENT B BOX (moves in 3D)
          Pt3Script *SPsegAactA = Pt3ScriptListlookupscriptname(seglist, segmentAactuatorA);
          if (!SPsegAactA) { printf("Segment B Box can't find Segment A Actuator A\n"); break; }
          Pt3Script *SPsegAactB = Pt3ScriptListlookupscriptname(seglist, segmentAactuatorB);
          if (!SPsegAactB) { printf("Segment B Box can't find Segment A Actuator B\n"); break; }
          Pt3 segAactAstop = SPsegAactA->points[4];
          Pt3 segAactBstop = SPsegAactB->points[4];
          VectorVECTORVAR(segBactAoffset, segAactAstop, segAactBstop);
          Pt3 segBstart = segAactAstop;
          Pt3 segBstop = Pt3offsetalongVector(segBactAoffset, segmentBboxlength);
          VectorVECTORVAR(segBrange, segBstart, segBstop);
          Pt3Scriptfreepoints(segmentscripts[5]);
          segmentscripts[5] = Pt3Scriptxzbox(segBrange, segmentBbox,
                                             segmentBboxwidth, segmentBboxheight);
        }
        p = p->next;
      }
      // second pass: offset disc height
      p = seglist;
      segindex = -1;
      while (p) {
        ++segindex;
        Pt3Script *s = p->script;
        int numpoints = 0;
        //int scriptlen =
        Pt3Scriptnumpoints(s, &numpoints);
        int ptix = -1;
        while (++ptix < numpoints) {
//          s->points[ptix] = Pt3yrotatearoundref(s->points[ptix], flipdeg, yaxis);
          s->points[ptix].y += discdistance;
        }
        p = p->next;
      }
    /*
      ri = -1;
      while (++ri < ArX.cols)
        { ArY.v[0][ri] += GETVARVALUE(discfloorz); }

      double segAposratio = GETVARVALUE(discsegApos) / discamplitude;
      double segAlenratio = GETVARVALUE(discsegAlen) / discamplitude;

      double segBposratio = GETVARVALUE(discsegBpos) / discamplitude;
      double segBlenratio = GETVARVALUE(discsegBlen) / discamplitude;

      double segClenratio = GETVARVALUE(discsegClen) / discamplitude;
      ArX.v[0][1] *= segAlengthratio;
      ArX.v[0][1] *= segAlengthratio;

////      ArY.v[0][2] -= segBop todo: build arm
*/
    }
