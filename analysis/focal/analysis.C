
#include "FOCALDetectorAnalysis.hpp"
#include "TMultiGraph.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"


FOCALDetectorAnalysis *fda;

double SystemRate(int run){
    if(run==0) return 10000;
    if(run==1) return 5000;
    if(run==2) return 2500;
    if(run==3) return 20000;
    if(run==4) return 50000;
    return 10000;
}

double ClusterSize(int run){
    if(run==5) return 6;
    if(run==6) return 8;
    if(run==7) return 3;
    if(run==8) return 2;
    return 4;
}

void singleAnalysis(){

        int run = 0;
        fda = new FOCALDetectorAnalysis(Form("/alf/data/focal/alpide_systemc/sim_output/run_%d", run), 25000);
        fda->SetSystemRate(SystemRate(run));
        fda->SetEventRate(1000);
        fda->FillHitmap();
        fda->FillValidHits();
        fda->AnalyseRULinks();
        fda->Draw();
}

void fullAnalysis(){    

    TLegend *legRateScan_Eta = new TLegend(0.125 ,0.125, 0.6, 0.5);
    TLegend *legClusterScan_Eta = new TLegend(0.125 ,0.125, 0.4, 0.5);
    TLegend *legRateScan_Radius = new TLegend(0.4 ,0.125, 0.875, 0.5);
    TLegend *legClusterScan_Radius = new TLegend(0.6 ,0.125, 0.875, 0.5);

    TLegend *legClusterScan_DataRate_Radius = new TLegend(0.7 ,0.875, 0.98, 0.5);
    TLegend *legRateScan_DataRate_Radius = new TLegend(0.58 ,0.875, 0.98, 0.5);

    TLegend *legClusterScan_DataRate_Eta = new TLegend(0.1 ,0.875, 0.4, 0.5);
    TLegend *legRateScan_DataRate_Eta = new TLegend(0.125 ,0.875, 0.6, 0.5);


    std::vector<TMultiGraph *> mgRateScan_Eta(2, nullptr);
    std::vector<TMultiGraph *> mgRateScan_Radius(2, nullptr);
    std::vector<TMultiGraph *> mgClusterScan_Eta(2, nullptr);
    std::vector<TMultiGraph *> mgClusterScan_Radius(2, nullptr);

    std::vector<TMultiGraph *> mgClusterScan_DataRate_Eta(2, nullptr);
    std::vector<TMultiGraph *> mgClusterScan_DataRate_Radius(2, nullptr);

    std::vector<TMultiGraph *> mgRateScan_DataRate_Eta(2, nullptr);
    std::vector<TMultiGraph *> mgRateScan_DataRate_Radius(2, nullptr);

    for(unsigned int l=0;l<=1;l++){

        mgRateScan_Radius.at(l) = new TMultiGraph();
        mgRateScan_Eta.at(l) = new TMultiGraph();
        mgClusterScan_Radius.at(l) = new TMultiGraph();
        mgClusterScan_Eta.at(l) = new TMultiGraph();

        mgRateScan_DataRate_Radius.at(l) = new TMultiGraph();
        mgRateScan_DataRate_Eta.at(l) = new TMultiGraph();
        mgClusterScan_DataRate_Radius.at(l) = new TMultiGraph();
        mgClusterScan_DataRate_Eta.at(l) = new TMultiGraph();

        mgRateScan_Radius.at(l)->SetTitle(Form("Layer %d: Efficiency vs. radius;Radius (mm);Efficiency", l));
        mgClusterScan_Radius.at(l)->SetTitle(Form("Layer %d:Efficiency vs. radius;Radius (mm);Efficiency", l));
        mgRateScan_Eta.at(l)->SetTitle(Form("Layer %d: Efficiency vs. eta;Pseudo-rapidity #eta;Efficiency", l));
        mgClusterScan_Eta.at(l)->SetTitle(Form("Layer %d: Efficiency vs. eta;Pseudo-rapidity #eta;Efficiency", l));


        mgRateScan_DataRate_Radius.at(l)->SetTitle(Form("Layer %d: Data rate vs. radius;Radius (mm);Data rate (Mbit/s)", l));
        mgClusterScan_DataRate_Radius.at(l)->SetTitle(Form("Layer %d:Data rate vs. radius;Radius (mm);Data rate (Mbit/s)", l));
        mgRateScan_DataRate_Eta.at(l)->SetTitle(Form("Layer %d: Data rate vs. eta;Pseudo-rapidity #eta;Data rate (Mbit/s)", l));
        mgClusterScan_DataRate_Eta.at(l)->SetTitle(Form("Layer %d: Data rate vs. eta;Pseudo-rapidity #eta;Data rate (Mbit/s)", l));
    }


    std::vector<std::vector<TGraphAsymmErrors*>> gEffiVsRadius(2, std::vector<TGraphAsymmErrors*>());
    std::vector<std::vector<TGraphAsymmErrors*>> gEffiVsEta(2, std::vector<TGraphAsymmErrors*>() );
    for(int run=0;run<=8;run++){
        fda = new FOCALDetectorAnalysis(Form("/alf/data/focal/alpide_systemc/sim_output/run_%d", run), 25000);
        //analysis->FillHitmap();
        fda->SetSystemRate(SystemRate(run));
        fda->SetEventRate(1000);
        fda->AnalyseRULinks();
        //analysis->FillValidHits();

        for(unsigned int l=0;l<=1;l++){

            fda->GraphEfficiency_vs_Eta(l)->SetMarkerColor(uibColorIndex(run));
            fda->GraphEfficiency_vs_Radius(l)->SetMarkerColor(uibColorIndex(run));
            fda->GraphEfficiency_vs_Eta(l)->SetLineColor(uibColorIndex(run));
            fda->GraphEfficiency_vs_Radius(l)->SetLineColor(uibColorIndex(run));
            fda->GraphEfficiency_vs_Eta(l)->SetMarkerStyle(20+run);
            fda->GraphEfficiency_vs_Radius(l)->SetMarkerStyle(20+run);

            fda->GraphDataRate_vs_Eta(l)->SetMarkerColor(uibColorIndex(run));
            fda->GraphDataRate_vs_Radius(l)->SetMarkerColor(uibColorIndex(run));
            fda->GraphDataRate_vs_Eta(l)->SetLineColor(uibColorIndex(run));
            fda->GraphDataRate_vs_Radius(l)->SetLineColor(uibColorIndex(run));
            fda->GraphDataRate_vs_Eta(l)->SetMarkerStyle(20+run);
            fda->GraphDataRate_vs_Radius(l)->SetMarkerStyle(20+run);
  


            gEffiVsEta.at(l).push_back(fda->GraphEfficiency_vs_Eta(l));
            gEffiVsRadius.at(l).push_back(fda->GraphEfficiency_vs_Radius(l));

            if(run==0 || run>=5){
                
                mgClusterScan_Eta.at(l)->Add(fda->GraphEfficiency_vs_Eta(l));
                mgClusterScan_Radius.at(l)->Add(fda->GraphEfficiency_vs_Radius(l));
                mgClusterScan_DataRate_Eta.at(l)->Add(fda->GraphDataRate_vs_Eta(l));
                mgClusterScan_DataRate_Radius.at(l)->Add(fda->GraphDataRate_vs_Radius(l));
                if(!l){
                    legClusterScan_Eta->AddEntry(fda->GraphEfficiency_vs_Eta(l), Form("Cluster size %.0lf", ClusterSize(run)), "PL");
                    legClusterScan_Radius->AddEntry(fda->GraphEfficiency_vs_Eta(l), Form("Cluster size %.0lf", ClusterSize(run)), "PL");         
                    legClusterScan_DataRate_Eta->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("Cluster size %.0lf", ClusterSize(run)), "PL");
                    legClusterScan_DataRate_Radius->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("Cluster size %.0lf", ClusterSize(run)), "PL");
                }

            }
            if(run<=4){

                mgRateScan_Eta.at(l)->Add(fda->GraphEfficiency_vs_Eta(l));
                mgRateScan_Radius.at(l)->Add(fda->GraphEfficiency_vs_Radius(l));

                mgRateScan_DataRate_Eta.at(l)->Add(fda->GraphDataRate_vs_Eta(l));
                mgRateScan_DataRate_Radius.at(l)->Add(fda->GraphDataRate_vs_Radius(l));
                
                if(!l){     
                    double rate = SystemRate(run);
                    legRateScan_Eta->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("System rate %.0lf#mus / %d kHz", rate, (int) (1000000/rate) ), "PL");
                    legRateScan_Radius->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("System rate %.0lf#mus / %d kHz", rate, (int) (1000000/rate) ), "PL");

                    legRateScan_DataRate_Eta->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("System rate %.0lf#mus / %d kHz", rate, (int) (1000000/rate) ), "PL");
                    legRateScan_DataRate_Radius->AddEntry(fda->GraphDataRate_vs_Eta(l), Form("System rate %.0lf#mus / %d kHz", rate, (int) (1000000/rate) ), "PL");
                }
            }

        }
    }
    //fda->Draw();

    TCanvas *cRateRadius = new TCanvas("cRate_Radius", "Rate scan (radius)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cRateRadius->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cRateRadius->cd(l+1);
        gPad->SetRightMargin(0.02);
        mgRateScan_Radius.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legRateScan_Radius->Draw();
    }
    cRateRadius->SaveAs(Form("plots/%s.png", cRateRadius->GetName()));
    

    TCanvas *cRateEta = new TCanvas("cRate_Eta", "Rate scan (eta)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cRateEta->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cRateEta->cd(l+1);
        gPad->SetRightMargin(0.02);
        mgRateScan_Eta.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legRateScan_Eta->Draw();
    }
    cRateEta->SaveAs(Form("plots/%s.png", cRateEta->GetName()));


    TCanvas *cClusterRadius = new TCanvas("cCluster_Radius", "Cluster scan (radius)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cClusterRadius->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cClusterRadius->cd(l+1);
        gPad->SetRightMargin(0.02);        
        mgClusterScan_Radius.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legClusterScan_Radius->Draw();

    }
    cClusterRadius->SaveAs(Form("plots/%s.png", cClusterRadius->GetName()));

    

    TCanvas *cClusterEta = new TCanvas("cCluster_Eta", "Cluster scan (eta)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cClusterEta->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cClusterEta->cd(l+1);
        gPad->SetRightMargin(0.02);
    
        mgClusterScan_Eta.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legClusterScan_Eta->Draw();

    }
    cClusterEta->SaveAs(Form("plots/%s.png", cClusterEta->GetName()));


    TCanvas *cRateRadius_DataRate = new TCanvas("cRateRadius_DataRate", "Rate scan (radius)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cRateRadius_DataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cRateRadius_DataRate->cd(l+1);
        gPad->SetRightMargin(0.02);
        mgRateScan_DataRate_Radius.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legRateScan_DataRate_Radius->Draw();
    }
    cRateRadius_DataRate->SaveAs(Form("plots/%s.png", cRateRadius_DataRate->GetName()));

    

    TCanvas *cRateEta_DataRate = new TCanvas("cRateEta_DataRate", "Rate scan (eta)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cRateEta_DataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cRateEta_DataRate->cd(l+1);
        gPad->SetRightMargin(0.02);
        mgRateScan_DataRate_Eta.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legRateScan_DataRate_Eta->Draw();
    }
    cRateEta_DataRate->SaveAs(Form("plots/%s.png", cRateEta_DataRate->GetName()));



    TCanvas *cClusterRadius_DataRate = new TCanvas("cClusterRadius_DataRate", "Cluster scan (radius)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cClusterRadius_DataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cClusterRadius_DataRate->cd(l+1);
        gPad->SetRightMargin(0.02);        
        mgClusterScan_DataRate_Radius.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legClusterScan_DataRate_Radius->Draw();

    }
    cClusterRadius_DataRate->SaveAs(Form("plots/%s.png", cClusterRadius_DataRate->GetName()));

    

    TCanvas *cClusterEta_DataRate = new TCanvas("cClusterEta_DataRate", "Cluster scan (eta)", CANVAS_WIDTH*1.1,CANVAS_HEIGHT);
    cClusterEta_DataRate->Divide(2,1);
    for(unsigned int l=0;l<=1;l++){
        cClusterEta_DataRate->cd(l+1);
        gPad->SetRightMargin(0.02);
    
        mgClusterScan_DataRate_Eta.at(l)->Draw("ALP");
        gPad->SetGridx();gPad->SetGridy();
        legClusterScan_DataRate_Eta->Draw();

    }

    cClusterEta_DataRate->SaveAs(Form("plots/%s.png", cClusterEta_DataRate->GetName()));

    
        //analysis->Draw();
}

void analysis(){
    //singleAnalysis();
    fullAnalysis();
}
