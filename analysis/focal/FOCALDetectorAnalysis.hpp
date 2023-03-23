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
#include "TGraphAsymmErrors.h"

#define CANVAS_WIDTH 1024
#define CANVAS_HEIGHT 512

#define NRADIUS_BINS 35
#define MAX_RADIUS 700

#define NEFFI_BINS 500
#define MAX_EFFI 1.001
#define MIN_EFFI MAX_EFFI-0.5
#define NETA_BINS 30
#define MAX_ETA 6
#define MIN_ETA 3

#define FOCAL_X_MAX_MM 500
#define FOCAL_Y_MAX_MM 500

#define FOCAL_X_MAX_CM (FOCAL_X_MAX_MM/10)
#define FOCAL_Y_MAX_CM (FOCAL_Y_MAX_MM/10)

#define FOCAL_N_BINS 500

#define NDATARATE_BINS 100
#define MIN_DATARATE 0
#define MAX_DATARATE 1000

double Eta(double r, double z){
    return -TMath::Log(TMath::Tan(TMath::ATan2(r,z)/2));
}


class FOCALDetectorAnalysis {

    private:
    
        std::string fSimFolder;
        
        TFile *fRUFile;
        TFile *fAlpideStatsFile;
        TChain *fPhysicsChain;

        TEllipse *fEtaEllipse;
        TLatex *fEtaText;

        ULong64_t fNEvents;
        Double_t fSystemRate;
        Double_t fEventRate;

        std::vector<TH2Poly*> fFOCALPlane_Bare;
        std::vector<TH2Poly*> fFOCALPlane_Occupancy;
        std::vector<TH2Poly*> fFOCALPlane_Hits;
        std::vector<TH2Poly*> fFOCALPlane_Datarate;
        std::vector<TH2Poly*> fFOCALPlane_ChipModes;
        std::vector<TH2Poly*> fFOCALPlane_ChipIDs;
        std::vector<TH2Poly*> fFOCALPlane_BusyV;
        std::vector<TH2Poly*> fFOCALPlane_Busy;
        std::vector<TH2Poly*> fFOCALPlane_FrameEfficiency;

        std::vector<TH2D*> fFOCALFrameEfficiency_vs_Radius;
        std::vector<TH2D*> fFOCALFrameEfficiency_vs_Eta;
        std::vector<TH2D*> fFOCALDataRate_vs_Eta;
        std::vector<TH2D*> fFOCALDataRate_vs_Radius;

        std::vector<TGraphAsymmErrors*> fGFOCALFrameEfficiency_vs_Radius;
        std::vector<TGraphAsymmErrors*> fGFOCALFrameEfficiency_vs_Eta;

        std::vector<TGraphAsymmErrors*> fGFOCALDataRate_vs_Radius;
        std::vector<TGraphAsymmErrors*> fGFOCALDataRate_vs_Eta;

        std::vector<TH2D*> fFOCALValidHits;
        std::vector<TH2D*> fFOCALInvalidHits;

        std::vector<int> fChipModes;
        std::vector<double> fChipRadius;
        std::vector<double> fChipEta;

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
        TCanvas *cFrameEff_geom;
        TCanvas *cDataRate_geom;
        TCanvas *cValidHits;

        void init();

    public:
        FOCALDetectorAnalysis(){init();};
        FOCALDetectorAnalysis(std::string simfolder, ULong64_t nevents) :fSimFolder(simfolder), fNEvents(nevents){
            std::cout << "FOCALDetectorAnalysis for folder "<<fSimFolder<<std::endl;
            gSystem->Exec(Form("mkdir %s/plots", fSimFolder.c_str())); 
            init();
        };

        void Draw();
        void FillHitmap();
        void FillValidHits();
        void AnalyseRULinks();

        void DrawEtaCircles(TCanvas *canvas, double eta_min, double eta_max, double delta_eta);
        void DrawEtaCircle(int layer, double eta);

