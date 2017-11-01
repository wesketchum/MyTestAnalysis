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

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "canvas/Persistency/Common/FindManyP.h"

//add the simple track ana alg!
#include "SimpleTrackAna.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindOne.h"

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

  typedef std::vector< art::Ptr<recob::Hit>> HitVector;
  typedef std::map< art::Ptr<recob::Cluster>, HitVector> ClustersToHits;

  // Declare member data here.
  TFile * m_file;
  TTree * m_tree;

  unsigned int nEvent;
  unsigned int nPFParticles;
  unsigned int nTracks;
  unsigned int nShowers;
  int largestHits;
  int largestDaughters;

  std::string     m_particleLabel; 
  std::string     m_hitfinderLabel;

  //adding the stuff for the simple track ana!!
  ana::SimpleTrackAna fAnaAlg; //the alg
  art::InputTag fTrackInputTag;

  
};


test::MyTestAnalyzer::MyTestAnalyzer(fhicl::ParameterSet const & p) :
  EDAnalyzer(p)  // ,
 // More initializers here.
{
  //initialize
  nEvent=0;

  //create output tree                                                                                                             
  art::ServiceHandle<art::TFileService> tfs;                                                                                       
  m_file = new TFile("MyTestAnalyzer.root", "RECREATE");                                                                 
  m_tree = tfs->make<TTree>("tree","Tree");

  //add branches
  m_tree->Branch("nEvent",&nEvent,"nEvent/I");
  m_tree->Branch("nPFParticles",&nPFParticles,"nPFParticles/I");
  m_tree->Branch("nTracks", &nTracks, "nTracks/I");
  m_tree->Branch("nShowers", &nShowers, "nShowers/I");
  m_tree->Branch("nShowers", &nShowers, "nShowers/I");
  m_tree->Branch("largestHits", &largestHits, "largestHits/I");
  m_tree->Branch("largestDaughters", &largestDaughters, "largestDaughters/I");

  this->reconfigure(p);

  //make a new/different tree for the trackanaalg too
  fAnaAlg.SetOutputTree(tfs->make<TTree>("trkanatree","MyTrackAnaTree"));

}

test::MyTestAnalyzer::~MyTestAnalyzer()
{                                                                                                              
  //store output tree                                                                                          
  m_file->cd();                                                                                                
  m_tree->Write("tree");                                                                                       
  m_file->Close();                                                                                             
  std::cout << "End!" << std::endl;                                                                            
}

void test::MyTestAnalyzer::analyze(art::Event const & e)
{
  // Implementation of required member function here.
  ++nEvent;

  //your analysis goes here! 
  art::Handle< std::vector<recob::PFParticle> > theParticles;
  e.getByLabel(m_particleLabel, theParticles);
  nPFParticles = theParticles->size();

  nTracks  = 0;
  nShowers = 0;
  for (unsigned int i = 0; i < theParticles->size(); ++i)
  {
    const art::Ptr<recob::PFParticle> particle(theParticles, i);
    const int pdg(particle->PdgCode());
    //NOTE: Pandora assigns a pdg code 13 to any track-like object, 
    //and 11 to any shower-like object 
    if (pdg == 13)
    {
      ++nTracks;
    }
    else if (pdg == 11)
    {
      ++nShowers;
    } 
    else
      std::cout << " Found PFParticle with PDG code: " << pdg << std::endl;
  }

  //Find the PFParticle with largest number of hits associated 

  //we need to go through one of the intermediate layers of associations, e.g. clusters:
  art::Handle< std::vector<recob::Hit> > hitListHandle;
  e.getByLabel(m_hitfinderLabel,hitListHandle);
  art::FindManyP<recob::Cluster> theHitAssns(hitListHandle, e, m_particleLabel);

  ClustersToHits clustersToHits; 
  for (unsigned int i = 0; i < hitListHandle->size(); i++)
  {
    if (!theHitAssns.at(i).empty())
      {
	const art::Ptr<recob::Cluster> cluster(theHitAssns.at(i)[0]);
	const art::Ptr<recob::Hit> hit(hitListHandle,i);
	clustersToHits[cluster].push_back(hit);
      }
  }

  //get pfparticles->clusters associations
  largestHits = -std::numeric_limits<std::size_t>::max(); 
  art::FindManyP<recob::Cluster> theClusterAssns(theParticles, e, m_particleLabel);

  for (unsigned int i = 0; i < theParticles->size(); ++i)
  {
    const art::Ptr<recob::PFParticle> particle(theParticles, i);
    const std::vector< art::Ptr<recob::Cluster> > clusters = theClusterAssns.at(i);

    int nHits(0);
    for (unsigned int j=0; j<clusters.size(); ++j)
    {
      const art::Ptr<recob::Cluster> cluster = clusters.at(j);
      ClustersToHits::const_iterator iter = clustersToHits.find(cluster);
      const HitVector &hitVector = iter->second;
      nHits += hitVector.size();
    }
    if (nHits > largestHits)
    {
      largestHits = nHits;
      const std::vector<size_t> &daughterIDs = particle->Daughters();
      largestDaughters = daughterIDs.size();
    }
  }

  //fill the tree
  m_tree->Fill();  


  //ok, and at the end here now, let's add in what we did in the
  //demo_SimpleTrackAna code. Annoying changes of "ev"-->"e" and the like
  
  //let's get a valid handle, and a vector of objects from it
  auto const& track_handle = e.getValidHandle<std::vector<recob::Track>>(fTrackInputTag);
  auto const& track_vec(*track_handle);
  
  //note, we need to get the hit associations before running the alg.
  
  //prepare the container
  std::vector< std::vector<recob::Hit const*> > hits_vecs(track_vec.size());
  
  //run the FindMany stuff
  art::FindMany<recob::Hit> hits_per_track(track_handle,e,fTrackInputTag);
  for (size_t i_f = 0, size_track = track_vec.size(); i_f != size_track; ++i_f)
    hits_per_track.get(i_f,hits_vecs[i_f]);
  
  //fill our trees in our ana alg!
  fAnaAlg.ProcessTracks(track_vec,hits_vecs);    
  
  
}

void test::MyTestAnalyzer::reconfigure(fhicl::ParameterSet const & p)
{
  // Implementation of optional member function here.
  //here you add an external fcl file to change configuration
  m_particleLabel  = p.get<std::string>("PFParticleModule","pandoraNu");
  m_hitfinderLabel = p.get<std::string>("HitFinderModule","linecluster");

  fTrackInputTag = p.get<art::InputTag>("TrackModule","pandoraNu");
}

DEFINE_ART_MODULE(test::MyTestAnalyzer)
