/*************************************************************
 * 
 * SimpleTrackAna class
 * 
 * This is a simple class that we can use for calculating variables
 * from a Track object
 *
 * Wesley Ketchum (wketchum@fnal.gov), Oct31, 2017
 * 
 *************************************************************/


#include "SimpleTrackAna.h"

//function to clear out our input vector;
void ana::TrackTreeObj::Clear(){

  idx=999999;
  startx=-999999; starty=-999999; startz=-999999;
  endx=-999999; endy=-999999; endz=-999999;
  length = -99999;
  n_hits = -1;
  
  for(int i=0; i<MAXHITS; ++i)
    {
      hit_time[i]=-99999999; hit_amp[i] = -9999; hit_integral[i] = -9999;
      hit_wire[i]=999999; hit_plane[i]=999999; hit_tpc[i]=999999;
    }
}

//do out branch setup here!
void ana::SimpleTrackAna::SetOutputTree(TTree* tree)
{
  fTree = tree;
  fTree->SetName("trkanatree");
  fTree->SetTitle("TrackAnaTree");
  fTree->Branch("trk",&fTrkVals,"idx/i:startx/F:starty/F:startz/F:endx/F:endy/F:endz/F:length/F:n_hits/I");
  fTree->Branch("hit_time",&fTrkVals.hit_time,"hit_time[n_hits]/F");
  fTree->Branch("hit_amp",&fTrkVals.hit_amp,"hit_amp[n_hits]/F");
  fTree->Branch("hit_integral",&fTrkVals.hit_integral,"hit_integral[n_hits]/F");
  fTree->Branch("hit_wire",&fTrkVals.hit_wire,"hit_wire[n_hits]/i");
  fTree->Branch("hit_plane",&fTrkVals.hit_plane,"hit_plane[n_hits]/i");
  fTree->Branch("hit_tpc",&fTrkVals.hit_tpc,"hit_tpc[n_hits]/i");
}

//ok, the function that will process the tracks and fill the tree
void ana::SimpleTrackAna::ProcessTracks(std::vector<recob::Track> const& track_vec,
					std::vector< std::vector<recob::Hit const*> > const& assn_hits_vec)
{
  //error-check first: track_vec same size as hits_vec?
  if(track_vec.size()!=assn_hits_vec.size()){
    std::cerr << "ERROR! TRACKS VEC SIZE " << track_vec.size()
	      << " != ASSN HITS VEC SIZE " << assn_hits_vec.size()
	      << "\n\tWILL SKIP THIS EVENT!" << std::endl;
    return;
  }

  //clear the track values for the tree
  fTrkVals.Clear();

  //loop over the tracks
  for (size_t i_tr = 0; i_tr != track_vec.size(); ++i_tr) {

    //grab the track from the collection
    recob::Track const& trk = track_vec[i_tr]; 

    //fill track values
    fTrkVals.idx = i_tr;
    fTrkVals.startx = trk.Start().X();
    fTrkVals.starty = trk.Start().Y();
    fTrkVals.startz = trk.Start().Z();
    fTrkVals.endx = trk.End().X();
    fTrkVals.endy = trk.End().Y();
    fTrkVals.endz = trk.End().Z();
    fTrkVals.length = trk.Length();

    //now grab the associated hits
    std::vector<recob::Hit const*> const& hits_vec(assn_hits_vec[i_tr]);
    
    fTrkVals.n_hits = hits_vec.size();

    //now loop over the hits
    for(size_t i_h=0, size_hits = hits_vec.size(); i_h!=size_hits; ++i_h){
      fTrkVals.hit_time[i_h] = hits_vec[i_h]->PeakTime();
      fTrkVals.hit_amp[i_h]   = hits_vec[i_h]->PeakAmplitude();
      fTrkVals.hit_integral[i_h] = hits_vec[i_h]->Integral();
      fTrkVals.hit_wire[i_h] = hits_vec[i_h]->WireID().Wire;
      fTrkVals.hit_plane[i_h] = hits_vec[i_h]->WireID().Plane;
      fTrkVals.hit_tpc[i_h] = hits_vec[i_h]->WireID().TPC;   
    }//end loop over hits

    //now fill the tree before exiting track loop
    fTree->Fill();
    
  }//end loop over tracks

}//end ProcessTracks

