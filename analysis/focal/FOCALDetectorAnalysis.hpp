#ifndef FOCALDETECTORANALYSIS
#define FOCALDETECTORANALYSIS

#define FOCAL_PIXEL0_Z_MM 7044
#define FOCAL_PIXEL1_Z_MM 7089

#include <iostream>

#include "FOCALDetectorAnalysis.hpp"
#include "../../src/Detector/Focal/Focal_constants.hpp"
#include "utils/UiBColors.cc"
#include "ALPIDE.h"

#include <vector>
#include <string>
#include <map>

#include "TH2Poly.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TChain.h"
#include "TMath.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TEllipse.h"

#define CANVAS_WIDTH 1024
#define CANVAS_HEIGHT 512

class FOCALDetectorAnalysis {

    private:
    
        std::string fSimFolder;
        
        TFile *fRUFile;
        TFile *fAlpideStatsFile;
        TChain *fPhysicsChain;

        TEllipse *fEtaEllipse;
        TLatex *fEtaText;

        ULong64_t fNEvents;

        std::vector<TH2Poly*> fFOCALPlane_Bare;
        std::vector<TH2Poly*> fFOCALPlane_Occupancy;
        std::vector<TH2Poly*> fFOCALPlane_Hits;
        std::vector<TH2Poly*> fFOCALPlane_Datarate;
        std::vector<TH2Poly*> fFOCALPlane_ChipModes;
        std::vector<TH2Poly*> fFOCALPlane_ChipIDs;
        std::vector<TH2Poly*> fFOCALPlane_BusyV;
        std::vector<TH2Poly*> fFOCALPlane_Busy;
        std::vector<TH2Poly*> fFOCALPlane_FrameEfficiency;

        std::vector<int> fChipModes;
        std::vector<double> fChipRadius;

        //this map links the RU data links to the global chip id bins
        // map[RU, link] => {global chip id, nchips}
        std::vector< std::map< std::pair<int,int> , std::pair<int, int>> > fRULink2GlobalBin;

        void CreateFOCALChipBins(TH2Poly* th2);

        void FillChipIDs();
        void initChipMode();
        void initRULinks();
        void analyseRULinks(int layer, int ru, int mode /*true:: inner, false: outer*/);

        int ChipMode(int chipid);

        TCanvas *cBare;
        TCanvas *cChipIDs;
        TCanvas *cHits;
        TCanvas *cOcc;
        TCanvas *cChipModes;
        TCanvas *cDataRate;
        TCanvas *cBusyV;
        TCanvas *cBusy;
        TCanvas *cFrameEff;

        void init();

    public:
        FOCALDetectorAnalysis(){init();};
        FOCALDetectorAnalysis(std::string simfolder, ULong64_t nevents) :fSimFolder(simfolder), fNEvents(nevents){
            gSystem->Exec(Form("mkdir %s/plots", fSimFolder.c_str())); 
            init();
        };

        void Draw();
        void FillHitmap();
        void AnalyseRULinks();

        void DrawEtaCircles(TCanvas *canvas, double eta_min, double eta_max, double delta_eta);
        void DrawEtaCircle(int layer, double eta);


        void SetNEvents(ULong64_t number_of_events)         {fNEvents=number_of_events;}
        ULong64_t GetNEvents(ULong64_t number_of_events)    {return fNEvents;}
        void SetSimFolder(std::string sim_folder)           {fSimFolder=sim_folder;}
        std::string GetSimFolder()                          {return fSimFolder;}


};

void FOCALDetectorAnalysis::initChipMode(){

    // init the chip modes
    // 0: Inner barrel mode chip
    // 1: Outer barrel mode master chip
    // 2: Outer barrel mode slave chip

    for(unsigned int i=0;i<Focal::CHIPS;i++){
        const int l = i/Focal::CHIPS_PER_LAYER;
        int imode =  ChipMode(i);
        fFOCALPlane_ChipModes.at(l)->SetBinContent(1+i%Focal::CHIPS_PER_LAYER, imode);
        fChipModes.push_back(imode);
    }
}

