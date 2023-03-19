#ifndef FOCALDETECTORANALYSIS
#define FOCALDETECTORANALYSIS


#include <iostream>

#include "FOCALDetectorAnalysis.hpp"
#include "../../src/Detector/Focal/Focal_constants.hpp"
#include "ALPIDE.h"

#include <vector>
#include <string>

#include "TH2Poly.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TChain.h"

#define CANVAS_WIDTH 1024
#define CANVAS_HEIGHT 512

class FOCALDetectorAnalysis {

    private:
    
        std::string fSimFolder;
        
        TFile *fRUFile;
        TFile *fAlpideStatsFile;
        TChain *fPhysicsChain;

        ULong64_t fNEvents;

        std::vector<TH2Poly*> fFOCALPlane_Bare;
        std::vector<TH2Poly*> fFOCALPlane_Occupancy;
        std::vector<TH2Poly*> fFOCALPlane_Hits;
        std::vector<TH2Poly*> fFOCALPlane_Datarate;
        std::vector<TH2Poly*> fFOCALPlane_ChipModes;
        std::vector<TH2Poly*> fFOCALPlane_ChipIDs;
        void CreateFOCALChipBins(TH2Poly* th2);

        void FillChipIDs();

        TCanvas *cBare;
        TCanvas *cChipIDs;
        TCanvas *cHits;
        TCanvas *cOcc;

        void init();

    public:
        FOCALDetectorAnalysis(){init();};
        FOCALDetectorAnalysis(std::string simfolder, ULong64_t nevents) :fSimFolder(simfolder), fNEvents(nevents){init();};
        void Draw();
        void FillHitmap();


        void SetNEvents(ULong64_t number_of_events)         {fNEvents=number_of_events;}
        ULong64_t GetNEvents(ULong64_t number_of_events)    {return fNEvents;}
        void SetSimFolder(std::string sim_folder)                {fSimFolder=sim_folder;}
        std::string GetSimFolder(ULong64_t number_of_events)      {return fSimFolder;}


};

void FOCALDetectorAnalysis::FillHitmap(){
    if(fSimFolder.length()==0){
        std::cerr << "FOCALDetectorAnalysis: simulation folder not set. " << std::endl;
        return; 
    }

    fPhysicsChain = new TChain("mPhysicsEvents");
    fPhysicsChain->Add(Form("%s/PhysicsEventData*.root", fSimFolder.c_str()));
    if(!fPhysicsChain->GetEntries()){
        std::cerr << "FOCALDetectorAnalysis: Could not open physics event data file. " << std::endl;
        return; 
    }
    fPhysicsChain->Print();
    std::cout << fPhysicsChain->GetEntries() << std::endl;

    std::vector<int> *n = new std::vector<int>(); 
    std::vector<int> *id = new std::vector<int>(); 

    fPhysicsChain->SetBranchStatus("*", 0);
    fPhysicsChain->SetBranchStatus("nHits", 1);
    fPhysicsChain->SetBranchStatus("chipId", 1);

    fPhysicsChain->SetBranchAddress("nHits", &n);
    fPhysicsChain->SetBranchAddress("chipId", &id);

    std::vector<Long64_t> NChipHits(Focal::CHIPS, 0);
    
    std::cout << "Reading physics chain " << fPhysicsChain->GetName() <<"...";
    for(unsigned int e=0;e<fPhysicsChain->GetEntries();e++){
        fPhysicsChain->GetEntry(e);
        unsigned int nevents = TMath::Min(n->size(),id->size());
        for(unsigned int i=0;i<nevents;i++){
            if( (unsigned int) id->at(i) < NChipHits.size() ) {
                NChipHits.at(id->at(i)) += n->at(i);
            }
        }
    }
    std::cout<<"done."<<std::endl;

    delete n;
    delete id;

    for(unsigned int i=0;i<NChipHits.size();i++){
        int id_layer = i % Focal::CHIPS_PER_LAYER;
        int layer = i/Focal::CHIPS_PER_LAYER;
        fFOCALPlane_Hits.at(layer)->SetBinContent(id_layer+1, NChipHits.at(i));
        fFOCALPlane_Occupancy.at(layer)->SetBinContent(id_layer+1, NChipHits.at(i));
    }



    if(fNEvents) {
        fFOCALPlane_Hits.at(0)->Scale(1./fNEvents);
        fFOCALPlane_Hits.at(1)->Scale(1./fNEvents);

        fFOCALPlane_Occupancy.at(0)->Scale(1./fNEvents);
        fFOCALPlane_Occupancy.at(1)->Scale(1./fNEvents);

        fFOCALPlane_Occupancy.at(0)->Scale(1./((double) (N_PIXEL_COLS*N_PIXEL_ROWS)));
        fFOCALPlane_Occupancy.at(1)->Scale(1./((double) (N_PIXEL_COLS*N_PIXEL_ROWS)));
    }


}

void FOCALDetectorAnalysis::Draw(){
    cBare = new TCanvas("cBare", "Bare Pixel Planes", CANVAS_WIDTH*1.1, CANVAS_HEIGHT);
    cBare->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBare->cd(l+1);
        fFOCALPlane_Bare.at(l)->Draw();
    }

    cChipIDs = new TCanvas("cChipIds", "Chip IDs Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cChipIDs->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cChipIDs->cd(l+1);
        gPad->SetRightMargin(0.15);
        fFOCALPlane_ChipIDs.at(l)->Draw("COLZ L");
    }


    cHits = new TCanvas("cHits", "Chip Hits Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cHits->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cHits->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Hits.at(l)->Draw("COLZ L");
    }

    cOcc = new TCanvas("cOcc", "Chip Ocupancy Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cOcc->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cOcc->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Occupancy.at(l)->Draw("COLZ L");
    }

}

void FOCALDetectorAnalysis::FillChipIDs(){
    for(unsigned int i=0;i<Focal::CHIPS;i++){
        const int l = i/Focal::CHIPS_PER_LAYER;
        fFOCALPlane_ChipIDs.at(l)->SetBinContent(1+i%Focal::CHIPS_PER_LAYER, i);
    }
    fFOCALPlane_ChipIDs.at(0)->SetMinimum(0);
    fFOCALPlane_ChipIDs.at(1)->SetMinimum(0);
    fFOCALPlane_ChipIDs.at(0)->SetMaximum(Focal::CHIPS);
    fFOCALPlane_ChipIDs.at(1)->SetMaximum(Focal::CHIPS);
}


void FOCALDetectorAnalysis::init(){

    TH2Poly *hpoly;
    for(unsigned int l=0;l<=1;l++){

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_bare_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Plane %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Bare.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_chipids_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Chip IDs Plane %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_ChipIDs.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_chipmodes_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Chip Modes Plane  %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_ChipModes.push_back(hpoly);
        
        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_occupancy_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Occupancy Plane %d;x (mm);y (mm);Average occupancy / chip / event", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Occupancy.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);        
        hpoly->SetName(Form("focalplane_datarate_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Datarate Plane %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Datarate.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_hits_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Hits Plane %d;x (mm);y (mm);Average hits / chip / event", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Hits.push_back(hpoly);

    }

    FillChipIDs();

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
	        //std::cout << quadrant << ", "<< half_patch_num<<", "<<stave_num_in_half_patch<<": "<< y_low << "\t" << Focal::HALF_PATCH_SIZE_Y_MM << std::endl;
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

#endif
