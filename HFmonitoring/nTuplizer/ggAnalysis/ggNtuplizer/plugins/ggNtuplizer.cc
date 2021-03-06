#include "ggAnalysis/ggNtuplizer/interface/ggNtuplizer.h"

using namespace std;
using namespace edm;

void setbit(UShort_t& x, UShort_t bit) {
  UShort_t a = 1;
  x |= (a << bit);
}

ggNtuplizer::ggNtuplizer(const edm::ParameterSet& ps) {

  vtxLabel_                  = consumes<reco::VertexCollection>        (ps.getParameter<InputTag>("VtxLabel"));
  rhoLabel_                  = consumes<double>                        (ps.getParameter<InputTag>("rhoLabel"));
  rhoCentralLabel_           = consumes<double>                        (ps.getParameter<InputTag>("rhoCentralLabel"));
  electronCollection_        = consumes<View<pat::Electron> >          (ps.getParameter<InputTag>("electronSrc"));
  calibelectronCollection_   = consumes<View<pat::Electron> >          (ps.getParameter<InputTag>("calibelectronSrc"));
    
  ebReducedRecHitCollection_ = consumes<EcalRecHitCollection>          (ps.getParameter<InputTag>("ebReducedRecHitCollection"));
  eeReducedRecHitCollection_ = consumes<EcalRecHitCollection>          (ps.getParameter<InputTag>("eeReducedRecHitCollection"));
  esReducedRecHitCollection_ = consumes<EcalRecHitCollection>          (ps.getParameter<InputTag>("esReducedRecHitCollection")); 

  pckPFCandidateCollection_  = consumes<pat::PackedCandidateCollection>(ps.getParameter<InputTag>("packedPFCands"));

  jetsAK4Label_              = consumes<View<pat::Jet> >               (ps.getParameter<InputTag>("ak4JetSrc"));

  Service<TFileService> fs;
  tree_    = fs->make<TTree>("EventTree", "Event data (tag V08_00_26_03)");
  hEvents_ = fs->make<TH1F>("hEvents",    "total processed and skimmed events",   2,  0,   2);

  branchesGlobalEvent(tree_);
  branchesElectrons(tree_);
  branchesHFElectrons(tree_);
}

ggNtuplizer::~ggNtuplizer() {

}

void ggNtuplizer::analyze(const edm::Event& e, const edm::EventSetup& es) {

  hEvents_->Fill(0.5);

  edm::Handle<reco::VertexCollection> vtxHandle;
  e.getByToken(vtxLabel_, vtxHandle);

  // best-known primary vertex coordinates
  math::XYZPoint pv(0, 0, 0);
  for (vector<reco::Vertex>::const_iterator v = vtxHandle->begin(); v != vtxHandle->end(); ++v) {
      pv.SetXYZ(v->x(), v->y(), v->z());
      break;
  }
    
  fillGlobalEvent(e, es);
  fillElectrons(e, es, pv);
  fillHFElectrons(e);

  hEvents_->Fill(1.5);
  tree_->Fill();

}


DEFINE_FWK_MODULE(ggNtuplizer);
