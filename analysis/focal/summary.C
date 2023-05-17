#include "TMultiGraph.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TGraph.h"

#include "RUNSettings.h"
#include "utils/UiBColors.cc"

#include <iostream>

#define BASEDIR "/alf/data/focal/alpide_systemc/sim_output/"
#define ALPIDE_TIME_CONSTANT 5000

TFile *rootfile;
TCanvas  *cBusyVEfficiency ;
TCanvas *cPileUp;

int dataset =0;

std::string dataset_dir;

TH1D* getHistogram( TCanvas *cplot, std::string base_dir, int run, char* const canvasname, char* const plotname, char* const drawoption){

//void summary(){

    //const char* canvasname = "cBusyVLinks";
    //const char* plotname = "fFramesWithBusyV";
    //int run=0;

    const char *filename = Form("%s/%s/run_%d/plots/%s.root" ,BASEDIR,base_dir.c_str(), run,canvasname);
    rootfile = new TFile(filename, "READ");
    if(!rootfile->IsOpen()) {
        std::cout<<"Could not open file "<<filename << std::endl;
        return nullptr;
    }
    TCanvas *c = (TCanvas*) rootfile->Get(canvasname);
    c->GetListOfPrimitives()->ls() ;

    if(c==nullptr){
        std::cout << canvasname << " is null pointer" << std::endl;
        return nullptr ;
    } else {
        //c->GetListOfPrimitives()->ls() ;
    }
    TH1D *hnew = (TH1D*) c->FindObject(plotname);
    if(hnew==nullptr){
        std::cout << plotname << " is null pointer" << std::endl;
        return nullptr;
    } else {
        //std::cout << "Found " << plotname << " "<<hnew->GetMean() << std::endl;
    }

    //TCanvas *c0 = new TCanvas();
    cplot->cd();
    hnew->SetMarkerStyle(kFullCircle);
    hnew->SetMarkerColor(uibColorIndex(dataset%4));
    hnew->SetLineColor(uibColorIndex(dataset%4));
    hnew->SetMarkerStyle(20+dataset/4);
    hnew->Draw(drawoption);
    //hnew->Draw("C sames");

    //hnew->Draw("EX0 sames");
    gPad->Modified();
    gPad->Update();
    return hnew;
}


void mySave(TCanvas *c){
    c->SaveAs(Form("plots/%s.png", c->GetName()));
    c->SaveAs(Form("plots/%s.pdf", c->GetName()));
    c->SaveAs(Form("plots/%s.eps", c->GetName()));
    c->SaveAs(Form("plots/%s.root", c->GetName()));
}