void FOCALDetectorAnalysis::analyseRULinks(int layer, int ru, int mode /*true:: inner, false: outer*/){
    int NLinks;
    if(mode) NLinks = Focal_I3_I3_O3_O6::DATA_LINKS;
    else NLinks = Focal_O3_O3_O3_O3_O3::DATA_LINKS;

    std::vector<UInt_t> NEntries = std::vector<UInt_t>(NLinks, 0);
    std::vector<ULong_t> DataRates = std::vector<ULong_t>(NLinks, 0);

    TFile *fRUEvents = new TFile(Form("%s/RU_%d_%d_ru_events.root", fSimFolder.c_str(), layer, ru), "READ");
    if(! fRUEvents->IsOpen()){
        std::cerr << "Could not open RU link file "<<fRUEvents->GetName()<<std::endl; return;
    } else {
        std::cout << "Reading " <<fRUEvents->GetName() <<std::endl;
    }

    TTree *dataratetree = (TTree*) fRUEvents->Get("DATA_RATE");
    if(!dataratetree) {
        std::cerr << "Could not load DATA_RATE tree" << std::endl;
        return;
    }

    UInt_t link;
    ULong_t rate;
    dataratetree->SetBranchStatus("*",0);
    dataratetree->SetBranchStatus("link", 1);
    dataratetree->SetBranchStatus("rate", 1);
    dataratetree->SetBranchAddress("rate", &rate);
    dataratetree->SetBranchAddress("link", &link);
    for(unsigned int e=0;e<dataratetree->GetEntries();e++){
        dataratetree->GetEntry(e);
        if(link<NEntries.size() && link<DataRates.size()){
            NEntries.at(link)++;
            DataRates.at(link)+=rate;
        }
    }

    for(unsigned int i=0;i<NLinks;i++){
        float datarate = DataRates.at(i) / (float) NEntries.at(i);
        for(unsigned int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++){
            unsigned int gbin = fRULink2GlobalBin.at(layer)[{ru, i}].first+_chips;
            gbin -= Focal::CUMULATIVE_CHIP_COUNT_AT_LAYER[layer];
            fFOCALPlane_Datarate.at(layer)->SetBinContent(1+gbin, datarate);
        }
    }

    TTree *busyvtree = (TTree*) fRUEvents->Get("BUSYV");
    if(!busyvtree) {
        std::cerr << "Could not load BUSYV tree" << std::endl;
        return;
    }

    std::vector<UInt_t> NBusyV = std::vector<UInt_t>(NLinks, 0);
    ULong_t busyvlink;
    unsigned char busyvchipid;
    busyvtree->SetBranchAddress("linkId", &busyvlink);
    busyvtree->SetBranchAddress("chipId", &busyvchipid);
    for(unsigned int e=0;e<busyvtree->GetEntries();e++){
        busyvtree->GetEntry(e);
        NBusyV.at((int) busyvlink)++;
    }
    
    for(unsigned int i=0;i<NLinks;i++){
        for(unsigned int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++){
            unsigned int gbin = fRULink2GlobalBin.at(layer)[{ru, i}].first + _chips;
            gbin -= Focal::CUMULATIVE_CHIP_COUNT_AT_LAYER[layer];
            fFOCALPlane_BusyV.at(layer)->SetBinContent(1+gbin, ((double)NBusyV.at(i))/fNEvents);
            fFOCALPlane_FrameEfficiency.at(layer)->SetBinContent(1+gbin, (double) ( (Int_t) (fNEvents-NBusyV.at(i)) )/( (double) fNEvents));
            //std::cout <<  ( (Int_t) (fNEvents-NBusyV.at(i)) )/( (double) fNEvents)<< std::endl;
        }
    }

    TTree *busytree = (TTree*) fRUEvents->Get("BUSY");
    if(!busytree) {
        std::cerr << "Could not load BUSY tree" << std::endl;
        return;
    }

    std::vector<UInt_t> NBusy = std::vector<UInt_t>(NLinks, 0);
    ULong_t busylink;
    busytree->SetBranchAddress("linkId", &busylink);
    for(unsigned int e=0;e<busytree->GetEntries();e++){
        busytree->GetEntry(e);
        NBusy.at((int) busylink)++;
    }
    
    for(unsigned int i=0;i<NLinks;i++){
        for(unsigned int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++){
            unsigned int gbin = fRULink2GlobalBin.at(layer)[{ru, i}].first + _chips;
            gbin -= Focal::CUMULATIVE_CHIP_COUNT_AT_LAYER[layer];
            fFOCALPlane_Busy.at(layer)->SetBinContent(1+gbin,  ((double)NBusy.at(i))/fNEvents);
        }
    }

    fRUEvents->Close();
    delete fRUEvents;
}

