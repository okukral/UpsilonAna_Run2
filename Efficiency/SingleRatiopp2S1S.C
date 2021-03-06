//Making code a littler cleaner
#include "effCommon.h"








double RError(double A, double eA, double B, double eB);
double PError(double A, double eA, double B, double eB);

const int  nPtBin = 3;
double m1S_low = 7.77;
double m1S_high = 10;
double m2S_low = 8.333;
double m2S_high = 10.563;


void SingleRatiopp2S1S(){
	cout<<"Started code"<<endl;
        gROOT->Macro("logon.C+");
	cout<<"set styles"<<endl;

        double          ptBin[nPtBin] = {2.5,8.5,21};   //  RapBin
        double          ptBinErr[nPtBin] = {2.5,3.5,9};
        Float_t         ptBinEdges[nPtBin+1] = {0.0,5.0,12.0,30.0};


	TH1D* hRecoNum;
	TH1D* hGenNum;
	TGraphAsymmErrors* hEffNum;

        TH1D* hRecoDen;
        TH1D* hGenDen;
        TGraphAsymmErrors* hEffDen;
	cout<<"declared variables"<<endl;
//        TH1D* hEffRatio = new TH1D("EffRatio", "", nPtBin, ptBinEdges);

        TGraphAsymmErrors* hEffpp2S1S = new TGraphAsymmErrors(nPtBin);
	hEffpp2S1S->SetName("hEffpp2S1S");
	cout<<"Make new histogram"<<endl;
//	hEffpp2S1S->Draw();


/////////// pp 2S

        TFile* fppEff2S = new TFile("ppEff2S.root", "Open");
        fppEff2S->GetObject("Reco", hRecoNum);
        fppEff2S->GetObject("Gen", hGenNum);
	fppEff2S->GetObject("Eff", hEffNum);

     //   fppEff2S->Close(); //closing before using. deleting from memory?
	cout<<"Loaded 2S efficiencies"<<endl;

////////// pp 1S

	TFile* fppEff1S = new TFile("ppEff1S.root", "Open");
        fppEff1S->GetObject("Reco", hRecoDen);
        fppEff1S->GetObject("Gen", hGenDen);
        fppEff1S->GetObject("Eff", hEffDen);

       // fppEff1S->Close(); //closing before using. deleting from memory?


///////// Single Ratio 2S/1S Calculation

        double EffRatio;
        double EffNum;
        double EffNumErrH;
        double EffDenErrH;
        double EffNumErrL;
        double EffDenErrL;
        double EffDen;
        //previously not defined
	double EffRatioErrH;
        double EffRatioErrL;

//                hEffRatio->Divide(hEffNum, hEffDen);


        for (Int_t i = 0; i < (nPtBin); i++){
		cout<<"Started loop: i = "<<i<<endl;
                EffNum = hEffNum->Eval(ptBin[i]);
                cout<<"Grabbed first bin of the numerator"<<endl;
		EffDen = hEffDen->Eval(ptBin[i]);
                cout<<"Calculated individual efficiencies"<<endl;
                EffNumErrH = hEffNum->GetErrorYhigh(i);
                EffNumErrL = hEffNum->GetErrorYlow(i);
                EffDenErrH = hEffDen->GetErrorYhigh(i);
                EffDenErrL = hEffDen->GetErrorYlow(i);
		cout<<"Calculated individual errors"<<endl;
		EffRatio = EffNum / EffDen;
		cout<<"Calculated ratio of efficiencies"<<endl;
                EffRatioErrH = RError(EffNum, EffNumErrH, EffDen, EffDenErrH);  //Calculation for the combined efficiency	
                EffRatioErrL = RError(EffNum, EffNumErrL, EffDen, EffDenErrL); //typo EffL -> ErrL
		cout<<"Calculated error ratios"<<endl;

                hEffpp2S1S->SetPoint((i), ptBin[i], EffRatio);
                hEffpp2S1S->SetPointError((i), ptBinErr[i], ptBinErr[i], EffRatioErrL, EffRatioErrH);

//		hist->SetBinContent(i, EffRatio);
//		hist->SetBinError

        }



	TFile* OutFile;
        OutFile = new TFile("EffSingleRatiopp.root", "Recreate");
        hEffpp2S1S->Write();

        OutFile->Close();


        TCanvas* can1 = new TCanvas("can1", "Canvas with results1", 1000, 680);

	//adding a line
	TLine* line1 = new TLine(0,1,30,1);
        line1->SetLineStyle(kDashed);

        hEffpp2S1S->SetMarkerSize(2.0);
        hEffpp2S1S->SetMarkerColor(kRed);
        hEffpp2S1S->SetMarkerStyle(21);
	hEffpp2S1S->SetLineColor(kRed);
        hEffpp2S1S->GetXaxis()->SetTitle("p^{#mu+#mu-}_{T}");
        hEffpp2S1S->GetXaxis()->CenterTitle();
	hEffpp2S1S->GetYaxis()->CenterTitle();

	hEffpp2S1S->GetYaxis()->SetTitle("Efficiency[#varUpsilon(2S)/#varUpsilon(1S)]_{pp}");
	hEffpp2S1S->GetYaxis()->SetRangeUser(0.5, 1.5);
	hEffpp2S1S->GetXaxis()->SetRangeUser(0.0, 30.0);
	hEffpp2S1S->GetXaxis()->SetTitleSize(0.05);
	hEffpp2S1S->GetXaxis()->SetTitleOffset(0.9);
	hEffpp2S1S->GetYaxis()->SetTitleSize(0.05);
	hEffpp2S1S->GetYaxis()->SetTitleOffset(0.9);
	hEffpp2S1S->Draw("AP");
	line1->Draw("SAME");

        can1->Update();
        can1->SaveAs("SingleRatioVsPtpp2S1S.png");

        for (Int_t i = 0; i < (nPtBin); i++){
        cout << hEffpp2S1S->Eval(ptBin[i]) << " , - " << hEffpp2S1S->GetErrorYlow(i) << " , + " << hEffpp2S1S->GetErrorYhigh(i) << endl;
        }

	  
        cout << "over" << endl;
	
	fppEff1S->Close(); 
	fppEff2S->Close(); 

/*
        TCanvas* can2 = new TCanvas("can2", "Canvas with results2", 1000, 680);
        hgEffpp2S->SetMarkerSize(2.0);
        hgEffpp2S->SetMarkerColor(kBlue);
        hgEffpp2S->SetMarkerStyle(20);
        hgEffpp2S->SetLineColor(kBlue);
        hgEffpp2S->GetXaxis()->SetTitle("p_{T}");
        hgEffpp2S->GetYaxis()->SetTitle("Efficiency");
        hgEffpp2S->GetYaxis()->SetRangeUser(0, 1);
        hgEffpp2S->GetXaxis()->SetTitleSize(0.05);
        hgEffpp2S->GetXaxis()->SetTitleOffset(0.9);
        hgEffpp2S->GetYaxis()->SetTitleSize(0.05);
        hgEffpp2S->GetYaxis()->SetTitleOffset(0.9);
        hgEffpp2S->Draw("AP");

        hgEffpp1S->SetMarkerSize(2.0);
        hgEffpp1S->SetMarkerColor(kRed);
        hgEffpp1S->SetMarkerStyle(21);
        hgEffpp1S->SetLineColor(kRed);
        hgEffpp1S->Draw("P");
// */


}




//Ratio Error Propogation
double RError(double A, double eA, double B, double eB){
	 double f=A/B;
	 double fA=eA/A;
	 double fB=eB/B;
	 double eR=  f*sqrt( (fA*fA + fB*fB )) ;
	 return eR;
}


//Product Error Propogation
double PError(double A, double eA, double B, double eB){
	 double f=A*B;
	 double fA=eA/A;
	 double fB=eB/B;
	 double eR=  f*sqrt( (fA*fA + fB*fB )) ;
	 return eR;
}


