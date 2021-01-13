//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jan 12 16:49:51 2021 by ROOT version 6.22/00
// from TTree tDigit/Digitization
// found on file: /home/mcgrew/data/dune/210112/test_ecalHitSim.root
//////////////////////////////////////////////////////////

#ifndef ERepSimECalDigit_h
#define ERepSimECalDigit_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
#include <string>

// Header file for the classes stored in the TTree if any.

class ERepSimECalDigit {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxcell = 209;
   static constexpr Int_t kMaxStt = 1;

   // Declaration of leaf types
   Int_t           cell_;
   Int_t           cell_id[kMaxcell];   //[cell_]
   Double_t        cell_z[kMaxcell];   //[cell_]
   Double_t        cell_y[kMaxcell];   //[cell_]
   Double_t        cell_x[kMaxcell];   //[cell_]
   Double_t        cell_l[kMaxcell];   //[cell_]
   Double_t        cell_adc1[kMaxcell];   //[cell_]
   Double_t        cell_tdc1[kMaxcell];   //[cell_]
   Double_t        cell_adc2[kMaxcell];   //[cell_]
   Double_t        cell_tdc2[kMaxcell];   //[cell_]
   Int_t           cell_mod[kMaxcell];   //[cell_]
   Int_t           cell_lay[kMaxcell];   //[cell_]
   Int_t           cell_cel[kMaxcell];   //[cell_]
   std::vector<double>  cell_pe_time1[kMaxcell];
   std::vector<int>     cell_hindex1[kMaxcell];
   std::vector<double>  cell_pe_time2[kMaxcell];
   std::vector<int>     cell_hindex2[kMaxcell];
   Int_t           Stt_;
   std::string     Stt_det[kMaxStt];
   Double_t        Stt_x[kMaxStt];   //[Stt_]
   Double_t        Stt_y[kMaxStt];   //[Stt_]
   Double_t        Stt_z[kMaxStt];   //[Stt_]
   Double_t        Stt_t[kMaxStt];   //[Stt_]
   Double_t        Stt_de[kMaxStt];   //[Stt_]
   Bool_t          Stt_hor[kMaxStt];   //[Stt_]
   std::vector<int>     Stt_hindex[kMaxStt];

   // List of branches
   TBranch        *b_cell_;   //!
   TBranch        *b_cell_id;   //!
   TBranch        *b_cell_z;   //!
   TBranch        *b_cell_y;   //!
   TBranch        *b_cell_x;   //!
   TBranch        *b_cell_l;   //!
   TBranch        *b_cell_adc1;   //!
   TBranch        *b_cell_tdc1;   //!
   TBranch        *b_cell_adc2;   //!
   TBranch        *b_cell_tdc2;   //!
   TBranch        *b_cell_mod;   //!
   TBranch        *b_cell_lay;   //!
   TBranch        *b_cell_cel;   //!
   TBranch        *b_cell_pe_time1;   //!
   TBranch        *b_cell_hindex1;   //!
   TBranch        *b_cell_pe_time2;   //!
   TBranch        *b_cell_hindex2;   //!
   TBranch        *b_Stt_;   //!
   TBranch        *b_Stt_det;   //!
   TBranch        *b_Stt_x;   //!
   TBranch        *b_Stt_y;   //!
   TBranch        *b_Stt_z;   //!
   TBranch        *b_Stt_t;   //!
   TBranch        *b_Stt_de;   //!
   TBranch        *b_Stt_hor;   //!
   TBranch        *b_Stt_hindex;   //!

   ERepSimECalDigit(TTree *tree=0);
   virtual ~ERepSimECalDigit();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ERepSimECalDigit_cxx
ERepSimECalDigit::ERepSimECalDigit(TTree *tree) : fChain(0)
{
   Init(tree);
}

ERepSimECalDigit::~ERepSimECalDigit()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ERepSimECalDigit::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ERepSimECalDigit::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ERepSimECalDigit::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("cell", &cell_, &b_cell_);
   fChain->SetBranchAddress("cell.id", cell_id, &b_cell_id);
   fChain->SetBranchAddress("cell.z", cell_z, &b_cell_z);
   fChain->SetBranchAddress("cell.y", cell_y, &b_cell_y);
   fChain->SetBranchAddress("cell.x", cell_x, &b_cell_x);
   fChain->SetBranchAddress("cell.l", cell_l, &b_cell_l);
   fChain->SetBranchAddress("cell.adc1", cell_adc1, &b_cell_adc1);
   fChain->SetBranchAddress("cell.tdc1", cell_tdc1, &b_cell_tdc1);
   fChain->SetBranchAddress("cell.adc2", cell_adc2, &b_cell_adc2);
   fChain->SetBranchAddress("cell.tdc2", cell_tdc2, &b_cell_tdc2);
   fChain->SetBranchAddress("cell.mod", cell_mod, &b_cell_mod);
   fChain->SetBranchAddress("cell.lay", cell_lay, &b_cell_lay);
   fChain->SetBranchAddress("cell.cel", cell_cel, &b_cell_cel);
   fChain->SetBranchAddress("cell.pe_time1", cell_pe_time1, &b_cell_pe_time1);
   fChain->SetBranchAddress("cell.hindex1", cell_hindex1, &b_cell_hindex1);
   fChain->SetBranchAddress("cell.pe_time2", cell_pe_time2, &b_cell_pe_time2);
   fChain->SetBranchAddress("cell.hindex2", cell_hindex2, &b_cell_hindex2);
   fChain->SetBranchAddress("Stt", &Stt_, &b_Stt_);
   fChain->SetBranchAddress("Stt.det", &Stt_det, &b_Stt_det);
   fChain->SetBranchAddress("Stt.x", &Stt_x, &b_Stt_x);
   fChain->SetBranchAddress("Stt.y", &Stt_y, &b_Stt_y);
   fChain->SetBranchAddress("Stt.z", &Stt_z, &b_Stt_z);
   fChain->SetBranchAddress("Stt.t", &Stt_t, &b_Stt_t);
   fChain->SetBranchAddress("Stt.de", &Stt_de, &b_Stt_de);
   fChain->SetBranchAddress("Stt.hor", &Stt_hor, &b_Stt_hor);
   fChain->SetBranchAddress("Stt.hindex", &Stt_hindex, &b_Stt_hindex);
   Notify();
}

Bool_t ERepSimECalDigit::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ERepSimECalDigit::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ERepSimECalDigit::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ERepSimECalDigit_cxx