void FOCALDetectorAnalysis::AnalyseRULinks(){
    for(int layer=0;layer<2;layer++){
        for(unsigned int ru=0;ru<4*Focal::STAVES_PER_QUADRANT;ru++){
            bool mode = (ru%Focal::STAVES_PER_QUADRANT < Focal::INNER_STAVES_PER_QUADRANT);
            analyseRULinks(layer, ru, mode);
        }
    }
}

void FOCALDetectorAnalysis::initRULinks(){
    fRULink2GlobalBin = std::vector< std::map< std::pair<int,int> , std::pair<int, int>> > (2,  std::map< std::pair<int,int> , std::pair<int, int>>()  );
    int global_id;
    for(int layer=0;layer<2;layer++){
        for(unsigned int ru=0;ru<4*Focal::STAVES_PER_QUADRANT;ru++){
            
            if(ru%Focal::STAVES_PER_QUADRANT < Focal::INNER_STAVES_PER_QUADRANT){
                for(unsigned int link=0;link<Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::DATA_LINKS;link++){
                    fRULink2GlobalBin.at(layer)[{ru,link}] = {global_id+link, 1};
                }
                fRULink2GlobalBin.at(layer)[{ru,Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::DATA_LINKS}] = {global_id+Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::DATA_LINKS, Focal_Outer3::CHIPS};
                fRULink2GlobalBin.at(layer)[{ru,Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::DATA_LINKS+Focal_I3_I3_O3_O6::OUTERGROUPS_O3*Focal_Outer3::DATA_LINKS}] = {global_id+Focal_I3_I3_O3_O6::INNERGROUPS*Focal_Inner3::DATA_LINKS+Focal_I3_I3_O3_O6::OUTERGROUPS_O3*Focal_Outer3::CHIPS, Focal_Outer6::CHIPS};
            } else {
                for(unsigned int link=0;link<Focal_O3_O3_O3_O3_O3::GROUPS*Focal_Outer3::DATA_LINKS;link++){
                    fRULink2GlobalBin.at(layer)[{ru,link}] = {global_id+link*Focal_Outer3::CHIPS, Focal_Outer3::CHIPS};
                }
            }
            global_id+=Focal::CHIPS_PER_FOCAL_STAVE;
        }
    }


    //for(int layer=0;layer<2;layer++){       
    //    for(auto const & rulink : fRULink2GlobalBin.at(layer)){
    //        std::cout<<"Layer:"<<layer << "- RU"<<rulink.first.first<<",Link"<<rulink.first.second<<": "<<rulink.second.first<<"-"<<rulink.second.second<<std::endl;
    //    }
    //}


}

