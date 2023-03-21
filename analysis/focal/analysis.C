#include "FOCALDetectorAnalysis.hpp"

void analysis(){

    FOCALDetectorAnalysis *analysis = new FOCALDetectorAnalysis("/alf/data/focal/alpide_systemc/sim_output/run_0/", 25000);
    
    analysis->FillHitmap();
    analysis->AnalyseRULinks();
    
    analysis->Draw();
}
