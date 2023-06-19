#include <iostream>

#include "FOCALDetectorAnalysis.hpp"
#include "../../src/Detector/Focal/Focal_constants.hpp"
#include "ALPIDE.h"

FOCALDetectorAnalysis::FOCALDetectorAnalysis(){

    for(unsigned int l=0;l<=1;l++){
        CreateFOCALChipBins(fFOCALPlane_Bare.at(l));
        CreateFOCALChipBins(fFOCALPlane_Occupancy.at(l));
        CreateFOCALChipBins(fFOCALPlane_Datarate.at(l));
        CreateFOCALChipBins(fFOCALPlane_Hits.at(l));
    }

}

///@brief Create the polygons/bins for the chips in the Focal
///       detector plane on a TH2Poly 2d-histogram object
///@param th2 Pointer to TH2Poly instance to create the polygons/bins for
void FOCALDetectorAnalysis::CreateFOCALChipBins(TH2Poly* th2)
{
  for(unsigned int quadrant = 1; quadrant <= 4; quadrant++) {
    for(unsigned int half_patch_num = 0; half_patch_num < Focal::HALF_PATCHES_PER_QUADRANT; half_patch_num++) {
      for(unsigned int stave_num_in_half_patch = 0;
          stave_num_in_half_patch < Focal::STAVES_PER_HALF_PATCH;
          stave_num_in_half_patch++) {
        
        unsigned int stave_num = half_patch_num*Focal::STAVES_PER_HALF_PATCH+stave_num_in_half_patch;

        double y_low = stave_num*ALPIDE::CHIPSIZE_Y_MM;
        double y_high = y_low+ALPIDE::CHIPSIZE_Y_MM;
        double x_start = 0;

        if(y_low < Focal::HALF_PATCH_SIZE_Y_MM){
	        std::cout << quadrant << ", "<< half_patch_num<<", "<<stave_num_in_half_patch<<": "<< y_low << "\t" << Focal::HALF_PATCH_SIZE_Y_MM << std::endl;
          x_start = Focal::GAP_SIZE_X_MM/2;
	      }

        for(unsigned int chip_num = 0; chip_num < Focal::CHIPS_PER_STRING; chip_num++) {
          double x_low = x_start + chip_num*ALPIDE::CHIPSIZE_X_MM;
          double x_high = x_low+ALPIDE::CHIPSIZE_X_MM;

          if(quadrant == 1) {
            th2->AddBin(x_low+Focal::SHIFT_X_MM, y_low, x_high+Focal::SHIFT_X_MM, y_high);
          } else if(quadrant == 2) {
            th2->AddBin(-x_low-Focal::SHIFT_X_MM, y_low, -x_high-Focal::SHIFT_X_MM, y_high);
          } else if(quadrant == 3) {
            th2->AddBin(-x_low-Focal::SHIFT_X_MM, -y_low, -x_high-Focal::SHIFT_X_MM, -y_high);
          } else {
            th2->AddBin(x_low+Focal::SHIFT_X_MM, -y_low, x_high+Focal::SHIFT_X_MM, -y_high);
          }
        }
      }
    }
  }
}