int FOCALDetectorAnalysis::ChipMode(int chipid){

    int string_chipid = chipid % Focal::CHIPS_PER_FOCAL_STAVE;
    // inner mode strings
    if(chipid % (Focal::STAVES_PER_QUADRANT*Focal::CHIPS_PER_FOCAL_STAVE) < Focal::CHIPS_PER_FOCAL_STAVE*Focal::INNER_STAVES_PER_QUADRANT){

        // inner mode chip
        if(string_chipid < Focal_Inner3::CHIPS*Focal_I3_I3_O3_O6::INNERGROUPS) 
            return 0;
        // outer mode master
        else if(string_chipid == Focal_Inner3::CHIPS*Focal_I3_I3_O3_O6::INNERGROUPS || string_chipid == Focal_Inner3::CHIPS*Focal_I3_I3_O3_O6::INNERGROUPS+Focal_I3_I3_O3_O6::OUTERGROUPS_O3*Focal_Outer3::CHIPS )
            return 1;
        else return 2;
    }
    else {
        // outer mode master
        if(!(string_chipid%3))
            return 1;
        // outer mode slave
        else return 2;
    }
}

void FOCALDetectorAnalysis::FillHitmap(){

    
    gInterpreter->GenerateDictionary("vector<int>", "vector");

    std::string chainname = Form("%s/PhysicsEventData*.root", fSimFolder.c_str());
    std::cout << "Starting to fill the hitmap from "<<chainname<<std::endl;
    if(fSimFolder.length()==0){
        std::cerr << "FOCALDetectorAnalysis: simulation folder not set. " << std::endl;
        return; 
    }

    fPhysicsChain = new TChain("mPhysicsEvents");
    fPhysicsChain->Add(Form("%s", chainname.c_str()));
    if(!fPhysicsChain->GetEntries()){
        std::cerr << "FOCALDetectorAnalysis: Could not open physics event data file. " << std::endl;
        return; 
    }
    //fPhysicsChain->Print();
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
    for(unsigned int e=0;e<TMath::Min((Long_t) 2000,(Long_t)fPhysicsChain->GetEntries());e++){
        fPhysicsChain->GetEntry(e);
	if(! (e%1000) ) std::cout<<e<<" / "<<fPhysicsChain->GetEntries()<<std::endl;
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

void FOCALDetectorAnalysis::DrawEtaCircles(TCanvas *c, double eta_min, double eta_max, double eta_step){
    for(double eta=eta_max;eta>=eta_min;eta-=eta_step){
        for(unsigned int layer=0;layer<=1;layer++){
            c->cd(layer+1);
            DrawEtaCircle(layer, eta);
        }
    }
}

void FOCALDetectorAnalysis::DrawEtaCircle(int layer, double eta){
    if( ! (layer==0 || layer==1) ) return;
    double exp = TMath::Exp(-eta);
    double arctan = TMath::ATan(exp);
    double tan = 2*arctan;
    double r;
    double z;
    if(layer==0) z=FOCAL_PIXEL0_Z_MM;
    else if (layer==1) z=FOCAL_PIXEL1_Z_MM;
    r=z*tan;
    fEtaEllipse = new TEllipse(0,0,r,r);
    fEtaEllipse->SetFillStyle(0);
    fEtaEllipse->SetLineWidth(2);
    fEtaEllipse->SetLineColor(uibDigitalRed->GetNumber());
    fEtaEllipse->Draw("sames");
    std::cout << "Drawing eta circle "<< eta << std::endl;
    gPad->Modified();gPad->Update();

    fEtaText = new TLatex();
    fEtaText->SetTextAlign(11);
    fEtaText->SetTextColor(uibDigitalRed->GetNumber());
    fEtaText->DrawLatex(10+r*TMath::Cos(TMath::Pi()/4), 10+r*TMath::Cos(TMath::Pi()/4.), Form("#eta = %.1lf", eta));

    
}

void FOCALDetectorAnalysis::Draw(){
    cBare = new TCanvas("cBare", "Bare Pixel Planes", CANVAS_WIDTH*1.2, CANVAS_HEIGHT);
    cBare->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBare->cd(l+1);
        fFOCALPlane_Bare.at(l)->Draw();
    }
    cBare->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cBare->GetName()));
    cBare->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cBare->GetName()));
    cBare->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cBare->GetName()));
    cBare->SaveAs(Form("%s/plots/%s.root", fSimFolder.c_str(), cBare->GetName()));

    cChipIDs = new TCanvas("cChipIds", "Chip IDs Pixel Planes", CANVAS_WIDTH*1.2, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cChipIDs->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cChipIDs->cd(l+1);
        gPad->SetRightMargin(0.15);
        fFOCALPlane_ChipIDs.at(l)->Draw("COLZ L");
    }
    cChipIDs->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cChipIDs->GetName()));
    cChipIDs->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cChipIDs->GetName()));
    cChipIDs->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cChipIDs->GetName()));
    cChipIDs->SaveAs(Form("%s/plots/%s.root", fSimFolder.c_str(), cChipIDs->GetName()));


    cChipModes = new TCanvas("cChipModes", "Chip Modes Pixel Planes", CANVAS_WIDTH*1.2, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cChipModes->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cChipModes->cd(l+1);

        gPad->SetRightMargin(0.1);
        fFOCALPlane_ChipModes.at(l)->Draw("COLZ L");
        DrawEtaCircle(l,5.5);
        DrawEtaCircle(l,4);
        DrawEtaCircle(l,3.5);
        fFOCALPlane_ChipModes.at(l)->GetZaxis()->SetNdivisions(2);
        gPad->Modified();gPad->Update();
    }
    cChipModes->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cChipModes->GetName()));
    cChipModes->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cChipModes->GetName()));
    cChipModes->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cChipModes->GetName()));
    cChipModes->SaveAs(Form("%s/plots/%s.root", fSimFolder.c_str(), cChipModes->GetName()));

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


    cDataRate = new TCanvas("cDataRate", "Chip Data Rate Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cDataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cDataRate->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Datarate.at(l)->Draw("COLZ L");
    }

    cBusyV = new TCanvas("cBusyV", "Chip Busy Violations Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cBusyV->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBusyV->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_BusyV.at(l)->SetMaximum(0.2);
        fFOCALPlane_BusyV.at(l)->Draw("COLZ L");
        //gPad->SetLogz();

    }

    cBusy = new TCanvas("cBusy", "Chip Busy Flags Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cBusy->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBusy->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Busy.at(l)->SetMaximum(0.1);
        fFOCALPlane_Busy.at(l)->Draw("COLZ L");
        //gPad->SetLogz();

    }

    cFrameEff = new TCanvas("cFrame", "Chip Frame Efficiency Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cFrameEff->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cFrameEff->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_FrameEfficiency.at(l)->Draw("COLZ L");

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
        hpoly->SetTitle(Form("FoCal Pixel Chip Modes Plane  %d;x (mm);y (mm);Chip mode (0=inner mode, 1=outer master, 2=outer slave)", l));
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
        hpoly->SetTitle(Form("FoCal Pixel Datarate Plane %d;x (mm);y (mm);Data rate per link (Mbit/s)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Datarate.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_hits_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Hits Plane %d;x (mm);y (mm);Average hits / chip / event", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Hits.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_busyv_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Busy Violations Plane %d;x (mm);y (mm);Busy violations / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_BusyV.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_busy_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Busy Plane %d;x (mm);y (mm);Busy flags / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Busy.push_back(hpoly);


        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_frameff_%d", l));
        hpoly->SetTitle(Form("FoCal Pixel Frame Efficiency Plane %d;x (mm);y (mm);Frame efficiency / chip", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_FrameEfficiency.push_back(hpoly);

    }

    FillChipIDs();
    initChipMode();
    initRULinks();



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
                
          fChipRadius.push_back( (x_high-x_low)*(x_high-x_low) + (y_high-y_low)*(y_high-y_low) );

        }

      }
    }
  }
}

#endif
