#include "functions.C"
#include "RooGlobalFunc.h"
#include "RooWorkspace.h"

using namespace RooFit; //new

RooWorkspace* test_combine(const char* name_pbpb="chad_ws_fits/centFits/ws_PbPbData_262548_263757_80cent100_0.0pt50.0_0.0y2.4.root", const char* name_pp="chad_ws_fits/centFits/ws_PPData_262157_262328_-1cent1_0.0pt50.0_0.0y2.4.root")
{
  // const char *poiname="N_{#Upsilon(3S)}";
  TFile *f = new TFile(name_pbpb) ;
  TFile *f_pp = new TFile(name_pp) ;

  // Retrieve workspace from file
  RooWorkspace* ws = (RooWorkspace*) f->Get("myws"); //change workspace name
  RooWorkspace* ws_pp = (RooWorkspace*) f_pp->Get("myws"); //change workspace name


  // RooRealVar *theVar; 
  RooDataSet *data; //RooAbsPdf *pdf;
  RooRealVar *theVar_pp; RooDataSet *data_pp; RooAbsPdf *pdf_pp;

  // theVar = ws->var(poiname);
  //pdf = ws->pdf("pdf");
  data = (RooDataSet *) ws->data("dataOS"); //dataOS, dataSS
  pdf_pp = ws_pp->pdf("pdf");
  data_pp = (RooDataSet *) ws_pp->data("dataOS"); //dataOS, dataSS

  RooCategory dataCat("dataCat", "dataCat");
  dataCat.defineType("hi");
  dataCat.defineType("pp");

  RooRealVar muppt("muPlusPt" ,"#mu+ pt",2,20,"GeV/c"); 
  RooRealVar mumpt("muMinusPt","#mu- pt",2,20,"GeV/c"); 
  RooRealVar dimuPt("dimuPt","p_{T}(#DiMuon)",0.,"GeV/c"); //RooRealVar upsPt("upsPt","p_{T}(#Upsilon)",0.,"GeV/c");
  RooRealVar vProb("vProb","vProb",0.05,1);
  //RooRealVar upsEta("upsEta","#eta(#Upsilon)",0.,"");
  RooRealVar dimuRapidity("dimuRapidity", "dimuRapidity", 0.); //RooRealVar upsRapidity("upsRapidity", "upsRapidity", 0.);
  RooCategory QQsign("QQsign", "QQsign");
  QQsign.defineType("PlusMinus", 0);
  QQsign.defineType("PlusPlus", 1);
  QQsign.defineType("MinusMinus", 2);
  RooRealVar Centrality("Centrality", "Centrality", 0., 200);
  RooRealVar * mass = ws->var("invariantMass");
  if (!mass) {
    mass = new RooRealVar("invariantMass", "#mu#mu mass", mmin, mmax, "GeV/c^{2}");
  }

  RooArgSet cols(*mass, muppt, mumpt, dimuPt, vProb, dimuRapidity, QQsign, Centrality); //RooArgSet cols(*mass, muppt, mumpt, dimuPt, vProb, dimuRapidity, QQsign, Centrality);

  RooDataSet data_combo("data", "data", cols, RooFit::Index(dataCat), RooFit::Import("hi", *data), RooFit::Import("pp", *data_pp));
      /*Only for track rotation*/
      //RooFit::Import("hi", *data), RooFit::Import("pp", *data_pp));

  RooWorkspace *wcombo = new RooWorkspace("wcombo","workspace for PbPb + pp");
  wcombo->import(data_combo);
  wcombo->import(*pdf_pp, RooFit::RenameAllNodes("pp"),
      RooFit::RenameAllVariablesExcept("pp", 
        //"invariantMass,"//npow->n_{CB}
        //"prior,"
        //"mean,"
        //"turnOn,"
        //"f23,f3o2,"
        ""
        //"x23,x3o2,"
        //"alpha,"
        //"sigma1"
        ), 
      RooFit::RecycleConflictNodes());

  // // create the combined variable
  // RooRealVar* n3shi = wcombo->var("N_{#Upsilon(3S)}_hi");
  // RooRealVar* n3spp = wcombo->var("N_{#Upsilon(3S)}_pp");
  // RooFormulaVar x3raw("x3raw","x3raw","@0/@1",RooArgList(*n3shi,*n3spp));
  // cout << x3raw.getVal() << endl;
  // wcombo->import(x3raw);
  // wcombo->Print();

  RooAbsPdf *sig1S = ws->pdf("sig1S"); //RooAbsPdf *sig1S = ws->pdf("cbcb");
  RooAbsPdf *sig2S = ws->pdf("sig2S");
  RooAbsPdf *sig3S = ws->pdf("sig3S");
  RooAbsPdf *pdf_combinedbkgd = ws->pdf("bkgPdf");
  RooRealVar *nsig1f = ws->var("N_{#varUpsilon(1S)}"); //RooRealVar *nsig1f = ws->var("N_{#Upsilon(1S)}");
  RooRealVar *f2Svs1S = ws->var("R_{#frac{2S}{1S}}"); //RooRealVar *nsig2f = ws->var("N_{#Upsilon(2S)}");
  RooRealVar *f3Svs1S = ws->var("R_{#frac{3S}{1S}}"); //RooRealVar *nsig2f = ws->var("N_{#Upsilon(2S)}");
  //RooAbsReal *nsig2f = ws->function("N_{ #varUpsilon(2S)}"); //RooRealVar *nsig1f = ws->var("N_{#Upsilon(1S)}");
  //RooAbsReal *nsig3f = ws->function("N_{ #varUpsilon(3S)}"); //RooRealVar *nsig1f = ws->var("N_{#Upsilon(1S)}");
  RooFormulaVar *nsig2f = new RooFormulaVar("N_{ #varUpsilon(2S)}","@0*@1",RooArgList(*f2Svs1S,*nsig1f));
  RooFormulaVar *nsig3f = new RooFormulaVar("N_{ #varUpsilon(3S)}","@0*@1",RooArgList(*f3Svs1S,*nsig1f));
  RooRealVar *nbkgd = ws->var("n_{Bkgd}");
  // RooRealVar *x3raw = new RooRealVar("x3raw","x3raw",7e-4,-10,10);

  // for pp setup
/*  RooAbsPdf *sig1S_pp = ws_pp->pdf("sig1S"); sig1S->SetName("sig1S_pp"); 
  RooAbsPdf *sig2S_pp = ws_pp->pdf("sig2S"); sig2S->SetName("sig2S_pp");
  RooAbsPdf *sig3S_pp = ws_pp->pdf("sig3S"); sig3S->SetName("sig3S_pp");
  RooAbsPdf *pdf_combinedbkgd_pp = ws_pp->pdf("bkgPdf"); pdf_combinedbkgd->SetName("pdf_combinedbkgd_pp");
  RooRealVar *nsig1f_pp = ws_pp->var("N_{#varUpsilon(1S)}"); nsig1f->SetName("nsig1f_pp");
  RooRealVar *nsig2f_pp = ws_pp->var("R_{#frac{2S}{1S}}"); nsig2f->SetName("nsig2f_pp");
  RooRealVar *nsig3f_pp = ws_pp->var("R_{#frac{3S}{1S}}"); nsig3f->SetName("nsig3f_pp");*/

  //RooRealVar *nsig3f_pp = ws_pp->var("R_{#frac{3S}{1S}}"); nsig3f_pp->SetName("N_{#Upsilon(3S)}_pp"); 
  //RooFormulaVar *nsig3f_new = new RooFormulaVar("N_{ #varUpsilon(3S)}","@0*@1",RooArgList(*nsig3f,*nsig1f));

  RooAbsPdf *pdf = new RooAddPdf ("pdf","new total p.d.f.",
      RooArgList(*sig1S,*sig2S,*sig3S,*pdf_combinedbkgd),
      RooArgList(*nsig1f,*nsig2f,*nsig3f,*nbkgd));
  wcombo->import(*pdf, RooFit::RenameAllNodes("hi"),
      RooFit::RenameAllVariablesExcept("hi", 
        //"n_{CB},invariantMass,"
        //"prior,"
        //"mean,"
        //"turnOn,"
        // "f23,f3o2,"
        ""
        //"x23,x3o2,"
        //"alpha,"
        //"sigma1,"
        //"x3raw,R_{#frac{3S}{1S}}_pp" //"x3raw,N_{#Upsilon(3S)}_pp"
        ), 
      RooFit::RecycleConflictNodes());
  //wcombo->Print();
  RooSimultaneous* simPdf = buildSimPdf(*wcombo,dataCat);
  wcombo->Print();

  // not sure this is really needed since we will fit again in the later workspace creation
  RooFitResult* fit_2nd;// fit results
  fit_2nd = simPdf->fitTo(data_combo,
      // RooFit::Constrained(),
      RooFit::Save(kTRUE),
      RooFit::Extended(kTRUE),
      RooFit::Minos(kTRUE),
      RooFit::NumCPU(50));


  // fix all other variables in model:
  // everything except observables, POI, and nuisance parameters
  // must be constant
  wcombo->var("#alpha_{CB}_hi")->setConstant(true);
  wcombo->var("#alpha_{CB}_pp")->setConstant(true);
  wcombo->var("#sigma_{CB1}_hi")->setConstant(true);
  wcombo->var("#sigma_{CB1}_pp")->setConstant(true);
  wcombo->var("#sigma_{CB2}/#sigma_{CB1}_hi")->setConstant(true);
  wcombo->var("#sigma_{CB2}/#sigma_{CB1}_pp")->setConstant(true);
//  wcombo->var("N_{ #varUpsilon(1S)}_hi")->setConstant(true);
//  wcombo->var("N_{ #varUpsilon(1S)}_pp")->setConstant(true);
//  wcombo->function("N_{ #varUpsilon(2S)}_hi");
//  wcombo->function("N_{ #varUpsilon(2S)}_pp");
//  wcombo->function("N_{ #varUpsilon(3S)}_hi");
//  wcombo->function("N_{ #varUpsilon(3S)}_pp");
  wcombo->var("N_{#varUpsilon(1S)}_hi")->setConstant(true);
  wcombo->var("N_{#varUpsilon(1S)}_pp")->setConstant(true);
  wcombo->var("R_{#frac{2S}{1S}}_hi")->setConstant(true); //new
  wcombo->var("R_{#frac{2S}{1S}}_pp")->setConstant(true); //new
  wcombo->var("R_{#frac{3S}{1S}}_hi")->setConstant(true); //new
  wcombo->var("R_{#frac{3S}{1S}}_pp")->setConstant(true); //new
  wcombo->var("decay_hi")->setConstant(true);
  wcombo->var("decay_pp")->setConstant(true);
  wcombo->var("m_{ #varUpsilon(1S)}_hi")->setConstant(true);
  wcombo->var("m_{ #varUpsilon(1S)}_pp")->setConstant(true);
  wcombo->var("n_{Bkgd}_hi")->setConstant(true);
  wcombo->var("n_{Bkgd}_pp")->setConstant(true);
  //wcombo->var("n_{CB}")->setConstant(true); //wcombo->var("npow")->setConstant(true);
  wcombo->var("n_{CB}_hi")->setConstant(true); //wcombo->var("npow")->setConstant(true);
  wcombo->var("n_{CB}_pp")->setConstant(true); //wcombo->var("npow")->setConstant(true);
  wcombo->var("sigmaFraction_hi")->setConstant(true);
  wcombo->var("sigmaFraction_pp")->setConstant(true);
  wcombo->var("turnOn_hi")->setConstant(true);
  wcombo->var("turnOn_pp")->setConstant(true);
  wcombo->var("width_hi")->setConstant(true);
  wcombo->var("width_pp")->setConstant(true);


  wcombo->writeToFile("fitresult_combo_80_100.root");
  return wcombo;
}
