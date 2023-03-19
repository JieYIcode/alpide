#include "FOCALDetectorAnalysis.hpp"

void analysis(){
    FOCALDetectorAnalysis *analysis = new FOCALDetectorAnalysis("../../sim_output/run_0/", 1000);
    
    analysis->FillHitmap();
    
    analysis->Draw();
}