        TGraphAsymmErrors *GraphEfficiency_vs_Radius(int layer){return fGFOCALFrameEfficiency_vs_Radius.at(layer);}
        TGraphAsymmErrors *GraphEfficiency_vs_Eta(int layer){return fGFOCALFrameEfficiency_vs_Eta.at(layer);}

        TGraphAsymmErrors *GraphDataRate_vs_Radius(int layer){return fGFOCALDataRate_vs_Radius.at(layer);}
        TGraphAsymmErrors *GraphDataRate_vs_Eta(int layer){return fGFOCALDataRate_vs_Eta.at(layer);}

        void SetNEvents(ULong64_t number_of_events)         {fNEvents=number_of_events;}
        ULong64_t GetNEvents(ULong64_t number_of_events)    {return fNEvents;}
        void SetSimFolder(std::string sim_folder)           {fSimFolder=sim_folder;}
        std::string GetSimFolder()                          {return fSimFolder;}

        void SetSystemRate(Double_t system_rate){fSystemRate=system_rate;}
        void SetEventRate(Double_t event_rate){fEventRate=event_rate;}

        Double_t GetSystemRate(){return fSystemRate;}
        Double_t GetEventRate(){return fEventRate;}


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
        //std::cout << "Reading " <<fRUEvents->GetName() <<std::endl;
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