void summary(std::vector<std::pair<std::string, RUNSettings>> myrunsettings, const char* tag){

    gStyle->SetCanvasPreferGL(false);

    //std::vector<RUNSettings> myrunsettings = runsettings[base_dir];
    
    TH1D *h;

    TGraph *gPileUp_vs_FrameEfficiency = new TGraph();

    dataset=0;
    cBusyVEfficiency = new TCanvas(Form("cBusyVEfficiency_cumulative_%s", tag), "Frame / BUSYV Efficiency", 1000*1.1,1000);
    double legBottom = gPad->GetBottomMargin()+0.01;
    double legRight = 1-gPad->GetRightMargin()-0.01;
    double dX =0.3;
    double dY =0.3;
    if(std::string(tag).find("StrobeInactive") != std::string::npos) dX = 0.6;
    if(std::string(tag).find("MaskGrid") != std::string::npos) {dX = 0.6;};
    TLegend *leg = new TLegend(legRight,legBottom, legRight-dX, legBottom+dY);
    for(auto mrs:myrunsettings){

        auto rs = mrs.second;
        std::string base_dir = mrs.first;
        
        h = getHistogram(cBusyVEfficiency, base_dir, rs.run, "cBusyVLinks", "fFramesWithBusyV_cumulative", "PC sames");
        h->GetXaxis()->SetTitle("Maximum number of links in BUSY violation");
        h->GetYaxis()->SetTitle("Fraction of frames");
        gPileUp_vs_FrameEfficiency->SetPoint(dataset, (ALPIDE_TIME_CONSTANT+rs.system_rate-rs.strobe_inactive)/((float) rs.event_rate), 1-h->GetBinContent(1));
        dataset++;
        std::cout << h->GetMean() <<std::endl;
        if(std::string(tag).find("StrobeInactive") != std::string::npos) leg->AddEntry(h, Form("Strobe inactive length %.1lf #mus", rs.strobe_inactive/1000.), "PL");
        else if(std::string(tag).find("MaskGrid") != std::string::npos) leg->AddEntry(h, Form("System rate %d #mus, Cl-size %d, d_{grid mask} = %d", rs.system_rate/1000, rs.cluster_size, rs.grid_mask_constant), "PL");
        else if(std::string(tag).find("CS4CS3") != std::string::npos) leg->AddEntry(h, Form("System rate %d #mus, Cl-size %d", rs.system_rate/1000, rs.cluster_size), "PL");
        else leg->AddEntry(h, Form("System rate %d #mus", rs.system_rate/1000), "PL");
    }
    leg->Draw();
    mySave(cBusyVEfficiency);

    cPileUp = new TCanvas(Form("cPileUp_%s", tag), "Pile Up <-> Efficiency", 1000*1.1,1000);
    gPileUp_vs_FrameEfficiency->SetTitle("Global frame inefficiency vs. pile-up;Pile-up;Global frame ineffiency");
    gPileUp_vs_FrameEfficiency->SetMarkerColor(uibColorIndex(0));
    gPileUp_vs_FrameEfficiency->SetLineColor(uibColorIndex(0));
    gPileUp_vs_FrameEfficiency->SetMarkerStyle(kFullCircle);
    gPileUp_vs_FrameEfficiency->Sort();
    gPileUp_vs_FrameEfficiency->Draw("APL");
    gPileUp_vs_FrameEfficiency->GetYaxis()->SetRangeUser(0,1);
    gPad->Modified();gPad->Update();

    mySave(cPileUp);


    dataset=0;
    cBusyVEfficiency = new TCanvas(Form("cBusyVEfficiency_%s", tag), "Frame / BUSYV Efficiency", 1000*1.1,1000);
    legBottom = gPad->GetBottomMargin()+0.01;
    legRight = 1-gPad->GetRightMargin()-0.01;
    leg = new TLegend(legRight,legBottom, legRight-0.3, legBottom+0.3);
    for(auto mrs:myrunsettings){

        auto rs = mrs.second;
        std::string base_dir = mrs.first;       
        h = getHistogram(cBusyVEfficiency,base_dir,  rs.run, "cBusyVLinks", "fFramesWithBusyV", "hist sames");
        dataset++;
        std::cout << h->GetMean() <<std::endl;
        leg->AddEntry(h, Form("System rate %d #mus", rs.system_rate/1000), "PL");
    }
    leg->Draw();
    mySave(cBusyVEfficiency);

}

void summary(){
    std::vector<std::pair<std::string, RUNSettings>> runs;//= {0,8,1,9};
//    summary("fixed_20230421", runs, "CS4");
    
    //runs = {2,10,3,11};
//    summary("fixed_20230421", runs, "CS3");

    //runs = {0,18,24};
//    summary("fixed_20230421", runs, "StrobeInactive_CS4");

    //runs = {2,20,26};
//    summary("fixed_20230421", runs, "StrobeInactive_CS3");

    //runs = {3,21,27};
//    summary("fixed_20230421", runs, "StrobeInactive_CS3_20us");

    std::string pp_base_dataset = "fixed_20230421";
    std::string pp_mask_dataset = "masks_20230426";
    std::string pPb_base_dataset = "pPb_20230427";

    /*runs = {
        {pp_base_dataset, runsettings[pp_base_dataset][0]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][12]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][6]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][0]}
    };*/

  /*  
    runs = {
        {pPb_base_dataset, runsettings[pPb_base_dataset][0]},
        {pPb_base_dataset, runsettings[pPb_base_dataset][2]},
        {pPb_base_dataset, runsettings[pPb_base_dataset][1]},
        {pPb_base_dataset, runsettings[pPb_base_dataset][3]}
    };
*/
    

    
    runs = {
    /*    {pp_base_dataset, runsettings[pp_base_dataset][0]},
        {pp_base_dataset, runsettings[pp_base_dataset][2]},
        {pp_base_dataset, runsettings[pp_base_dataset][1]},
        {pp_base_dataset, runsettings[pp_base_dataset][3]}
    };*/
    
        {pp_mask_dataset, runsettings[pp_mask_dataset][14]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][8]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][15]},
        {pp_mask_dataset, runsettings[pp_mask_dataset][9]}
    };
    
    summary(runs, "MaskGrid");


}
