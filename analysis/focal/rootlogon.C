#include "TStyle.h"

void rootlogon() {

  gROOT->ProcessLine(".include /home/mxr/dev/cern/alpide_systemc/src/Detector/Focal/");
  gROOT->ProcessLine(".include /home/mxr/dev/cern/alpide_systemc/src/Alpide/");
  gROOT->ProcessLine(".include /home/mxr/dev/cern/alpide_systemc/src");
     	// ...
  gStyle->SetCanvasPreferGL(true);
  //gStyle->SetPadGridX(true);
  //gStyle->SetPadGridY(true);
  //gStyle->SetGridColor(kGray);
  gStyle->SetPadTickX(true);
  gStyle->SetPadTickY(true);

  gStyle->SetTitleSize(.05, "XYZ");
  gStyle->SetTitleOffset(1, "Y");
  gStyle->SetTitleOffset(1, "X");


}
