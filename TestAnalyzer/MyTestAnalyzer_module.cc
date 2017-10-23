////////////////////////////////////////////////////////////////////////
// Class:       MyTestAnalyzer
// Plugin Type: analyzer (art v2_08_03)
// File:        MyTestAnalyzer_module.cc
//
// Generated at Fri Oct 20 15:42:43 2017 by Lorena Escudero Sanchez using cetskelgen
// from cetlib version v3_01_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//to save our tree
#include "TTree.h"                                       
#include "TFile.h"                                                                                                             
#include "art/Framework/Services/Optional/TFileService.h"

namespace test {
  class MyTestAnalyzer;
}


class test::MyTestAnalyzer : public art::EDAnalyzer {
public:
  explicit MyTestAnalyzer(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.
  virtual ~MyTestAnalyzer();

  // Plugins should not be copied or assigned.
  MyTestAnalyzer(MyTestAnalyzer const &) = delete;
  MyTestAnalyzer(MyTestAnalyzer &&) = delete;
  MyTestAnalyzer & operator = (MyTestAnalyzer const &) = delete;
  MyTestAnalyzer & operator = (MyTestAnalyzer &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

  // Selected optional functions.
  void reconfigure(fhicl::ParameterSet const & p) override;

private:

  // Declare member data here.
  TFile * myTFile;
  TTree * myTTree;

};


test::MyTestAnalyzer::MyTestAnalyzer(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{
  //create output tree                                                                                                             
  art::ServiceHandle<art::TFileService> tfs;                                                                                       
  myTFile = new TFile("MyTestAnalyzer.root", "RECREATE");                                                                 
  myTTree = tfs->make<TTree>("tree","Tree");
  //add branches
  

  this->reconfigure(p);

}

test::MyTestAnalyzer::~MyTestAnalyzer()
{                                                                                                                                 
  //store output tree                                                                                                         
  myTFile->cd();                                                                                                               
  myTTree->Write("tree");                                                                                            
  myTFile->Close();                                                                                                        
  std::cout << "End!" << std::endl;                                                                                     
}

void test::MyTestAnalyzer::analyze(art::Event const & e)
{
  // Implementation of required member function here.

  //your analysis goes here! 
}

void test::MyTestAnalyzer::reconfigure(fhicl::ParameterSet const & p)
{
  // Implementation of optional member function here.
  //here you add an external fcl file to change configuration
}

DEFINE_ART_MODULE(test::MyTestAnalyzer)
