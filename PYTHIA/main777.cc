// MY DIRE SIMULATION PROGRAM
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details. 
// Please respect the MCnet Guidelines, see GUIDELINES for details.
// Author: Stefano Veroni (based on dire09.cc given by tutor A.Bressan)
// 09/08/2021
// Keywords: DIRE, LHC, ROOT TH1, ROOT TTREE
// Running lines:
// make main777
// ./main777 main777.cmnd > main777.out
// Simulates the parton shower generated by a hard scattering between an
// incoming leptonic Abeam and a quark in a nucleonic Bbeam. 

// PYTHIA
#include "Pythia8/Pythia.h"
#include "Pythia8/Dire.h"

// Generic Packages
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cmath>

// ROOT functionalities
#include "TApplication.h"
#include "TBox.h"
#include "TButton.h"
#include "TCanvas.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TColor.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"
#include "TMath.h"
#include "TPad.h"
#include "TParticlePDG.h"
#include "TPostScript.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TRotation.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TText.h"
#include "TTree.h"
#include "TVector3.h"
#include "TVirtualPad.h"
#include "TVirtualPS.h"
#include "Riostream.h"


using namespace Pythia8;

//============================================================================

int main( int argc, char* argv[] ){



  //==========================================================================
  //PREPARATION    PREPARATION    PREPARATION    PREPARATION    PREPARATION 
  //==========================================================================
  // Save estimates in vectors.
  vector<double> xsecLO;
  vector<double> nSelectedLO;
  vector<double> nAcceptLO;
  vector<int>    strategyLO;

  static UInt_t   Evt;     //  event number
  int    TrgMsk = 0 ;
  int    SelV   = 0 ; 
  
  //Many Not-A-Number Variables, to be given value afterwards  
  float  Zprim  = nan("1");   //Coordinates of primary vertex   
  float  Xprim  = nan("1");   
  float  Yprim  = nan("1");  
  float  theta  = nan("1");   //Lepton scattering angle theta
 
  // Suffixes: _p->4-mom, ph->azimuthal phi, th->polar theta, tr->true values
  // The differnce between true values and reconstructed is that true values
  // take into account possible emissions of photons, as far as possible for
  // PYTHIA 8.306, before the hard scattering process, hence resulting into a 
  //different value of kinematic variables.
  float  aeam_p   = nan("1");   //Incoming A beam (lab)   
  float  aeamph   = nan("1");         
  float  aeamth   = nan("1");
  float  beam_p   = nan("1");   //Incoming B beam (lab)       
  float  beamph   = nan("1");         
  float  beamth   = nan("1");         
  float  outlep_p = nan("1");   //Outgoing lepton (lab)
  float  outlepph = nan("1");
  float  outlepth = nan("1"); 
  float  gaene    = nan("1");   //Virtual photon (lab)
  float  gathe    = nan("1");
  float  gaphi    = nan("1");
  float  phi_s    = nan("1");   //SPIN phi GNS (Gamma Nucleon System)
  float  cosvp    = nan("1");
  float  gathx    = nan("1");   //Lab frame gamma x angle
  float  gathy    = nan("1");   //lab frame gamma y angle
  float  bcm      = nan("1");   //Reconstructed Lorentz boost
  float  gcm      = nan("1");   //Reconstructed Lorentz gamma
  float  phr_p    = nan("1");
  float  phrth    = nan("1");
  float  phrph    = nan("1");
  float  nu       = nan("1");   //Values of kinematic variables
  float  Q2       = nan("1");
  float  xbj      = nan("1");
  float  y        = nan("1");
  float  W        = nan("1");
  float  nutr     = nan("1");   //True Values of kinematic variables
  float  Q2tr     = nan("1");
  float  xbjtr    = nan("1");
  float  ytr      = nan("1");
  float  Wtr      = nan("1");  
  float  str      = nan("1");  //Mandelstam s (Total 4P^2) (True Value)
  float  ttr      = nan("1");  //Mandelstam t (True Value)

  Int_t  HadNb   = 0 ;
  Int_t  GamNb   = 0 ;
  Int_t  Pi0Nb   = 0 ;

  //Hadrons' Shower (note h suffix)
  const Int_t Hmax=50;
  int    SelH  [Hmax];
  Int_t  ch    [Hmax]; //identity
  float  zh    [Hmax]; //z variable
  float  eh    [Hmax]; //energy
  float  ph    [Hmax]; //momentum
  float  pth   [Hmax];
  //float  xfh   [Hmax];
  float  etah  [Hmax];
  float  phi_h [Hmax];
  float  theha [Hmax];
  float  phiha [Hmax];

  
  //Photons (note g suffix)
  //const Int_t Gmax=50;
  //Int_t  cg    [Gmax];
  //float  zg    [Gmax];
  //float  eg    [Gmax];
  //float  pxg   [Gmax];
  //float  pyg   [Gmax];
  //float  pzg   [Gmax];
  //float  ptg   [Gmax];

 static TTree* tree(NULL);
 TRandom * r1 = new TRandom(1);
 TRandom * r2 = new TRandom(1);
 TRandom * r3 = new TRandom(1);
 
 
 //Create the TTree
 tree = new TTree("dis","DIS tree"); // name (has to be unique) and title of the Ntuple
 
 //Branches (branch name, address for variable to be read, leafname/<type>)
 tree->Branch("Evt"       ,&Evt       ,"Evt/i"          );
 tree->Branch("SelV"      ,&SelV      ,"SelV/i"         );
 tree->Branch("Xprim"     ,&Xprim     ,"Xprim/F"        );
 tree->Branch("Yprim"     ,&Yprim     ,"Yprim/F"        );
 tree->Branch("Zprim"     ,&Zprim     ,"Zprim/F"        );
 tree->Branch("theta"     ,&theta     ,"theta/F"        );
 tree->Branch("beam_p"    ,&beam_p    ,"beam_p/F"       );// incoming B beam
 tree->Branch("beamph"    ,&beamph    ,"beamph/F"       );
 tree->Branch("beamth"    ,&beamth    ,"beamth/F"       );
 tree->Branch("aeam_p"    ,&aeam_p    ,"aeam_p/F"       );// incoming A beam
 tree->Branch("aeamph"    ,&aeamph    ,"aeamph/F"       );
 tree->Branch("aeamth"    ,&aeamth    ,"aeamth/F"       );
 tree->Branch("outlep_p"  ,&outlep_p  ,"aupr_p/F"       );//outgoing mu 	  
 tree->Branch("outlepph"  ,&outlepph  ,"outlepph/F"     );
 tree->Branch("outlepth"  ,&outlepth  ,"outlepth/F"     );
 tree->Branch("gaene"     ,&gaene     ,"gaene/F"        );// virtual photon
 tree->Branch("gaphi"     ,&gaphi     ,"gaphi/F"        );
 tree->Branch("gathe"     ,&gathe     ,"gathe/F"        );
 tree->Branch("phi_s"     ,&phi_s     ,"phi_s/F"        );// SPIN phi  GNS 
 tree->Branch("bcm"       ,&bcm       ,"bcm/F"          );// Lorentz Beta    
 tree->Branch("gcm"       ,&gcm       ,"gcm/F"          );// Lorentz Gamma 
 tree->Branch("nu"        ,&nu        ,"nu/F"           );// nu (GeV)
 tree->Branch("Q2"        ,&Q2        ,"Q2/F"           );// Q2 (GeV/c)^2
 tree->Branch("xbj"       ,&xbj       ,"xbj/F"          );// xbj
 tree->Branch("y"         ,&y         ,"y/F"            );// y
 tree->Branch("W"         ,&W         ,"W/F"            );// W  
 tree->Branch("nutr"      ,&nutr      ,"nutr/F"         );// true nu (GeV)
 tree->Branch("Q2tr"      ,&Q2tr      ,"Q2tr/F"         );// true Q2 (GeV/c)^2
 tree->Branch("xbjtr"     ,&xbjtr     ,"xbjtr/F"        );// true xbj
 tree->Branch("ytr"       ,&ytr       ,"ytr/F"          );// true y
 tree->Branch("Wtr"       ,&Wtr       ,"Wtr/F"          );// true W 
 tree->Branch("str"       ,&str       ,"str/F"          );// Mandelstam s(true)
 tree->Branch("ttr"       ,&ttr       ,"ttr/F"          );// Mandelstam t(true)
 tree->Branch("cosvp"     ,&cosvp     ,"cosvp/F"        );
 tree->Branch("gathx"     ,&gathx     ,"gathx/F"        );
 tree->Branch("gathy"     ,&gathy     ,"gathy/F"        );
 tree->Branch("phr_p"     ,&phr_p     ,"phr_p/F"	  );
 tree->Branch("phrth"     ,&phrth     ,"phrth/F"        );
 tree->Branch("phrph"     ,&phrph     ,"phrph/F"        );
  
 // hadrons (0<i<HadNb)				       
 tree->Branch("HadNb"   ,&HadNb   ,"HadNb/I"        );// Tot number of hadrons
 tree->Branch("SelH"    , SelH    ,"SelH[HadNb]/I"  );
 tree->Branch("ch"      , ch      ,"ch[HadNb]/I"    );// hadron i jetset ID
 tree->Branch("zh"      , zh      ,"zh[HadNb]/F"    );// hadron i z
 tree->Branch("eh"      , eh      ,"eh[HadNb]/F"    );// hadron i Lab energy
 tree->Branch("ph"      , ph      ,"ph[HadNb]/F"    );// hadron i Lab momentum
 tree->Branch("theha"   , theha   ,"theha[HadNb]/F" );// hadron i polar theta
 tree->Branch("phiha"   , phiha   ,"phiha[HadNb]/F" );// hadron i azimut phi
 //tree->Branch("xfh"     , xfh     ,"xfh[HadNb]/F"   );// hadron i x_Feynmann  
 tree->Branch("etah"    , etah    ,"etah[HadNb]/F"  );// hadron i pseudo-rapidity
 tree->Branch("pth"     , pth     ,"pth[HadNb]/F"   );// hadron i GNS P_hT
 tree->Branch("phi_h"   , phi_h   ,"phi_h[HadNb]/F" );// hadron i GNS azimut phi
 
 // photons					       
 //tree->Branch("GamNb"   ,&GamNb   ,"GamNb/I"        );
 //tree->Branch("cg"      , cg      ,"cg[GamNb]/I"    );
 //tree->Branch("zg"      , zg      ,"zg[GamNb]/F"    );
 //tree->Branch("eg"      , eg      ,"eg[GamNb]/F"    );
 //tree->Branch("pxg"     , pxg     ,"pxg[GamNb]/F"   );
 //tree->Branch("pyg"     , pyg     ,"pyg[GamNb]/F"   );
 //tree->Branch("pzg"     , pzg     ,"pzg[GamNb]/F"   );
 //tree->Branch("ptg"     , ptg     ,"ptg[GamNb]/F"   );
 
 
 //Doubles, 4Vecs and 3Vecs for later analysis
 
 TLorentzVector lvh     (0,0,0,0);
 TLorentzVector hrd_p   (0,0,0,0);
 TLorentzVector l_esse  (0,0,0,0);
 TLorentzVector l_nuc_i  (0,0,0,0);      //4p of proton
 TLorentzVector l_MM    (0,0,0,0);      
 TLorentzVector l_lep_i  (0,0,0,0);      //4p of incoming lepton 
 TLorentzVector l_lep_f  (0,0,0,0);      //4p of outcoming lepton
 TLorentzVector p_cms   (0,0,0,0);
 TLorentzVector Gamma   (0.,0.,0.,0.);  //4p of virtual photon
 TLorentzVector Gammatr (0.,0.,0.,0.);  //true 4p of virtual photon
 TLorentzVector dummyl_ (0.,0.,0.,0.);
 TLorentzVector lSpin   (0.,1.,0.,0.);
 
 TVector3 p1,p2     ;
 TVector3 iv(1.,0.,0.); //Basis Vectors
 TVector3 jv(0.,1.,0.);
 TVector3 kv(0.,0.,1.);
 
 Double_t boostGNS      ,boostGNStr   ; 
 Double_t sinphis_GNS   ,cosphis_GNS  ;
 Double_t sinphihp_GNS  ,cosphihp_GNS ;
 Double_t sinphih_GNS   ,cosphih_GNS  , sinphih_GNS2;
 
 TVector3 xx   ,yy   ,zz   ,xxtr   ,yytr   ,zztr;
 TVector3 xxl  ,yyl  ,zzl  ;
 TVector3 xxGNS,yyGNS,zzGNS,xxGNStr,yyGNStr,zzGNStr;
 TVector3 BoostGNS, BoostGNStr;
 TVector3 SB,SBtr,HB,HBtr,HP;
 TRotation GNS, GNStr;
 TLorentzVector GammaGNS, l_lep_iGNS, l_lep_fGNS, l_nuc_iGNS, GammaGNStr, l_lep_iGNStr, l_lep_fGNStr;
 TLorentzVector lSpinGNS, lSpinGNStr;
 TLorentzVector v_fot,i_lep,f_lep,v_fottr,i_leptr,f_leptr,hrd_pGNS,f_had,f_hadtr;
 TLorentzVector lvhGNS,lvhGNStr,lvhrealGNS,lvhrealGNStr;
 
 
 
  //=========================================================================
  //INITIALIZATION    INITIALIZATION    INITIALIZATION    INITIALIZATION  ===   
  //=========================================================================
  
  Pythia pythia;
  pythia.readFile  (argv[1]);
  
  int nEvent = pythia.mode("Main:numberOfEvents");



  //=========================================================================
  // CROSS SECTION ESTIMATE RUN =============================================
  //=========================================================================
  
  //Switch OFF all showering and MPI when estimating the cross section, 
  //then re-initialise (unfortunately).
  bool fsr = pythia.flag("PartonLevel:FSR");
  bool isr = pythia.flag("PartonLevel:ISR");
  bool mpi = pythia.flag("PartonLevel:MPI");
  bool had = pythia.flag("HadronLevel:all");
  bool rem = pythia.flag("PartonLevel:Remnants");
  bool chk = pythia.flag("Check:Event");
  int nCount = pythia.settings.mode("Next:numberCount");
  pythia.settings.flag("PartonLevel:FSR",     false);
  pythia.settings.flag("PartonLevel:ISR",     false);
  pythia.settings.flag("PartonLevel:MPI",     false);
  pythia.settings.flag("HadronLevel:all",     false);
  pythia.settings.flag("PartonLevel:Remnants",false);
  pythia.settings.flag("Check:Event",         false);
  pythia.settings.mode("Next:numberCount",nEvent);
  pythia.init();
  
  double sumSH     = 0.;
  double nAcceptSH = 0.;
  
  for( int iEvent=0; iEvent<nEvent; ++iEvent ){
    
    if( !pythia.next() ) {
      if( pythia.info.atEndOfFile() )
        break;
      else continue;
      }

    sumSH     += pythia.info.weight();
    
    //Build a map from a certain string to a certain other string
    map <string,string> eventAttributes;
    //pythia.info.eventAttributes is a pointer to a <string,string> map 
    //and public attribute of pythia::info
    if (pythia.info.eventAttributes) {
      eventAttributes = *(pythia.info.eventAttributes); //retrieve the map
      string trials = (eventAttributes.find("trials") != eventAttributes.end())
                      ?  eventAttributes["trials"] : "";
      if (trials != "") {nAcceptSH += atof (trials.c_str()) ;}
     
     xsecLO.push_back(pythia.info.sigmaGen(iEvent));
     nAcceptLO.push_back(pythia.info.nAccepted(iEvent));
    }
  }
  
  pythia.stat();

  int nAccept   = pythia.info.nAccepted(); //accepted events by pythia and user
  double xs     = pythia.info.sigmaGen();  //estimated cross section
  

  
  //===========================================================================
  // EVENT GENERATION LOOP=====================================================
  //===========================================================================
   
  pythia.settings.mode("Next:numberCount", nCount);

  // Cross section and error.
  double sigmaTotal  = 0.;
  double errorTotal  = 0.;

  double sigmaSample = 0., errorSample = 0.;
  
  // Switch showering and multiple interaction back to original setting.
  pythia.settings.flag("PartonLevel:FSR",fsr);
  pythia.settings.flag("PartonLevel:ISR",isr);
  pythia.settings.flag("HadronLevel:all",had);
  pythia.settings.flag("PartonLevel:MPI",mpi);
  pythia.settings.flag("PartonLevel:Remnants",rem);
  pythia.settings.flag("Check:Event",chk);
  pythia.init();

  double wmax    =-1e15; 
  double wmin    = 1e15;
  double sumwt   = 0.;
  double sumwtsq = 0.;
  double wtcount = 0.;
  
  // Root-Histogram the weights
  TH1F *histWT = new TH1F("histWT", "Weights", 5000000, -1000., 1000.);
  
  for( int iEvent=0; iEvent<nEvent; ++iEvent ){
  
  //Set to NAN before every event
    TrgMsk = 0 ;
    SelV   = 0 ;        
    Zprim  = nan("1");     
    Xprim  = nan("1");     
    Yprim  = nan("1");     
    
    aeam_p = nan("1");         
    aeamph = nan("1");         
    aeamth = nan("1"); 
    beam_p = nan("1");         
    beamph = nan("1");         
    beamth = nan("1");                 
    outlep_p = nan("1");
    outlepph = nan("1");
    outlepth = nan("1");
    gaene  = nan("1");
    gathe  = nan("1");
    gaphi  = nan("1");
    phi_s  = nan("1");
    cosvp  = nan("1");
    gathx  = nan("1");
    gathy  = nan("1");
    bcm    = nan("1");
    gcm    = nan("1");
    nu     = nan("1");
    Q2     = nan("1");
    xbj    = nan("1");
    y      = nan("1");
    W      = nan("1");
    phr_p  = nan("1");
    phrth  = nan("1");
    phrph  = nan("1");
   
    HadNb   = 0 ;
    GamNb   = 0 ;
    
    for (int ik=0;ik<Hmax;ik++) {
      SelH   [ik] = 0;
      ch     [ik] = 0;
      zh     [ik] = nanf("1");
      eh     [ik] = nanf("1");
      ph     [ik] = nanf("1");
      pth    [ik] = nanf("1");
      //xfh    [ik] = nanf("1");
      etah   [ik] = nanf("1");
      phi_h  [ik] = nanf("1");
      theha  [ik] = nanf("1");
      phiha  [ik] = nanf("1");
    }
    
    //for (int ik=0;ik<Gmax;ik++) {
    //  cg     [ik] = 0        ;
    //  zg     [ik] = nanf("1");
    //  eg     [ik] = nanf("1");
    //  pxg    [ik] = nanf("1");
    //  pyg    [ik] = nanf("1");
    //  pzg    [ik] = nanf("1");
    //  ptg    [ik] = nanf("1");
    //}


    if( !pythia.next() ) {
      if( pythia.info.atEndOfFile() )
        break;
      else continue;
    }
    
    //-----------------------------------------------------------------------
    //WEIGHTS ---------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Get event weight(s).
    double evtweight         = pythia.info.weight();

    if (abs(evtweight) > 1e3) {
      cout << scientific << setprecision(8)
      << "Warning: Large shower weight wt = " << evtweight << endl;
      if (abs(evtweight) > 1e4) { 
        cout << "Warning: Shower weight larger than 10000."
        << "Discard event with rare shower weight fluctuation."
        << endl;
        evtweight = 0.;
      }
    }   
    // Do not print zero-weight events.
    if ( evtweight == 0. ) continue;

    wmin     = min(wmin, evtweight);
    wmax     = max(wmax, evtweight);
    sumwt   += evtweight;
    sumwtsq += pow2(evtweight);
    wtcount += 1.;
    histWT -> Fill (evtweight);
    
    double normhepmc = xsecLO[iEvent] / nAcceptLO[iEvent];
    // Weighted events with additional number of trial events to consider.
    if ( pythia.info.lhaStrategy() != 0
      && pythia.info.lhaStrategy() != 3
      && nAcceptSH > 0)
      normhepmc = 1. / (1e9*nAcceptSH);
    // Weighted events.
    else if ( pythia.info.lhaStrategy() != 0
      && pythia.info.lhaStrategy() != 3
      && nAcceptSH == 0)
      normhepmc = 1. / (1e9*nAccept);

    
    //------------------------------------------------------------------------
    //KINEMATIC ANALYSIS------------------------------------------------------
    //------------------------------------------------------------------------    
    if(pythia.event.size() > 3){

      sigmaTotal  += evtweight * normhepmc;
      sigmaSample += evtweight * normhepmc;
      errorTotal  += pow2(evtweight * normhepmc);
      errorSample += pow2(evtweight * normhepmc);       
      
      int iNucleon = pythia.event[1].isHadron() ? 1 : 2;
      int iLepton  = iNucleon == 1 ? 2 : 1;             
      
      int iInLepton(0), iScatLepton(0);
      for ( int i=0; i < pythia.event.size(); ++i ) {
	if(pythia.event[i].statusAbs()==21 && pythia.event[i].isAncestor(iLepton))
	iInLepton = i;
	else if(pythia.event[i].statusAbs()==23 && pythia.event[i].id()==pythia.event[iLepton].id())
	iScatLepton = i;
      }

      Zprim = 0; Xprim = 0; Yprim = 0;
      if(pythia.event[iLepton].idAbs() == 13){
	Zprim = r1 -> Uniform(-350.,-100); 
	Xprim = r2 -> Gaus(0., 1.5); 
	Yprim = r3 -> Gaus(0., 1.5); 
      }

      // Construct q, Q2, W2, y, xbj.
      Vec4 p0Lept    ( pythia.event[iLepton]    .p() );
      Vec4 pInLept   ( pythia.event[iInLepton]  .p() );
      Vec4 pScatLept ( pythia.event[iScatLepton].p() );
      Vec4 pNucleon  ( pythia.event[iNucleon]   .p() );
      Vec4 qtr       ( pInLept - pScatLept );
      Vec4 q         ( p0Lept  - pScatLept );
      Vec4 hadSys    ( pNucleon + qtr );
      
      double W2tr    = hadSys.m2Calc();
      Q2tr    = -qtr.m2Calc();
      nutr    = qtr.e();
      Wtr     = pow( W2tr, 0.5);
      ytr     = (pNucleon * qtr) / (pNucleon * pInLept);
      xbjtr   = Q2tr / (2. * pNucleon * qtr);
      
      double W2    = ( pNucleon + q ).m2Calc();
      Q2      = -q.m2Calc();
      nu      = q.e();
      W       = pow( W2, 0.5);
      y       = (pNucleon * q) / (pNucleon * p0Lept);
      xbj     = Q2 / (2. * pNucleon * q);
      
      str   = pythia.info.sHat();
      ttr   = pythia.info.tHat();
      Evt   = iEvent;
      

      beam_p= pInLept.pAbs();
      beamth= acos(pInLept.pz()/pInLept.pAbs());
      beamph= atan2(pInLept.py(), pInLept.px()) ;
      
      aeam_p= pNucleon.pAbs();
      aeamth= acos(pNucleon.pz()/pNucleon.pAbs());
      aeamph= atan2(pNucleon.py(), pNucleon.px() ) ;
      
      outlep_p= pScatLept.pAbs();
      outlepth= acos( pScatLept.pz()/ pScatLept.pAbs()) ;
      outlepph= atan2(pScatLept.py(), pScatLept.px()) ;
      
      Gamma.SetPxPyPzE ( q.px(), q.py(), q.pz(), q.e());
      gathe = Gamma.Vect().Theta();
      gaphi = Gamma.Vect().Phi();
      gaene = Gamma.Vect().Mag();


      //Now using ROOT's TLorentzVector instead of PYTHIA's Vec4--------------
      //----------------------------------------------------------------------
      l_nuc_i.SetPxPyPzE( pNucleon.px(), pNucleon.py(), pNucleon.pz(),
                          pNucleon.e());
      l_lep_i.SetPxPyPzE(  pInLept.px(),  pInLept.py(),  pInLept.pz(), 
                           pInLept.e());
      l_lep_f.SetPxPyPzE(pScatLept.px(),pScatLept.py(),pScatLept.pz(),
                           pScatLept.e());
      p_cms.SetPxPyPzE  (  hadSys.px(),  hadSys.py(),  hadSys.pz(),  
                           hadSys.e());
      
      double thetaNom = acos( l_lep_i.Vect().Dot(l_lep_f.Vect()) );
      double thetaDen = (l_lep_i.P() * l_lep_f.P());
      theta = thetaNom / thetaDen;
      
      
      // --- Lab Frame gamma Angles------------------------------------------- 
      // --------------------------------------------------------------------- 
      xxl = l_lep_i.Vect().Unit();
      yyl = l_lep_i.Vect().Cross(l_lep_f.Vect());  yyl.Unit();
      zzl = xxl.Cross(yyl);
      
      gathx  = atan2(  Gamma.Vect().Dot(xxl),  Gamma.Vect().Dot(zzl)); 
      gathy  = atan2(  Gamma.Vect().Dot(yyl),  Gamma.Vect().Dot(zzl));
      
       
      // --- Gamma Nucleon Frame (GNS) --------------------------------------- 
      // ---------------------------------------------------------------------
      BoostGNS  = - p_cms.Vect() * (1./p_cms.E());
      bcm       = BoostGNS.Mag();
      gcm       = p_cms.E()/p_cms.Mag();
      
      //Boost 4Vecs in other frame, same unit vectors as Lab Frame
      GammaGNS   =  Gamma ; GammaGNS.Boost (BoostGNS);
      l_lep_iGNS =  l_lep_i; l_lep_iGNS.Boost(BoostGNS);
      l_lep_fGNS =  l_lep_f; l_lep_fGNS.Boost(BoostGNS);
      l_nuc_iGNS =  l_nuc_i; l_nuc_iGNS.Boost(BoostGNS);
      lSpinGNS   =  lSpin ; lSpinGNS.Boost (BoostGNS);

      zz = GammaGNS.Vect().Unit();
      yy = l_lep_iGNS.Vect().Cross(l_lep_fGNS.Vect());  yy=yy.Unit();
      xx = yy.Cross(zz);
     
      //Do things with the rotational matrix GNS     
      GNS.SetToIdentity();       //sets GNS equal to identity matrix I
      GNS.MakeBasis(iv,jv,kv);   //sets unit vectors as unit variables 
      GNS.RotateAxes(xx,yy,zz);  //adds a rotation of local axes     
      GNS.Invert();              //invert matrix

      //Transform 4Vecs in rotated GNS xx,yy,zz frame, I think
      GammaGNS.Transform (GNS);  
      l_nuc_iGNS.Transform(GNS);  
      l_lep_iGNS.Transform(GNS);  
      l_lep_fGNS.Transform(GNS);  
      lSpinGNS.Transform (GNS);
      
      xxGNS = xx; xxGNS.Transform(GNS);
      yyGNS = yy; yyGNS.Transform(GNS);
      zzGNS = zz; zzGNS.Transform(GNS);

      // cout.setf(ios::fixed);
      // cout << " lab - gamma"
      // 	   << setprecision ( 4) << setw(12) <<  Gamma.Px()
      // 	   << setprecision ( 4) << setw(12) <<  Gamma.Py()
      // 	   << setprecision ( 4) << setw(12) <<  Gamma.Pz()
      // 	   << " lab - mu_i "
      // 	   << setprecision ( 4) << setw(12) << l_lep_i.Px()
      // 	   << setprecision ( 4) << setw(12) << l_lep_i.Py()
      // 	   << setprecision ( 4) << setw(12) << l_lep_i.Pz()
      // 	   << " lab - mu_f "
      // 	   << setprecision ( 4) << setw(12) << l_lep_f.Px()
      // 	   << setprecision ( 4) << setw(12) << l_lep_f.Py()
      // 	   << setprecision ( 4) << setw(12) << l_lep_f.Pz()
      // 	   << " lab - pr_i "
      // 	   << setprecision ( 4) << setw(12) << l_nuc_i.Px()
      // 	   << setprecision ( 4) << setw(12) << l_nuc_i.Py()
      // 	   << setprecision ( 4) << setw(12) << l_nuc_i.Pz()
      // 	   << endl ;
      // cout << " GNS - gamma"
      // 	   << setprecision ( 4) << setw(12) << GammaGNS.Px()
      // 	   << setprecision ( 4) << setw(12) << GammaGNS.Py()
      // 	   << setprecision ( 4) << setw(12) << GammaGNS.Pz()
      // 	   << " GNS - mu_i "
      // 	   << setprecision ( 4) << setw(12) << l_lep_iGNS.Px()
      // 	   << setprecision ( 4) << setw(12) << l_lep_iGNS.Py()
      // 	   << setprecision ( 4) << setw(12) << l_lep_iGNS.Pz()
      // 	   << " GNS - mu_f "
      // 	   << setprecision ( 4) << setw(12) << l_lep_fGNS.Px()
      // 	   << setprecision ( 4) << setw(12) << l_lep_fGNS.Py()
      // 	   << setprecision ( 4) << setw(12) << l_lep_fGNS.Pz()
      // 	   << " GNS - pr_i "
      // 	   << setprecision ( 4) << setw(12) << l_nuc_iGNS.Px()
      // 	   << setprecision ( 4) << setw(12) << l_nuc_iGNS.Py()
      // 	   << setprecision ( 4) << setw(12) << l_nuc_iGNS.Pz()
      // 	   << endl << endl << endl;
      
      v_fot  = GammaGNS;
      i_lep  = l_lep_iGNS;
      f_lep  = l_lep_fGNS;
      SB     = lSpinGNS.Vect();
      // from muons:
      float phs_den = (zzGNS.Cross(i_lep.Vect()).Mag()*zzGNS.Cross(SB).Mag());
      sinphis_GNS = zzGNS.Cross(i_lep.Vect()).Dot(SB) / phs_den;
      cosphis_GNS = zzGNS.Cross(i_lep.Vect()).Dot(zzGNS.Cross(SB))/ phs_den; 
      phi_s = atan2(sinphis_GNS, cosphis_GNS); 
      
      
      //--- Hadrons' analysis-------------------------------------------------
      //----------------------------------------------------------------------
      for ( int i=0; i < pythia.event.size(); ++i ) {
	if(i  == iInLepton   ) continue; 
	if(i  == iNucleon       ) continue; 
	if(i  == iScatLepton ) continue; 
	if(    pythia.event[i].isNeutral()) continue;
	if( ! (pythia.event[i].isFinal() )) continue;
	if(HadNb >= Hmax) continue;
	Vec4 pHadron( pythia.event[i].p() );
        lvh.SetPxPyPzE(pHadron.px(),pHadron.py(),pHadron.pz(),pHadron.e());
	
	// ---- Rotations to GNS ( better GNS ) ------------------------------
	lvhGNS  = lvh;  lvhGNS.Boost(BoostGNS); lvhGNS.Transform(GNS);

	f_had = lvhGNS;
	HB    =  f_had.Vect();
	// from muons:	
	float ph_den= (zzGNS.Cross(i_lep.Vect()).Mag()*zzGNS.Cross(HB).Mag());
	cosphih_GNS = zzGNS.Cross(i_lep.Vect()).Dot(zzGNS.Cross(HB)) / ph_den;
	sinphih_GNS = (i_lep.Vect().Cross(HB)).Dot(zzGNS) / ph_den;
	//sinphih_GNS = zzGNS.Cross(i_lep.Vect()).Dot(HB) / ph_den;	

	ch   [HadNb] = pythia.event[i].id();
	eh   [HadNb] = lvh.E()   ;
	ph   [HadNb] = lvh.Rho() ;
	zh   [HadNb] = pNucleon*pHadron / (pNucleon * q);
	pth  [HadNb] = f_had.Pt(v_fot.Vect());
	//xfh  [HadNb] = PaAlgo::Xf(l_lep_iGNS, l_lep_fGNS, lvhGNS);
	etah [HadNb] = lvhGNS.Rapidity();
	phi_h[HadNb] = atan2(sinphih_GNS, cosphih_GNS);
	theha[HadNb] = lvh.Vect().Theta() ;
	phiha[HadNb] = lvh.Vect().Phi() ;

	HadNb++;
      //END KINEMATIC ANALYSIS------------------------------------------------
      //----------------------------------------------------------------------    	
      }      
      
      //For testing purposes 1
      //if (pythia.event.size() > 40) 
      //pythia.event.list(); 
      
      //For testing purposes 2   
      //if (pythia.event[iLepton].e() - pythia.event[iInLepton].e() > 100) {
      //cout <<endl<<iLepton<<endl<<iInLepton<<endl<<iScatLepton;
      //pythia.event.list(); }
    }
    tree -> Fill(); 
 
  } // end loop over events to generate

  // print cross section and errors
  pythia.stat();

  //Printing weights statistics
  cout << scientific << setprecision(6)
       << "\t Minimal shower weight     = " << wmin << "\n"
       << "\t Maximal shower weight     = " << wmax << "\n"
       << "\t Mean shower weight        = " << sumwt/wtcount << "\n"
       << "\t Variance of shower weight = "
       << sqrt(1/nAccept*(sumwtsq - pow(sumwt,2)/nAccept))
       << endl;

  // Print and write tree to file
  TFile *hfile = TFile::Open("main777tree.root","recreate");
  tree   -> Print();
  tree   -> Write(); 
  hfile  -> Close();
  
  // Draw and write histograms to file
  TApplication theApp("hist", &argc, argv);
  TFile *histfile = TFile::Open("main777hist.root", "RECREATE");
  TCanvas *c1 = new TCanvas ("c1");
  histWT -> SetAxisRange(wmin - abs(wmax) / 10, wmax + abs(wmin) / 10, "X");
  histWT -> Draw();
  gPad   -> SetGridy();
  gPad   -> SetLogy();
  gPad   -> WaitPrimitive();
  histWT -> Write();
  histfile -> Close();

  return 0;
}