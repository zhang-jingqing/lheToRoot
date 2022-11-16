#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

//covert lhe file to a root file
//https://arxiv.org/pdf/hep-ph/0609017.pdf, sect. 2, 3
// parameter name and meaing, e.g., IDBMUP, PUP, can read LHA paper, the ref[1]

void lheToRoot() {
  std::string inputLheFile = "a.lhe";
  fstream fin(inputLheFile.c_str(), std::ios::in);  

  std::size_t itail = inputLheFile.find(".lhe");
  std::string outRootFile = inputLheFile.substr(0, itail);
  outRootFile += ".root";

  TFile *fout = new TFile(outRootFile.c_str(), "RECREATE");

  /*
  Initialization information is stored in HEPRUP:
  INTEGER MAXPUP
  PARAMETER (MAXPUP=100)
  INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
  DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
  COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
  &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
  &XMAXUP(MAXPUP),LPRUP(MAXPUP)
  
  1) Initialization information, given once
  a) one line with process-number-independent information:
  IDBMUP(1) IDBMUP(2) EBMUP(1) EBMUP(2) PDFGUP(1) PDFGUP(2)  PDFSUP(1) PDFSUP(2) IDWTUP NPRUP
  b) NPRUP lines, one for each process IPR in the range 1 through NPRUP:
  XSECUP(IPR) XERRUP(IPR) XMAXUP(IPR) LPRUP(IPR)
  */
  TTree *treeInit = new TTree("treeInit", "Init information");
  const int MAXPUP=100;
  int IDWTUP, NPRUP;
  int IDBMUP[2], PDFGUP[2], PDFSUP[2];
  double EBMUP[2];
  int LPRUP[MAXPUP];
  double XSECUP[MAXPUP], XERRUP[MAXPUP], XMAXUP[MAXPUP];
  treeInit->Branch("IDBMUP", IDBMUP, "IDBMUP[2]/I");
  treeInit->Branch("EBMUP", EBMUP, "EBMUP[2]/D");
  treeInit->Branch("PDFGUP", PDFGUP, "PDFGUP[2]/I");
  treeInit->Branch("PDFSUP", PDFSUP, "PDFSUP[2]/I");
  treeInit->Branch("IDWTUP", &IDWTUP, "IDWTUP/I");
  treeInit->Branch("NPRUP", &NPRUP, "NPRUP/I");
  treeInit->Branch("XSECUP", XSECUP, "XSECUP[NPRUP]/D");
  treeInit->Branch("XERRUP", XERRUP, "XERRUP[NPRUP]/D");
  treeInit->Branch("XMAXUP", XMAXUP, "XMAXUP[NPRUP]/D");
  treeInit->Branch("LPRUP", LPRUP, "LPRUP[NPRUP]/I");

  
  TTree *treeEvent = new TTree("treeEvent", "Events tree");
  /*
  Information on each separate event is stored in HEPEUP:
  INTEGER MAXNUP
  PARAMETER (MAXNUP=500)
  INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
  DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,
  &SPINUP
  COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
  &IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
  &ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
  &SPINUP(MAXNUP)

  2) Event information, repeated as many times as there are events
  a) one line with common event information:
  NUP IDPRUP XWGTUP SCALUP AQEDUP AQCDUP
  b) NUP lines, one for each particle I in the range 1 through NUP
  IDUP(I) ISTUP(I) MOTHUP(1,I) MOTHUP(2,I) ICOLUP(1,I) ICOLUP(2,I) PUP(1,I) PUP(2,I) PUP(3,I) PUP(4,I) PUP(5,I)  VTIMUP(I) SPINUP(I)
  */
  const int MAXNUP = 500;
  int NUP, IDPRUP;
  double XWGTUP, SCALUP, AQEDUP, AQCDUP;
  int IDUP[MAXNUP], ISTUP[MAXNUP], MOTHUP[MAXNUP][2], ICOLUP[MAXNUP][2];
  double PUP[MAXNUP][5], VTIMUP[MAXNUP], SPINUP[MAXNUP];
  treeEvent->Branch("NUP", &NUP, "NUP/I"); //N particles
  treeEvent->Branch("IDPRUP", &IDPRUP, "IDPRUP/I");
  treeEvent->Branch("IDUP", IDUP, "IDUP[NUP]/I"); //pdg id of the particle
  treeEvent->Branch("ISTUP", ISTUP, "ISTUP[NUP]/I"); //status of the particle: -1 means incoming, 1 means final, 2 means intermediate
  treeEvent->Branch("MOTHUP", MOTHUP, "MOTHUP[NUP][2]/I"); //mother's index in this array
  treeEvent->Branch("ICOLUP", ICOLUP, "ICOLUP[NUP][2]/I");
  treeEvent->Branch("XWGTUP", &XWGTUP, "XWGTUP/D");
  treeEvent->Branch("SCALUP", &SCALUP, "SCALUP/D");
  treeEvent->Branch("AQEDUP", &AQEDUP, "AQEDUP/D");
  treeEvent->Branch("AQCDUP", &AQCDUP, "AQCDUP/D");
  treeEvent->Branch("PUP", PUP, "PUP[NUP][5]/D"); //px,py,pz,e, mass of the particle
  treeEvent->Branch("VTIMUP", VTIMUP, "VTIMUP[NUP]/D");
  treeEvent->Branch("SPINUP", SPINUP, "SPINUP[NUP]/D");
  
  //convert lhe file by lines
  std::string initBegin("<init>");
  std::string initEnd("</init>");
  std::string eventBegin("<event>");
  std::string eventEnd("</event>");
  std::string line;
  Long64_t icount = 0;
  while (std::getline(fin, line)) {
    if (icount % 10000 == 1) {
      std::cout << "converting " << icount << " event" << std::endl;
    }
    if (std::string::npos != line.find(initBegin)) {
      std::getline(fin, line);
      std::istringstream iss(line);
      iss >> IDBMUP[0] >> IDBMUP[1] >> EBMUP[0] >> EBMUP[1] >> PDFGUP[0] >> PDFGUP[1] >> PDFSUP[0] >> PDFSUP[1] >> IDWTUP >> NPRUP;
      for (int IPR = 0; IPR < NPRUP; ++IPR) {
        std::getline(fin, line);
        //if (std::string::npos != line.find("#")) {
        //  //skip comment line if have
        //  IPR--;
        //  continue;
        //}
        std::istringstream iss2(line);
        iss2 >> XSECUP[IPR] >> XERRUP[IPR] >> XMAXUP[IPR] >> LPRUP[IPR];
      }
      std::getline(fin, line); //read initEnd line
      treeInit->Fill();
    }
    if (std::string::npos != line.find(eventBegin)) {
      std::getline(fin, line);
      std::istringstream iss(line);
      iss >> NUP >> IDPRUP >> XWGTUP >> SCALUP >> AQEDUP >> AQCDUP;
      int MotherIndex0, MotherIndex1;
      for (int I = 0; I < NUP; ++I) {
        std::getline(fin, line);
        //if (std::string::npos != line.find("#")) {
        //  //skip comment line if have
        //  NUP--;
        //  continue;
        //}
        std::istringstream iss2(line);
        iss2 >> IDUP[I] >> ISTUP[I] >> MotherIndex0 >> MotherIndex1 >> ICOLUP[I][0] >> ICOLUP[I][1] >> PUP[I][0] >> PUP[I][1] >> PUP[I][2] >> PUP[I][3] >> PUP[I][4] >> VTIMUP[I] >> SPINUP[I];
        //array starts from 0, so convert mother index to array index
        MOTHUP[I][0] = MotherIndex0 - 1;
        MOTHUP[I][1] = MotherIndex1 - 1;
      }
      std::getline(fin, line); //read eventEnd line
      treeEvent->Fill();
      ++icount;
    }
  }
  fin.close();

  treeInit->Write();
  treeEvent->Write();
  fout->Close();
  
  std::cout << "Convert " << icount << " events to " << outRootFile << " in total" << std::endl;

}