    for( int i=0;i<NLinks;i++){
        float datarate = DataRates.at(i) / (float) NEntries.at(i);
        unsigned int gbin = fRULink2GlobalBin.at(layer)[{ru, i}].first;
        gbin -= Focal::CUMULATIVE_CHIP_COUNT_AT_LAYER[layer];
        fFOCALDataRate_vs_Radius.at(layer)->Fill(fChipRadius.at(gbin), datarate);
        fFOCALDataRate_vs_Eta.at(layer)->Fill(fChipEta.at(gbin), datarate);
        for( int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++,gbin++){
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
    
    for( int i=0;i<NLinks;i++){
        for( int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++){
            unsigned int gbin = fRULink2GlobalBin.at(layer)[{ru, i}].first + _chips;
            gbin -= Focal::CUMULATIVE_CHIP_COUNT_AT_LAYER[layer];
            
            //fFOCALPlane_BusyV.at(layer)->SetBinContent(1+gbin, ((double)NBusyV.at(i))/fNEvents);

            double busyvrate =  ((double)NBusyV.at(i))/(fNEvents*fEventRate/fSystemRate);
            fFOCALPlane_BusyV.at(layer)->SetBinContent(1+gbin, busyvrate);

            double frameeffi =  1- busyvrate;

            if(frameeffi<=0) std::cerr<<"Error: frame efficiency <= 0" << std::endl;

            fFOCALPlane_FrameEfficiency.at(layer)->SetBinContent(1+gbin,frameeffi);
            
            fFOCALFrameEfficiency_vs_Radius.at(layer)->Fill(fChipRadius.at(gbin), frameeffi);
            fFOCALFrameEfficiency_vs_Eta.at(layer)->Fill(fChipEta.at(gbin), frameeffi);
            //std::cout <<  ( (Int_t) (fNEvents-NBusyV.at(i)) )/( (double) fNEvents)<< std::endl;
        }
    }

    for(unsigned int layer=0;layer<=1;layer++){

        int etapoint = 0;
        int radiuspoint = 0;
        int datarate_etapoint = 0;
        int datarate_radiuspoint = 0;

        fGFOCALDataRate_vs_Radius.push_back(new TGraphAsymmErrors());
        fGFOCALDataRate_vs_Eta.push_back(new TGraphAsymmErrors());

        fGFOCALFrameEfficiency_vs_Radius.push_back(new TGraphAsymmErrors());
        fGFOCALFrameEfficiency_vs_Eta.push_back(new TGraphAsymmErrors());


        for( int bin=1;bin<=fFOCALDataRate_vs_Radius.at(layer)->GetNbinsX();bin++){
            TH1D *htmp = fFOCALDataRate_vs_Radius.at(layer)->ProjectionY("htmp", bin, bin);
            double x ;
            double y ;
            double rmsy;
            double ey_low;
            double ey_high;

            y = htmp->GetMean();

            if(htmp->GetEntries()){

                x = fFOCALDataRate_vs_Radius.at(layer)->GetXaxis()->GetBinCenter(bin);
                y = htmp->GetMean();
                rmsy = htmp->GetRMS();

                if(y-rmsy>0) ey_low = rmsy;
                else ey_low = y;

                if(y+rmsy>=MAX_DATARATE) ey_high = 1-y;
                else ey_high = rmsy;

                fGFOCALDataRate_vs_Radius.at(layer)->SetPoint(datarate_radiuspoint,x,y);
                fGFOCALDataRate_vs_Radius.at(layer)->SetPointError(datarate_radiuspoint, 0, 0, ey_low, ey_high);
                datarate_radiuspoint++;
            }

            if(htmp) delete htmp;
        }

        for( int bin=1;bin<=fFOCALDataRate_vs_Eta.at(layer)->GetNbinsX();bin++){
            TH1D *htmp = fFOCALDataRate_vs_Eta.at(layer)->ProjectionY("htmp", bin, bin);
            double x ;
            double y ;
            double rmsy;
            double ey_low;
            double ey_high;

            y = htmp->GetMean();

            if(htmp->GetEntries()){

                x = fFOCALDataRate_vs_Eta.at(layer)->GetXaxis()->GetBinCenter(bin);
                y = htmp->GetMean();
                rmsy = htmp->GetRMS();

                if(y-rmsy>0) ey_low = rmsy;
                else ey_low = y;

                if(y+rmsy>=MAX_DATARATE) ey_high = 1-y;
                else ey_high = rmsy;

                fGFOCALDataRate_vs_Eta.at(layer)->SetPoint(datarate_etapoint,x,y);
                fGFOCALDataRate_vs_Eta.at(layer)->SetPointError(datarate_etapoint, 0, 0, ey_low, ey_high);
                datarate_etapoint++;
            }

            if(htmp) delete htmp;
        }

        for( int bin=1;bin<=fFOCALFrameEfficiency_vs_Radius.at(layer)->GetNbinsX();bin++){
            TH1D *htmp = fFOCALFrameEfficiency_vs_Radius.at(layer)->ProjectionY("htmp", bin, bin);
            double x ;
            double y ;
            double rmsy;
            double ey_low;
            double ey_high;

            y = htmp->GetMean();

            if(htmp->GetEntries()){

                x = fFOCALFrameEfficiency_vs_Radius.at(layer)->GetXaxis()->GetBinCenter(bin);
                y = htmp->GetMean();
                rmsy = htmp->GetRMS();

                if(y-rmsy>0) ey_low = rmsy;
                else ey_low = y;

                if(y+rmsy>=MAX_EFFI) ey_high = 1-y;
                else ey_high = rmsy;

                fGFOCALFrameEfficiency_vs_Radius.at(layer)->SetPoint(radiuspoint,x,y);
                fGFOCALFrameEfficiency_vs_Radius.at(layer)->SetPointError(radiuspoint, 0, 0, ey_low, ey_high);
                radiuspoint++;
            }

            if(htmp) delete htmp;
        }
        
        for( int bin=1;bin<=fFOCALFrameEfficiency_vs_Eta.at(layer)->GetNbinsX();bin++){

            double x ;
            double y ;
            double rmsy;
            double ey_low;
            double ey_high;
            TH1D* htmp = fFOCALFrameEfficiency_vs_Eta.at(layer)->ProjectionY("htmp", bin, bin);

            if(htmp->GetEntries()){    
                x = fFOCALFrameEfficiency_vs_Eta.at(layer)->GetXaxis()->GetBinCenter(bin);
                y = htmp->GetMean();
                rmsy = htmp->GetRMS();

                if(y-rmsy>0) ey_low = rmsy;
                else ey_low = y;

                if(y+rmsy>=MAX_EFFI) ey_high = 1-y;
                else ey_high = rmsy;

                fGFOCALFrameEfficiency_vs_Eta.at(layer)->SetPoint(etapoint,x,y);
                fGFOCALFrameEfficiency_vs_Eta.at(layer)->SetPointError(etapoint, 0, 0, ey_low, ey_high);

                etapoint++;
            }
        

            if(htmp) delete htmp;

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
    
    for( int i=0;i<NLinks;i++){
        for( int _chips=0;_chips < fRULink2GlobalBin.at(layer)[{ru, i}].second;_chips++){
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
    int global_id=0;
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

    unsigned int string_chipid = chipid % Focal::CHIPS_PER_FOCAL_STAVE;
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

void FOCALDetectorAnalysis::FillValidHits(){
    TFile *hitfile = new TFile(Form("%s/Hits.root", fSimFolder.c_str()), "READ");
    TTree *hittree = (TTree*) hitfile->Get("treeHits");
    double X,Y;
    int layer;
    Bool_t valid;
    hittree->SetBranchAddress("X", &X);
    hittree->SetBranchAddress("Y", &Y);
    hittree->SetBranchAddress("valid", &valid);
    hittree->SetBranchAddress("layer", &layer);
    for(int e=0;e<hittree->GetEntries();e++){
        hittree->GetEntry(e);
        if(!valid) fFOCALInvalidHits.at(layer)->Fill(X,Y);
        if(valid) fFOCALValidHits.at(layer)->Fill(X,Y);
    }
    hitfile->Close();
    delete hitfile;
}

void FOCALDetectorAnalysis::FillHitmap(){

    
    gInterpreter->GenerateDictionary("vector<int>", "vector");

    std::cout << "Starting to fill the hitmap from "<<std::endl;
    if(fSimFolder.length()==0){
        std::cerr << "FOCALDetectorAnalysis: simulation folder not set. " << std::endl;
        return; 
    }

    fPhysicsChain = new TChain("mPhysicsEvents");

    std::string chainname = Form("%s/PhysicsEventData.root", fSimFolder.c_str());
    fPhysicsChain->Add(Form("%s", chainname.c_str()));

    for(unsigned int i=1;i<10;i++){
    
        std::cout<<chainname<<std::endl;
        chainname = Form("%s/PhysicsEventData_%d.root", fSimFolder.c_str(), i);
        fPhysicsChain->Add(Form("%s", chainname.c_str()));
    }
    if(!fPhysicsChain->GetEntries()){
        std::cerr << "FOCALDetectorAnalysis: Could not open physics event data file. " << std::endl;
        return; 
    }
    //fPhysicsChain->Print();
    std::cout << fPhysicsChain->GetEntries() << std::endl;

    std::vector<int> *n = new std::vector<int>(); 
    std::vector<int> *id = new std::vector<int>(); 
    ULong64_t tNow =0;

    fPhysicsChain->SetBranchStatus("*", 0);
    fPhysicsChain->SetBranchStatus("nHits", 1);
    fPhysicsChain->SetBranchStatus("chipId", 1);
    fPhysicsChain->SetBranchStatus("tNow", 1);

    fPhysicsChain->SetBranchAddress("nHits", &n);
    fPhysicsChain->SetBranchAddress("chipId", &id);
    fPhysicsChain->SetBranchAddress("tNow", &tNow);

    std::vector<Long64_t> NChipHits(Focal::CHIPS, 0);
    ULong64_t NHits = 0;

    Long64_t NChainEvents = fPhysicsChain->GetEntries();
    NChainEvents = 1000;

    std::cout << "Reading physics chain " << fPhysicsChain->GetName() << " with "<<NChainEvents <<" entries ...";
    for(unsigned int e=0;e<NChainEvents;e++){
        if(! (e%1000) ) std::cout<<e<<" / "<<NChainEvents<<std::endl;
        fPhysicsChain->GetEntry(e);
        if(tNow<50000) continue;
        if(tNow>=100000) continue;
        unsigned int nevents = TMath::Min(n->size(),id->size());
        for(unsigned int i=0;i<nevents;i++){
            if( (unsigned int) id->at(i) < NChipHits.size() ) {
                NChipHits.at(id->at(i)) += n->at(i);
                NHits += n->at(i);
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



    if(NChainEvents) {
        std::cout << "Scaling by " << 1./NChainEvents << std::endl;
        std::cout << fFOCALPlane_Hits.at(0)->GetMaximum() <<std::endl;
        std::cout << NHits <<std::endl;

        //fFOCALPlane_Hits.at(0)->Scale(1./NChainEvents);
        //fFOCALPlane_Hits.at(1)->Scale(1./NChainEvents);
        std::cout << fFOCALPlane_Hits.at(0)->GetMaximum() <<std::endl;


        //fFOCALPlane_Occupancy.at(0)->Scale(1./NChainEvents);
        //fFOCALPlane_Occupancy.at(1)->Scale(1./NChainEvents);

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

/*    
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

*/
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

    cOcc->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cOcc->GetName()));
    cOcc->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cOcc->GetName()));
    cOcc->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cOcc->GetName()));
    cOcc->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cOcc->GetName()));

    cDataRate = new TCanvas("cDataRate", "Chip Data Rate Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cDataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cDataRate->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Datarate.at(l)->Draw("COLZ L");
    }


    cDataRate->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cDataRate->GetName()));
    cDataRate->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cDataRate->GetName()));
    cDataRate->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cDataRate->GetName()));
    cDataRate->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cDataRate->GetName()));

    cBusyV = new TCanvas("cBusyV", "Chip Busy Violations Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cBusyV->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBusyV->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_BusyV.at(l)->Draw("COLZ L");
        //gPad->SetLogz();

    }

    cBusyV->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cBusyV->GetName()));
    cBusyV->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cBusyV->GetName()));
    cBusyV->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cBusyV->GetName()));
    cBusyV->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cBusyV->GetName()));


    cBusy = new TCanvas("cBusy", "Chip Busy Flags Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cBusy->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cBusy->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_Busy.at(l)->Draw("COLZ L");
        //gPad->SetLogz();

    }

    cBusy->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cBusy->GetName()));
    cBusy->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cBusy->GetName()));
    cBusy->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cBusy->GetName()));
    cBusy->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cBusy->GetName()));

    cFrameEff = new TCanvas("cFrame", "Chip Frame Efficiency Pixel Planes", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    gStyle->SetPalette(kLake);
    cFrameEff->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cFrameEff->cd(l+1);
        gPad->SetRightMargin(0.2);
        fFOCALPlane_FrameEfficiency.at(l)->Draw("COLZ L");

    }

    cFrameEff->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cFrameEff->GetName()));
    cFrameEff->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cFrameEff->GetName()));
    cFrameEff->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cFrameEff->GetName()));
    cFrameEff->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cFrameEff->GetName()));

    cFrameEff_geom = new TCanvas("cFrame_geom", "Chip Frame Efficiency vs. ETA/Radius", CANVAS_WIDTH*1.25, 2*CANVAS_HEIGHT);
    cFrameEff_geom->Divide(2,2);

    for(unsigned int l=0;l<=1;l++){
        cFrameEff_geom->cd(l+1);
        fFOCALFrameEfficiency_vs_Radius.at(l)->Draw("COLZ");
        fGFOCALFrameEfficiency_vs_Radius.at(l)->Draw("*L sames");

        gPad->SetGridy();gPad->SetGridx();
        cFrameEff_geom->cd(l+1+2);
        fFOCALFrameEfficiency_vs_Eta.at(l)->Draw("COLZ");
        fGFOCALFrameEfficiency_vs_Eta.at(l)->Draw("*L sames");

        //fFOCALFrameEfficiency_vs_Eta.at(l)->GetYaxis()->SetLimits(0,1);
        gPad->Modified();
        gPad->Update();

        gPad->SetGridy();gPad->SetGridx();

    }
    cFrameEff_geom->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cFrameEff_geom->GetName()));
    cFrameEff_geom->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cFrameEff_geom->GetName()));
    cFrameEff_geom->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cFrameEff_geom->GetName()));
    cFrameEff_geom->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cFrameEff_geom->GetName()));

    cDataRate_geom = new TCanvas("cDataRate_geom", "Data rate vs. ETA/Radius", CANVAS_WIDTH*1.25, 2*CANVAS_HEIGHT);
    cDataRate_geom->Divide(2,2);

    for(unsigned int l=0;l<=1;l++){
        cDataRate_geom->cd(l+1);
        fFOCALDataRate_vs_Radius.at(l)->Draw("COLZ");
        fGFOCALDataRate_vs_Radius.at(l)->Draw("*L sames");

        gPad->SetGridy();gPad->SetGridx();
        cDataRate_geom->cd(l+1+2);
        fFOCALDataRate_vs_Eta.at(l)->Draw("COLZ");
        fGFOCALDataRate_vs_Eta.at(l)->Draw("*L sames");

        //fFOCALFrameEfficiency_vs_Eta.at(l)->GetYaxis()->SetLimits(0,1);
        gPad->Modified();
        gPad->Update();

        gPad->SetGridy();gPad->SetGridx();

    }

    cDataRate_geom->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cDataRate_geom->GetName()));
    cDataRate_geom->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cDataRate_geom->GetName()));
    cDataRate_geom->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cDataRate_geom->GetName()));
    cDataRate_geom->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cDataRate_geom->GetName()));


    cValidHits = new TCanvas("cValidHits", "Valid/Invalid Hits in FOCAL-Simu -> SystemC", CANVAS_WIDTH*1.25, CANVAS_HEIGHT);
    cValidHits->Divide(2,1);

    for(unsigned int l=0;l<=1;l++){
        cValidHits->cd(l+1);
        fFOCALValidHits.at(l)->Draw("");
        fFOCALInvalidHits.at(l)->Draw("sames");
        gPad->SetGridy();gPad->SetGridx();
    }

    cValidHits->SaveAs(Form("%s/plots/%s.png", fSimFolder.c_str(), cValidHits->GetName()));
    cValidHits->SaveAs(Form("%s/plots/%s.pdf", fSimFolder.c_str(), cValidHits->GetName()));
    cValidHits->SaveAs(Form("%s/plots/%s.eps", fSimFolder.c_str(), cValidHits->GetName()));
    cValidHits->SaveAs(Form("%s/plots/%s.root",fSimFolder.c_str(), cValidHits->GetName()));
    
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
        hpoly->SetName(Form("focalplane_bare_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Plane %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Bare.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_chipids_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Chip IDs Plane %d;x (mm);y (mm)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_ChipIDs.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_chipmodes_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Chip Modes Plane  %d;x (mm);y (mm);Chip mode (0=inner mode, 1=outer master, 2=outer slave)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_ChipModes.push_back(hpoly);
        
        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_occupancy_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Occupancy Plane %d;x (mm);y (mm);Average occupancy / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Occupancy.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);        
        hpoly->SetName(Form("focalplane_datarate_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Datarate Plane %d;x (mm);y (mm);Data rate per link (Mbit/s)", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Datarate.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_hits_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Hits Plane %d;x (mm);y (mm);Average hits / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Hits.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_busyv_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Busy Violations Plane %d;x (mm);y (mm);Busy violations / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_BusyV.push_back(hpoly);

        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_busy_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Busy Plane %d;x (mm);y (mm);Busy flags / chip / frame", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_Busy.push_back(hpoly);


        hpoly = new TH2Poly();
        hpoly->SetStats(0);
        hpoly->SetName(Form("focalplane_frameff_%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()));
        hpoly->SetTitle(Form("FoCal Pixel Frame Efficiency Plane %d;x (mm);y (mm);Frame efficiency / chip", l));
        CreateFOCALChipBins(hpoly);
        fFOCALPlane_FrameEfficiency.push_back(hpoly);

        TH2D *_hfer = new TH2D(Form("hFrameEfficiency_vs_R_Layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Frame Efficiency vs. Radius Layer %d;Radius (mm);Efficiency;Entries", l), NRADIUS_BINS, 0 , MAX_RADIUS, NEFFI_BINS, MIN_EFFI, MAX_EFFI) ;
        _hfer->SetStats(0);
        fFOCALFrameEfficiency_vs_Radius.push_back(_hfer);
 
        TH2D *_hfeeta = new TH2D(Form("hFrameEfficiency_vs_Eta_Layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Frame Efficiency vs. #eta Layer %d;#eta;Efficiency;Entries", l), NETA_BINS, MIN_ETA, MAX_ETA,NEFFI_BINS, MIN_EFFI, MAX_EFFI) ;
        _hfeeta->SetStats(0);
        fFOCALFrameEfficiency_vs_Eta.push_back(_hfeeta);


        TH2D *_hdreta = new TH2D(Form("hDataRate_vs_Eta_Layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Data Rate vs. #eta Layer %d;#eta;Data rate (Mbit/s);Entries", l), NETA_BINS, MIN_ETA, MAX_ETA, NDATARATE_BINS, MIN_DATARATE, MAX_DATARATE) ;
        _hdreta->SetStats(0);
        fFOCALDataRate_vs_Eta.push_back(_hdreta);


        TH2D *_hdrradius = new TH2D(Form("hDataRate_vs_Radius_Layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Data Rate vs. Radius Layer %d;Radius (mm);Data rate (Mbit/s);Entries", l), NRADIUS_BINS, 0 , MAX_RADIUS, NDATARATE_BINS, MIN_DATARATE, MAX_DATARATE) ;
        _hdrradius->SetStats(0);
        fFOCALDataRate_vs_Radius.push_back(_hdrradius);


        TH2D *valid = new TH2D(Form("validhits_layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Valid Hits Layer %d;X (mm);Y (mm)", l), FOCAL_N_BINS, -FOCAL_X_MAX_CM, FOCAL_X_MAX_CM, FOCAL_N_BINS, -FOCAL_Y_MAX_CM, FOCAL_Y_MAX_CM);
        TH2D *invalid = new TH2D(Form("invalidhits_layer%d_%s", l, fSimFolder.substr(fSimFolder.size() - 5).c_str()), Form("Inalid Hits Layer %d;X (mm);Y (mm)", l), FOCAL_N_BINS, -FOCAL_X_MAX_CM, FOCAL_X_MAX_CM, FOCAL_N_BINS, -FOCAL_Y_MAX_CM, FOCAL_Y_MAX_CM);
        valid->SetStats(0);
        invalid->SetStats(0);
        valid->SetMarkerColor(kGreen+1);
        invalid->SetMarkerColor(kRed);

        fFOCALInvalidHits.push_back(invalid);
        fFOCALValidHits.push_back(valid);

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
                
          double R= TMath::Sqrt((x_high+x_low)*(x_high+x_low)/4 + (y_high+y_low)*(y_high+y_low)/4);
          double eta = Eta(R,(FOCAL_PIXEL0_Z_MM+FOCAL_PIXEL1_Z_MM)/2.);
          
          if(fChipRadius.size()<1980){
            fChipRadius.push_back( R );
            fChipEta.push_back(eta);
          }


          //    std::cout << R << "\t" << eta << std::endl;

        }

      }
    }
  }
  std::cout << fChipRadius.size() << "\t" << fChipEta.size() << std::endl;

}

#endif
