/*************************************************************
 * 
 * SimpleTrackAna class
 * 
 * This is a simple class that we can use for writing out
 * a TTree with interesting information from Track obejcts.
 *
 * Wesley Ketchum (wketchum@fnal.gov), Oct31, 2017
 * 
 *************************************************************/

//some standard C++ includes
#include <vector>

//some ROOT includes
#include "TTree.h"
#include "TH1F.h"

//"larsoft" object includes
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"

//let's make a useful struct for our output tree!
const int MAXHITS = 10000;

namespace ana{
  class SimpleTrackAna;
  struct TrackTreeObj;
}

struct ana::TrackTreeObj{
  unsigned int idx;

  float startx;
  float starty;
  float startz;
  float endx;
  float endy;
  float endz;
  float length;

  int n_hits;  

  float hit_time[MAXHITS];
  float hit_amp[MAXHITS];
  float hit_integral[MAXHITS];
  unsigned int hit_wire[MAXHITS];
  unsigned int hit_plane[MAXHITS];
  unsigned int hit_tpc[MAXHITS];

  void Clear();
  
  TrackTreeObj() { Clear(); }
};

class ana::SimpleTrackAna {

public:
    
  SimpleTrackAna(){}

  //This will be our function call inside the module
  void ProcessTracks(std::vector<recob::Track> const&,
		     std::vector< std::vector<recob::Hit const*> > const& );

  void SetOutputTree(TTree*);
  
private:

  TTree* fTree; //our analysis tree
  TrackTreeObj fTrkVals;
  
};
