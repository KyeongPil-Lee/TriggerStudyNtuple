
#include "TriggerStudyNtuple/ntupler/interface/ntupler.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonTrackLinks.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateIsolation.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Scalers/interface/LumiScalers.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"


#include <map>
#include <string>
#include <iomanip>
#include "TTree.h"

using namespace std;
using namespace reco;
using namespace edm;

ntupler::ntupler(const edm::ParameterSet& iConfig):
Token_OfflineMuon           ( consumes< std::vector<reco::Muon> >         (iConfig.getUntrackedParameter<edm::InputTag>("OfflineMuon")) ),
Token_OfflineVertex         ( consumes< reco::VertexCollection >        (iConfig.getUntrackedParameter<edm::InputTag>("OfflineVertex")) ),
Token_TriggerResults        ( consumes<edm::TriggerResults>           (iConfig.getUntrackedParameter<edm::InputTag>("TriggerResults")) ),
Token_TriggerEvent          ( consumes<trigger::TriggerEvent>         (iConfig.getUntrackedParameter<edm::InputTag>("TriggerEvent")) ),
Token_MyTriggerResults      ( consumes<edm::TriggerResults>           (iConfig.getUntrackedParameter<edm::InputTag>("MyTriggerResults")) ),
Token_MyTriggerEvent        ( consumes<trigger::TriggerEvent>         (iConfig.getUntrackedParameter<edm::InputTag>("MyTriggerEvent")) ),
Token_L3Muon                ( consumes<reco::RecoChargedCandidateCollection>  (iConfig.getUntrackedParameter<edm::InputTag>("L3Muon")) ),
Token_L2Muon                ( consumes<reco::RecoChargedCandidateCollection>  (iConfig.getUntrackedParameter<edm::InputTag>("L2Muon")) ),
Token_L1Muon                ( consumes<l1t::MuonBxCollection>           (iConfig.getUntrackedParameter<edm::InputTag>("L1Muon")) ),
Token_TkMuon                ( consumes<reco::RecoChargedCandidateCollection>  (iConfig.getUntrackedParameter<edm::InputTag>("TkMuon")) ),

Token_IterL3OI              ( consumes< std::vector<reco::MuonTrackLinks> >  (iConfig.getUntrackedParameter<edm::InputTag>("IterL3OI")) ),
Token_IterL3IO_L2Seeded     ( consumes< std::vector<reco::MuonTrackLinks> >  (iConfig.getUntrackedParameter<edm::InputTag>("IterL3IO_L2Seeded")) ),
Token_IterL3IO_FromL1       ( consumes< std::vector<reco::Track> >           (iConfig.getUntrackedParameter<edm::InputTag>("IterL3IO_FromL1")) ),
Token_IterL3_FromL2         ( consumes< std::vector<reco::MuonTrackLinks> >  (iConfig.getUntrackedParameter<edm::InputTag>("IterL3_FromL2")) ),

Token_ChargedIsoDep         ( consumes<reco::IsoDepositMap>           (iConfig.getUntrackedParameter<edm::InputTag>("ChargedIsoDep")) ),
Token_NeutralIsoDep         ( consumes<reco::RecoChargedCandidateIsolationMap>  (iConfig.getUntrackedParameter<edm::InputTag>("NeutralIsoDep")) ),
Token_PhotonIsoDep          ( consumes<reco::RecoChargedCandidateIsolationMap>  (iConfig.getUntrackedParameter<edm::InputTag>("PhotonIsoDep")) ),
Token_Rho                   ( consumes<double>                  (iConfig.getUntrackedParameter<edm::InputTag>("Rho")) ),
Token_OfflineRho            ( consumes<double>                  (iConfig.getUntrackedParameter<edm::InputTag>("OfflineRho")) ),
Token_RhoECAL               ( consumes<double>                  (iConfig.getUntrackedParameter<edm::InputTag>("RhoECAL")) ),
Token_RhoHCAL               ( consumes<double>                  (iConfig.getUntrackedParameter<edm::InputTag>("RhoHCAL")) ),
Token_OfflineECALPFIso03    ( consumes<edm::ValueMap<float>>          (iConfig.getUntrackedParameter<edm::InputTag>("OfflineECALPFIso03")) ),
Token_OfflineHCALPFIso03    ( consumes<edm::ValueMap<float>>          (iConfig.getUntrackedParameter<edm::InputTag>("OfflineHCALPFIso03")) ),
Token_OfflineECALPFIso04    ( consumes<edm::ValueMap<float>>          (iConfig.getUntrackedParameter<edm::InputTag>("OfflineECALPFIso04")) ),
Token_OfflineHCALPFIso04    ( consumes<edm::ValueMap<float>>          (iConfig.getUntrackedParameter<edm::InputTag>("OfflineHCALPFIso04")) ),
Token_LumiScaler            ( consumes<LumiScalersCollection>           (iConfig.getUntrackedParameter<edm::InputTag>("LumiScaler")) ),
Token_OfflineLumiScaler     ( consumes<LumiScalersCollection>           (iConfig.getUntrackedParameter<edm::InputTag>("OfflineLumiScaler")) ),
Token_PUSummaryInfo         ( consumes< std::vector<PileupSummaryInfo> >    (iConfig.getUntrackedParameter<edm::InputTag>("PUSummaryInfo")) ),
Token_GenEventInfo          ( consumes< GenEventInfoProduct >         (iConfig.getUntrackedParameter<edm::InputTag>("GenEventInfo")) ),
Token_GenParticle           ( consumes<reco::GenParticleCollection>       (iConfig.getUntrackedParameter<edm::InputTag>("GenParticle")) )
{

}

void ntupler::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
  this->Init();
  this->IsRealData = iEvent.isRealData();
  this->RunNum = iEvent.id().run();
  this->LumiBlockNum = iEvent.id().luminosityBlock();
  this->EventNum = iEvent.id().event();

  // -- vertex -- //
  edm::Handle<reco::VertexCollection> Handle_Vertex;
  if( iEvent.getByToken(Token_OfflineVertex, Handle_Vertex) )
  {
    int nGoodVtx = 0;
    for(reco::VertexCollection::const_iterator it = Handle_Vertex->begin(); it != Handle_Vertex->end(); ++it)
    {
      if( it->isValid() ) nGoodVtx++;
    }
    this->nVertices = nGoodVtx;
  }
  // else
  //  std::cout << "Handle_Vertex is not valid ... nVertices will not be filled" << std::endl;

  // -- Rho (offline) -- //
  edm::Handle<double>  Handle_OfflineRho;
  iEvent.getByToken(Token_OfflineRho, Handle_OfflineRho);
  if( Handle_OfflineRho.isValid() ) this->Rho_Offline = *(Handle_OfflineRho.product());

  if( iEvent.isRealData() )
  {
    this->BX_ID = iEvent.bunchCrossing();

    edm::Handle<LumiScalersCollection> Handle_LumiScaler;
    iEvent.getByToken(Token_LumiScaler, Handle_LumiScaler);
    if (Handle_LumiScaler->begin() != Handle_LumiScaler->end())
    {
      this->InstLumi = Handle_LumiScaler->begin()->instantLumi();
      this->DataPU = Handle_LumiScaler->begin()->pileup();
      this->DataPURMS = Handle_LumiScaler->begin()->pileupRMS();
      this->BunchLumi = Handle_LumiScaler->begin()->bunchLumi();
    }

    edm::Handle<LumiScalersCollection> Handle_OfflineLumiScaler;
    if( iEvent.getByToken(Token_OfflineLumiScaler, Handle_OfflineLumiScaler) )
    {
      this->OfflineInstLumi = Handle_OfflineLumiScaler->begin()->instantLumi();
      this->OfflineDataPU = Handle_OfflineLumiScaler->begin()->pileup();
      this->OfflineDataPURMS = Handle_OfflineLumiScaler->begin()->pileupRMS();
      this->OfflineBunchLumi = Handle_OfflineLumiScaler->begin()->bunchLumi();
    }
  }

  // -- True PU info: only for MC -- //
  if( !iEvent.isRealData() )
  {
    edm::Handle<std::vector< PileupSummaryInfo > > Handle_PUSummaryInfo;

    if( iEvent.getByToken(Token_PUSummaryInfo,Handle_PUSummaryInfo) )
    {
      std::vector<PileupSummaryInfo>::const_iterator PVI;
      for(PVI = Handle_PUSummaryInfo->begin(); PVI != Handle_PUSummaryInfo->end(); ++PVI)
      {
        if(PVI->getBunchCrossing()==0)
        {
          this->TruePU = PVI->getTrueNumInteractions();
          continue;
        }
      } // -- end of PU iteration -- //
    } // -- end of if ( token exists )
  } // -- end of isMC -- //

  this->Fill_Muon(iEvent);
  this->Fill_HLT(iEvent);
  this->Fill_MYHLT(iEvent);
  this->Fill_HLTMuon(iEvent);
  this->Fill_L1Muon(iEvent);

  this->Fill_IterL3(iEvent);

  if( !iEvent.isRealData() ) this->Fill_GenParticle(iEvent);

  this->ntuple->Fill();
}

void ntupler::beginJob()
{
  edm::Service<TFileService> fs;
  this->ntuple = fs->make<TTree>("ntuple","ntuple");

  this->Make_Branch();
}

void ntupler::Init()
{
  this->IsRealData = false;
  this->RunNum = -999;
  this->LumiBlockNum = -999;
  this->EventNum = 0;
  this->nVertices = -999;
  this->Rho_Offline = -999;
  this->BX_ID = -999;
  this->InstLumi = -999;
  this->DataPU = -999;
  this->DataPURMS = -999;
  this->BunchLumi = -999;
  this->OfflineInstLumi = -999;
  this->OfflineDataPU = -999;
  this->OfflineDataPURMS = -999;
  this->OfflineBunchLumi = -999;
  this->TruePU = -999;
  this->GenEventWeight = -999;

  this->nGenParticle = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->GenParticle_ID[i] = -999;
    this->GenParticle_Status[i] = -999;
    this->GenParticle_Mother[i] = -999;

    this->GenParticle_Pt[i] = -999;
    this->GenParticle_Eta[i] = -999;
    this->GenParticle_Phi[i] = -999;
    this->GenParticle_Px[i] = -999;
    this->GenParticle_Py[i] = -999;
    this->GenParticle_Pz[i] = -999;
    this->GenParticle_E[i] = -999;
    this->GenParticle_Charge[i] = -999;

    this->GenParticle_isPrompt[i] = 0;
    this->GenParticle_isPromptFinalState[i] = 0;
    this->GenParticle_isTauDecayProduct[i] = 0;
    this->GenParticle_isPromptTauDecayProduct[i] = 0;
    this->GenParticle_isDirectPromptTauDecayProductFinalState[i] = 0;
    this->GenParticle_isHardProcess[i] = 0;
    this->GenParticle_isLastCopy[i] = 0;
    this->GenParticle_isLastCopyBeforeFSR[i] = 0;
    this->GenParticle_isPromptDecayed[i] = 0;
    this->GenParticle_isDecayedLeptonHadron[i] = 0;
    this->GenParticle_fromHardProcessBeforeFSR[i] = 0;
    this->GenParticle_fromHardProcessDecayed[i] = 0;
    this->GenParticle_fromHardProcessFinalState[i] = 0;
    this->GenParticle_isMostlyLikePythia6Status3[i] = 0;
  }

  // -- original trigger objects- - //
  this->vec_FiredTrigger.clear();
  this->vec_FilterName.clear();
  this->vec_HLTObj_Pt.clear();
  this->vec_HLTObj_Eta.clear();
  this->vec_HLTObj_Phi.clear();

  // -- HLT rerun objects -- //
  this->vec_MyFiredTrigger.clear();
  this->vec_MyFilterName.clear();
  this->vec_MyHLTObj_Pt.clear();
  this->vec_MyHLTObj_Eta.clear();
  this->vec_MyHLTObj_Phi.clear();

  this->Rho = -999;
  this->RhoECAL = -999;
  this->RhoHCAL = -999;

  this->nMuon = 0;

  for( int i=0; i<this->ArrSize; i++)
  {
    this->Muon_Pt[i] = -999;
    this->Muon_Eta[i] = -999;
    this->Muon_Phi[i] = -999;
    this->Muon_Px[i] = -999;
    this->Muon_Py[i] = -999;
    this->Muon_Pz[i] = -999;
    this->Muon_dB[i] = -999;
    this->Muon_Charge[i] = -999;
    this->Muon_IsGLB[i] = 0;
    this->Muon_IsSTA[i] = 0;
    this->Muon_IsTRK[i] = 0;
    this->Muon_IsPF[i] = 0;
    this->Muon_IsTight[i] = 0;
    this->Muon_IsMedium[i] = 0;
    this->Muon_IsLoose[i] = 0;
    this->Muon_IsHighPt[i] = 0;

    this->Muon_Iso03_sumPt[i] = -999;
    this->Muon_Iso03_hadEt[i] = -999;
    this->Muon_Iso03_emEt[i] = -999;

    this->Muon_PFIso03_Charged[i] = -999;
    this->Muon_PFIso03_Neutral[i] = -999;
    this->Muon_PFIso03_Photon[i] = -999;
    this->Muon_PFIso03_SumPU[i] = -999;

    this->Muon_PFIso04_Charged[i] = -999;
    this->Muon_PFIso04_Neutral[i] = -999;
    this->Muon_PFIso04_Photon[i] = -999;
    this->Muon_PFIso04_SumPU[i] = -999;

    this->Muon_PFCluster03_ECAL[i] = -999;
    this->Muon_PFCluster03_HCAL[i] = -999;

    this->Muon_PFCluster04_ECAL[i] = -999;
    this->Muon_PFCluster04_HCAL[i] = -999;

    this->Muon_NormChi2_GlbTrk[i] = -999;
    this->Muon_nTrackerHit_GlbTrk[i] = -999;
    this->Muon_nTrackerLayer_GlbTrk[i] = -999;
    this->Muon_nPixelHit_GlbTrk[i] = -999;
    this->Muon_nMuonHit_GlbTrk[i] = -999;

    this->Muon_NormChi2_InnerTrk[i] = -999;
    this->Muon_nTrackerHit_InnerTrk[i] = -999;
    this->Muon_nTrackerLayer_InnerTrk[i] = -999;
    this->Muon_nPixelHit_InnerTrk[i] = -999;

    this->Muon_Pt_TunePTrk[i] = -999;
    this->Muon_PtError_TunePTrk[i] = -999;

    this->Muon_dxyVTX_BestTrk[i] = -999;
    this->Muon_dzVTX_BestTrk[i] = -999;

    this->Muon_nMatchedStation[i] = -999;
    this->Muon_nMatchedRPCLayer[i] = -999;
    this->Muon_StationMask[i] = -999;
  }

  this->nL3Muon = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->L3Muon_Pt[i] = -999;
    this->L3Muon_Eta[i] = -999;
    this->L3Muon_Phi[i] = -999;
    this->L3Muon_Charge[i] = -999;
    this->L3Muon_TrkPt[i] = -999;
    this->L3Muon_TrkIso[i] = -999;
    this->L3Muon_ECALIso[i] = -999;
    this->L3Muon_HCALIso[i] = -999;
  }

  this->nL2Muon = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->L2Muon_Pt[i] = -999;
    this->L2Muon_Eta[i] = -999;
    this->L2Muon_Phi[i] = -999;
    this->L2Muon_Charge[i] = -999;
    this->L2Muon_TrkPt[i] = -999;
  }

  this->nTkMuon = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->TkMuon_Pt[i] = -999;
    this->TkMuon_Eta[i] = -999;
    this->TkMuon_Phi[i] = -999;
    this->TkMuon_Charge[i] = -999;
    this->TkMuon_TrkPt[i] = -999;
  }

  this->nL1Muon = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->L1Muon_Pt[i] = -999;
    this->L1Muon_Eta[i] = -999;
    this->L1Muon_Phi[i] = -999;
    this->L1Muon_Charge[i] = -999;
    this->L1Muon_Quality[i] = -999;
  }

    this->nIterL3OI = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->IterL3OI_TK_Pt[i] = -999;  //TracKer
    this->IterL3OI_TK_Eta[i] = -999;
    this->IterL3OI_TK_Phi[i] = -999;
    this->IterL3OI_TK_Charge[i] = -999;
    this->IterL3OI_SA_Pt[i] = -999;  //StandAlone
    this->IterL3OI_SA_Eta[i] = -999;
    this->IterL3OI_SA_Phi[i] = -999;
    this->IterL3OI_SA_Charge[i] = -999;
    this->IterL3OI_GL_Pt[i] = -999;  //GLobal
    this->IterL3OI_GL_Eta[i] = -999;
    this->IterL3OI_GL_Phi[i] = -999;
    this->IterL3OI_GL_Charge[i] = -999;
  }

  this->nIterL3IO_L2Seeded = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->IterL3IO_L2Seeded_TK_Pt[i] = -999;  //TracKer
    this->IterL3IO_L2Seeded_TK_Eta[i] = -999;
    this->IterL3IO_L2Seeded_TK_Phi[i] = -999;
    this->IterL3IO_L2Seeded_TK_Charge[i] = -999;
    this->IterL3IO_L2Seeded_SA_Pt[i] = -999;  //StandAlone
    this->IterL3IO_L2Seeded_SA_Eta[i] = -999;
    this->IterL3IO_L2Seeded_SA_Phi[i] = -999;
    this->IterL3IO_L2Seeded_SA_Charge[i] = -999;
    this->IterL3IO_L2Seeded_GL_Pt[i] = -999;  //GLobal
    this->IterL3IO_L2Seeded_GL_Eta[i] = -999;
    this->IterL3IO_L2Seeded_GL_Phi[i] = -999;
    this->IterL3IO_L2Seeded_GL_Charge[i] = -999;
  }

  this->nIterL3IO_FromL1 = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->IterL3IO_FromL1_Pt[i] = -999;  //reco::Tracks
    this->IterL3IO_FromL1_Eta[i] = -999;
    this->IterL3IO_FromL1_Phi[i] = -999;
    this->IterL3IO_FromL1_Charge[i] = -999;
  }

  this->nIterL3_FromL2 = 0;
  for( int i=0; i<this->ArrSize; i++)
  {
    this->IterL3_FromL2_TK_Pt[i] = -999;  //TracKer
    this->IterL3_FromL2_TK_Eta[i] = -999;
    this->IterL3_FromL2_TK_Phi[i] = -999;
    this->IterL3_FromL2_TK_Charge[i] = -999;
    this->IterL3_FromL2_SA_Pt[i] = -999;  //StandAlone
    this->IterL3_FromL2_SA_Eta[i] = -999;
    this->IterL3_FromL2_SA_Phi[i] = -999;
    this->IterL3_FromL2_SA_Charge[i] = -999;
    this->IterL3_FromL2_GL_Pt[i] = -999;  //GLobal
    this->IterL3_FromL2_GL_Eta[i] = -999;
    this->IterL3_FromL2_GL_Phi[i] = -999;
    this->IterL3_FromL2_GL_Charge[i] = -999;
  }

}

void ntupler::Make_Branch()
{
  this->ntuple->Branch("IsRealData", &IsRealData, "IsRealData/O"); // -- O: boolean -- //
  this->ntuple->Branch("RunNum",&RunNum,"RunNum/I");
  this->ntuple->Branch("LumiBlockNum",&LumiBlockNum,"LumiBlockNum/I");
  this->ntuple->Branch("EventNum",&EventNum,"EventNum/l"); // -- unsigned long long -- //
  this->ntuple->Branch("nVertices", &nVertices, "nVertices/I");
  this->ntuple->Branch("Rho_Offline", &Rho_Offline, "Rho_Offline/D");
  this->ntuple->Branch("BX_ID", &BX_ID, "BX_ID/D");
  this->ntuple->Branch("InstLumi", &InstLumi, "InstLumi/D");
  this->ntuple->Branch("DataPU", &DataPU, "DataPU/D");
  this->ntuple->Branch("DataPURMS", &DataPURMS, "DataPURMS/D");
  this->ntuple->Branch("BunchLumi", &BunchLumi, "BunchLumi/D");
  this->ntuple->Branch("OfflineInstLumi", &OfflineInstLumi, "OfflineInstLumi/D");
  this->ntuple->Branch("OfflineDataPU", &OfflineDataPU, "OfflineDataPU/D");
  this->ntuple->Branch("OfflineDataPURMS", &OfflineDataPURMS, "OfflineDataPURMS/D");
  this->ntuple->Branch("OfflineBunchLumi", &OfflineBunchLumi, "OfflineBunchLumi/D");
  this->ntuple->Branch("TruePU", &TruePU, "TruePU/I");

  this->ntuple->Branch("GenEventWeight", &GenEventWeight, "GenEventWeight/D");
  this->ntuple->Branch("nGenParticle", &nGenParticle, "nGenParticle/I");
  this->ntuple->Branch("GenParticle_ID", &GenParticle_ID, "GenParticle_ID[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_Status", &GenParticle_Status, "GenParticle_Status[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_Mother", &GenParticle_Mother, "GenParticle_Mother[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_Pt", &GenParticle_Pt, "GenParticle_Pt[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Eta", &GenParticle_Eta, "GenParticle_Eta[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Phi", &GenParticle_Phi, "GenParticle_Phi[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Px", &GenParticle_Px, "GenParticle_Px[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Py", &GenParticle_Py, "GenParticle_Py[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Pz", &GenParticle_Pz, "GenParticle_Pz[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_E", &GenParticle_E, "GenParticle_E[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_Charge", &GenParticle_Charge, "GenParticle_Charge[nGenParticle]/D");
  this->ntuple->Branch("GenParticle_isPrompt", &GenParticle_isPrompt, "GenParticle_isPrompt[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isPromptFinalState", &GenParticle_isPromptFinalState, "GenParticle_isPromptFinalState[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isTauDecayProduct", &GenParticle_isTauDecayProduct, "GenParticle_isTauDecayProduct[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isPromptTauDecayProduct", &GenParticle_isPromptTauDecayProduct, "GenParticle_isPromptTauDecayProduct[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isDirectPromptTauDecayProductFinalState", &GenParticle_isDirectPromptTauDecayProductFinalState, "GenParticle_isDirectPromptTauDecayProductFinalState[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isHardProcess", &GenParticle_isHardProcess, "GenParticle_isHardProcess[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isLastCopy", &GenParticle_isLastCopy, "GenParticle_isLastCopy[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isLastCopyBeforeFSR", &GenParticle_isLastCopyBeforeFSR, "GenParticle_isLastCopyBeforeFSR[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isPromptDecayed", &GenParticle_isPromptDecayed, "GenParticle_isPromptDecayed[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isDecayedLeptonHadron", &GenParticle_isDecayedLeptonHadron, "GenParticle_isDecayedLeptonHadron[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_fromHardProcessBeforeFSR", &GenParticle_fromHardProcessBeforeFSR, "GenParticle_fromHardProcessBeforeFSR[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_fromHardProcessDecayed", &GenParticle_fromHardProcessDecayed, "GenParticle_fromHardProcessDecayed[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_fromHardProcessFinalState", &GenParticle_fromHardProcessFinalState, "GenParticle_fromHardProcessFinalState[nGenParticle]/I");
  this->ntuple->Branch("GenParticle_isMostlyLikePythia6Status3", &GenParticle_isMostlyLikePythia6Status3, "GenParticle_isMostlyLikePythia6Status3[nGenParticle]/I");

  this->ntuple->Branch("vec_FiredTrigger", &vec_FiredTrigger);
  this->ntuple->Branch("vec_FilterName", &vec_FilterName);
  this->ntuple->Branch("vec_HLTObj_Pt", &vec_HLTObj_Pt);
  this->ntuple->Branch("vec_HLTObj_Eta", &vec_HLTObj_Eta);
  this->ntuple->Branch("vec_HLTObj_Phi", &vec_HLTObj_Phi);

  this->ntuple->Branch("vec_MyFiredTrigger", &vec_MyFiredTrigger);
  this->ntuple->Branch("vec_MyFilterName", &vec_MyFilterName);
  this->ntuple->Branch("vec_MyHLTObj_Pt", &vec_MyHLTObj_Pt);
  this->ntuple->Branch("vec_MyHLTObj_Eta", &vec_MyHLTObj_Eta);
  this->ntuple->Branch("vec_MyHLTObj_Phi", &vec_MyHLTObj_Phi);

  this->ntuple->Branch("Rho", &Rho, "Rho/D");
  this->ntuple->Branch("RhoECAL", &RhoECAL, "RhoECAL/D");
  this->ntuple->Branch("RhoHCAL", &RhoHCAL, "RhoHCAL/D");

  this->ntuple->Branch("nMuon", &nMuon, "nMuon/I");

  this->ntuple->Branch("Muon_Pt", &Muon_Pt, "Muon_Pt[nMuon]/D");
  this->ntuple->Branch("Muon_Eta", &Muon_Eta, "Muon_Eta[nMuon]/D");
  this->ntuple->Branch("Muon_Phi", &Muon_Phi, "Muon_Phi[nMuon]/D");
  this->ntuple->Branch("Muon_Px", &Muon_Px, "Muon_Px[nMuon]/D");
  this->ntuple->Branch("Muon_Py", &Muon_Py, "Muon_Py[nMuon]/D");
  this->ntuple->Branch("Muon_Pz", &Muon_Pz, "Muon_Pz[nMuon]/D");
  this->ntuple->Branch("Muon_dB", &Muon_dB, "Muon_dB[nMuon]/D");
  this->ntuple->Branch("Muon_Charge", &Muon_Charge, "Muon_Charge[nMuon]/D");
  this->ntuple->Branch("Muon_IsGLB", &Muon_IsGLB, "Muon_IsGLB[nMuon]/I");
  this->ntuple->Branch("Muon_IsSTA", &Muon_IsSTA, "Muon_IsSTA[nMuon]/I");
  this->ntuple->Branch("Muon_IsTRK", &Muon_IsTRK, "Muon_IsTRK[nMuon]/I");
  this->ntuple->Branch("Muon_IsPF", &Muon_IsPF, "Muon_IsPF[nMuon]/I");
  this->ntuple->Branch("Muon_IsTight", &Muon_IsTight, "Muon_IsTight[nMuon]/I");
  this->ntuple->Branch("Muon_IsMedium", &Muon_IsMedium, "Muon_IsMedium[nMuon]/I");
  this->ntuple->Branch("Muon_IsLoose", &Muon_IsLoose, "Muon_IsLoose[nMuon]/I");
  this->ntuple->Branch("Muon_IsHighPt", &Muon_IsHighPt, "Muon_IsHighPt[nMuon]/I");

  this->ntuple->Branch("Muon_Iso03_sumPt", &Muon_Iso03_sumPt, "Muon_Iso03_sumPt[nMuon]/D");
  this->ntuple->Branch("Muon_Iso03_hadEt", &Muon_Iso03_hadEt, "Muon_Iso03_hadEt[nMuon]/D");
  this->ntuple->Branch("Muon_Iso03_emEt", &Muon_Iso03_emEt, "Muon_Iso03_emEt[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso03_Charged", &Muon_PFIso03_Charged, "Muon_PFIso03_Charged[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso03_Neutral", &Muon_PFIso03_Neutral, "Muon_PFIso03_Neutral[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso03_Photon", &Muon_PFIso03_Photon, "Muon_PFIso03_Photon[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso03_SumPU", &Muon_PFIso03_SumPU, "Muon_PFIso03_SumPU[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso04_Charged", &Muon_PFIso04_Charged, "Muon_PFIso04_Charged[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso04_Neutral", &Muon_PFIso04_Neutral, "Muon_PFIso04_Neutral[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso04_Photon", &Muon_PFIso04_Photon, "Muon_PFIso04_Photon[nMuon]/D");
  this->ntuple->Branch("Muon_PFIso04_SumPU", &Muon_PFIso04_SumPU, "Muon_PFIso04_SumPU[nMuon]/D");

  this->ntuple->Branch("Muon_PFCluster03_ECAL", &Muon_PFCluster03_ECAL, "Muon_PFCluster03_ECAL[nMuon]/D");
  this->ntuple->Branch("Muon_PFCluster03_HCAL", &Muon_PFCluster03_HCAL, "Muon_PFCluster03_HCAL[nMuon]/D");
  this->ntuple->Branch("Muon_PFCluster04_ECAL", &Muon_PFCluster04_ECAL, "Muon_PFCluster04_ECAL[nMuon]/D");
  this->ntuple->Branch("Muon_PFCluster04_HCAL", &Muon_PFCluster04_HCAL, "Muon_PFCluster04_HCAL[nMuon]/D");
  this->ntuple->Branch("Muon_NormChi2_GlbTrk", &Muon_NormChi2_GlbTrk, "Muon_NormChi2_GlbTrk[nMuon]/D");
  this->ntuple->Branch("Muon_nTrackerHit_GlbTrk", &Muon_nTrackerHit_GlbTrk, "Muon_nTrackerHit_GlbTrk[nMuon]/I");
  this->ntuple->Branch("Muon_nTrackerLayer_GlbTrk", &Muon_nTrackerLayer_GlbTrk, "Muon_nTrackerLayer_GlbTrk[nMuon]/I");
  this->ntuple->Branch("Muon_nPixelHit_GlbTrk", &Muon_nPixelHit_GlbTrk, "Muon_nPixelHit_GlbTrk[nMuon]/I");
  this->ntuple->Branch("Muon_nMuonHit_GlbTrk", &Muon_nMuonHit_GlbTrk, "Muon_nMuonHit_GlbTrk[nMuon]/I");
  this->ntuple->Branch("Muon_NormChi2_InnerTrk", &Muon_NormChi2_InnerTrk, "Muon_NormChi2_InnerTrk[nMuon]/D");
  this->ntuple->Branch("Muon_nTrackerHit_InnerTrk", &Muon_nTrackerHit_InnerTrk, "Muon_nTrackerHit_InnerTrk[nMuon]/I");
  this->ntuple->Branch("Muon_nTrackerLayer_InnerTrk", &Muon_nTrackerLayer_InnerTrk, "Muon_nTrackerLayer_InnerTrk[nMuon]/I");
  this->ntuple->Branch("Muon_nPixelHit_InnerTrk", &Muon_nPixelHit_InnerTrk, "Muon_nPixelHit_InnerTrk[nMuon]/I");
  this->ntuple->Branch("Muon_Pt_TunePTrk", &Muon_Pt_TunePTrk, "Muon_Pt_TunePTrk[nMuon]/D");
  this->ntuple->Branch("Muon_PtError_TunePTrk", &Muon_PtError_TunePTrk, "Muon_PtError_TunePTrk[nMuon]/D");
  this->ntuple->Branch("Muon_dxyVTX_BestTrk", &Muon_dxyVTX_BestTrk, "Muon_dxyVTX_BestTrk[nMuon]/D");
  this->ntuple->Branch("Muon_dzVTX_BestTrk", &Muon_dzVTX_BestTrk, "Muon_dzVTX_BestTrk[nMuon]/D");
  this->ntuple->Branch("Muon_nMatchedStation", &Muon_nMatchedStation, "Muon_nMatchedStation[nMuon]/I");
  this->ntuple->Branch("Muon_nMatchedRPCLayer", &Muon_nMatchedRPCLayer, "Muon_nMatchedRPCLayer[nMuon]/I");
  this->ntuple->Branch("Muon_StationMask", &Muon_StationMask, "Muon_StationMask[nMuon]/I");

  this->ntuple->Branch("nL3Muon", &nL3Muon, "nL3Muon/I");
  this->ntuple->Branch("L3Muon_Pt", &L3Muon_Pt, "L3Muon_Pt[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_Eta", &L3Muon_Eta, "L3Muon_Eta[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_Phi", &L3Muon_Phi, "L3Muon_Phi[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_Charge", &L3Muon_Charge, "L3Muon_Charge[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_TrkPt", &L3Muon_TrkPt, "L3Muon_TrkPt[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_TrkIso", &L3Muon_TrkIso, "L3Muon_TrkIso[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_ECALIso", &L3Muon_ECALIso, "L3Muon_ECALIso[nL3Muon]/D");
  this->ntuple->Branch("L3Muon_HCALIso", &L3Muon_HCALIso, "L3Muon_HCALIso[nL3Muon]/D");

  this->ntuple->Branch("nL2Muon", &nL2Muon, "nL2Muon/I");
  this->ntuple->Branch("L2Muon_Pt", &L2Muon_Pt, "L2Muon_Pt[nL2Muon]/D");
  this->ntuple->Branch("L2Muon_Eta", &L2Muon_Eta, "L2Muon_Eta[nL2Muon]/D");
  this->ntuple->Branch("L2Muon_Phi", &L2Muon_Phi, "L2Muon_Phi[nL2Muon]/D");
  this->ntuple->Branch("L2Muon_Charge", &L2Muon_Charge, "L2Muon_Charge[nL2Muon]/D");
  this->ntuple->Branch("L2Muon_TrkPt", &L2Muon_TrkPt, "L2Muon_TrkPt[nL2Muon]/D");

  this->ntuple->Branch("nTkMuon", &nTkMuon, "nTkMuon/I");
  this->ntuple->Branch("TkMuon_Pt", &TkMuon_Pt, "TkMuon_Pt[nTkMuon]/D");
  this->ntuple->Branch("TkMuon_Eta", &TkMuon_Eta, "TkMuon_Eta[nTkMuon]/D");
  this->ntuple->Branch("TkMuon_Phi", &TkMuon_Phi, "TkMuon_Phi[nTkMuon]/D");
  this->ntuple->Branch("TkMuon_Charge", &TkMuon_Charge, "TkMuon_Charge[nTkMuon]/D");
  this->ntuple->Branch("TkMuon_TrkPt", &TkMuon_TrkPt, "TkMuon_TrkPt[nTkMuon]/D");

  this->ntuple->Branch("nIterL3OI", &nIterL3OI, "nIterL3OI/I");
  this->ntuple->Branch("IterL3OI_TK_Pt", &IterL3OI_TK_Pt, "IterL3OI_TK_Pt[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_TK_Eta", &IterL3OI_TK_Eta, "IterL3OI_TK_Eta[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_TK_Phi", &IterL3OI_TK_Phi, "IterL3OI_TK_Phi[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_TK_Charge", &IterL3OI_TK_Charge, "IterL3OI_TK_Charge[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_SA_Pt", &IterL3OI_SA_Pt, "IterL3OI_SA_Pt[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_SA_Eta", &IterL3OI_SA_Eta, "IterL3OI_SA_Eta[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_SA_Phi", &IterL3OI_SA_Phi, "IterL3OI_SA_Phi[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_SA_Charge", &IterL3OI_SA_Charge, "IterL3OI_SA_Charge[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_GL_Pt", &IterL3OI_GL_Pt, "IterL3OI_GL_Pt[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_GL_Eta", &IterL3OI_GL_Eta, "IterL3OI_GL_Eta[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_GL_Phi", &IterL3OI_GL_Phi, "IterL3OI_GL_Phi[nIterL3OI]/D");
  this->ntuple->Branch("IterL3OI_GL_Charge", &IterL3OI_GL_Charge, "IterL3OI_GL_Charge[nIterL3OI]/D");

  this->ntuple->Branch("nIterL3IO_L2Seeded", &nIterL3IO_L2Seeded, "nIterL3IO_L2Seeded/I");
  this->ntuple->Branch("IterL3IO_L2Seeded_TK_Pt", &IterL3IO_L2Seeded_TK_Pt, "IterL3IO_L2Seeded_TK_Pt[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_TK_Eta", &IterL3IO_L2Seeded_TK_Eta, "IterL3IO_L2Seeded_TK_Eta[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_TK_Phi", &IterL3IO_L2Seeded_TK_Phi, "IterL3IO_L2Seeded_TK_Phi[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_TK_Charge", &IterL3IO_L2Seeded_TK_Charge, "IterL3IO_L2Seeded_TK_Charge[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_SA_Pt", &IterL3IO_L2Seeded_SA_Pt, "IterL3IO_L2Seeded_SA_Pt[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_SA_Eta", &IterL3IO_L2Seeded_SA_Eta, "IterL3IO_L2Seeded_SA_Eta[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_SA_Phi", &IterL3IO_L2Seeded_SA_Phi, "IterL3IO_L2Seeded_SA_Phi[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_SA_Charge", &IterL3IO_L2Seeded_SA_Charge, "IterL3IO_L2Seeded_SA_Charge[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_GL_Pt", &IterL3IO_L2Seeded_GL_Pt, "IterL3IO_L2Seeded_GL_Pt[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_GL_Eta", &IterL3IO_L2Seeded_GL_Eta, "IterL3IO_L2Seeded_GL_Eta[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_GL_Phi", &IterL3IO_L2Seeded_GL_Phi, "IterL3IO_L2Seeded_GL_Phi[nIterL3IO_L2Seeded]/D");
  this->ntuple->Branch("IterL3IO_L2Seeded_GL_Charge", &IterL3IO_L2Seeded_GL_Charge, "IterL3IO_L2Seeded_GL_Charge[nIterL3IO_L2Seeded]/D");

  this->ntuple->Branch("nIterL3IO_FromL1", &nIterL3IO_FromL1, "nIterL3IO_FromL1/I");
  this->ntuple->Branch("IterL3IO_FromL1_Pt", &IterL3IO_FromL1_Pt, "IterL3IO_FromL1_Pt[nIterL3IO_FromL1]/D");
  this->ntuple->Branch("IterL3IO_FromL1_Eta", &IterL3IO_FromL1_Eta, "IterL3IO_FromL1_Eta[nIterL3IO_FromL1]/D");
  this->ntuple->Branch("IterL3IO_FromL1_Phi", &IterL3IO_FromL1_Phi, "IterL3IO_FromL1_Phi[nIterL3IO_FromL1]/D");
  this->ntuple->Branch("IterL3IO_FromL1_Charge", &IterL3IO_FromL1_Charge, "IterL3IO_FromL1_Charge[nIterL3IO_FromL1]/D");

  this->ntuple->Branch("nIterL3_FromL2", &nIterL3_FromL2, "nIterL3_FromL2/I");
  this->ntuple->Branch("IterL3_FromL2_TK_Pt", &IterL3_FromL2_TK_Pt, "IterL3_FromL2_TK_Pt[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_TK_Eta", &IterL3_FromL2_TK_Eta, "IterL3_FromL2_TK_Eta[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_TK_Phi", &IterL3_FromL2_TK_Phi, "IterL3_FromL2_TK_Phi[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_TK_Charge", &IterL3_FromL2_TK_Charge, "IterL3_FromL2_TK_Charge[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_SA_Pt", &IterL3_FromL2_SA_Pt, "IterL3_FromL2_SA_Pt[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_SA_Eta", &IterL3_FromL2_SA_Eta, "IterL3_FromL2_SA_Eta[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_SA_Phi", &IterL3_FromL2_SA_Phi, "IterL3_FromL2_SA_Phi[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_SA_Charge", &IterL3_FromL2_SA_Charge, "IterL3_FromL2_SA_Charge[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_GL_Pt", &IterL3_FromL2_GL_Pt, "IterL3_FromL2_GL_Pt[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_GL_Eta", &IterL3_FromL2_GL_Eta, "IterL3_FromL2_GL_Eta[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_GL_Phi", &IterL3_FromL2_GL_Phi, "IterL3_FromL2_GL_Phi[nIterL3_FromL2]/D");
  this->ntuple->Branch("IterL3_FromL2_GL_Charge", &IterL3_FromL2_GL_Charge, "IterL3_FromL2_GL_Charge[nIterL3_FromL2]/D");

}

void ntupler::Fill_Muon(const edm::Event &iEvent)
{
  edm::Handle<std::vector<reco::Muon> > Handle_OfflineMuon;
  // edm::Handle< edm::View<reco::Muon> > Handle_OfflineMuon;
  if( iEvent.getByToken(Token_OfflineMuon, Handle_OfflineMuon) ) // -- only when the dataset has offline muon collection (e.g. AOD) -- //
  {
    edm::Handle<reco::VertexCollection> Handle_Vertex;
    iEvent.getByToken(Token_OfflineVertex, Handle_Vertex);
    const reco::Vertex & pv = Handle_Vertex->at(0);

    edm::Handle< edm::ValueMap<float> > Handle_OfflineECALPFIso03;
    edm::Handle< edm::ValueMap<float> > Handle_OfflineHCALPFIso03;
    bool Flag_Valid_Iso03 = false;
    if( iEvent.getByToken( Token_OfflineECALPFIso03, Handle_OfflineECALPFIso03 ) &&
      iEvent.getByToken( Token_OfflineHCALPFIso03, Handle_OfflineHCALPFIso03 ) ) Flag_Valid_Iso03 = true;

    edm::Handle< edm::ValueMap<float> > Handle_OfflineECALPFIso04;
    edm::Handle< edm::ValueMap<float> > Handle_OfflineHCALPFIso04;
    bool Flag_Valid_Iso04 = false;
    if( iEvent.getByToken( Token_OfflineECALPFIso04, Handle_OfflineECALPFIso04 ) &&
      iEvent.getByToken( Token_OfflineHCALPFIso04, Handle_OfflineHCALPFIso04 ) ) Flag_Valid_Iso04 = true;

    int _nMuon = 0;
    for(std::vector<reco::Muon>::const_iterator mu=Handle_OfflineMuon->begin(); mu!=Handle_OfflineMuon->end(); ++mu)
    {
      this->Muon_Pt[_nMuon] = mu->pt();
      this->Muon_Eta[_nMuon] = mu->eta();
      this->Muon_Phi[_nMuon] = mu->phi();
      this->Muon_Px[_nMuon] = mu->px();
      this->Muon_Py[_nMuon] = mu->py();
      this->Muon_Pz[_nMuon] = mu->pz();
      // this->Muon_dB[_nMuon] = mu->dB(); // -- dB is only availabe in pat::Muon -- //

      this->Muon_Charge[_nMuon] = mu->charge();

      if( mu->isGlobalMuon() ) this->Muon_IsGLB[_nMuon] = 1;
      if( mu->isStandAloneMuon() ) this->Muon_IsSTA[_nMuon] = 1;
      if( mu->isTrackerMuon() ) this->Muon_IsTRK[_nMuon] = 1;
      if( mu->isPFMuon() ) this->Muon_IsPF[_nMuon] = 1;
      // -- defintion of ID functions: http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_9_4_0/doc/html/da/d18/namespacemuon.html#ac122b2516e5711ce206256d7945473d2 -- //
      if( muon::isTightMuon( (*mu), pv ) ) this->Muon_IsTight[_nMuon] = 1;
      if( muon::isMediumMuon( (*mu) ) ) this->Muon_IsMedium[_nMuon] = 1;
      if( muon::isLooseMuon( (*mu) ) ) this->Muon_IsLoose[_nMuon] = 1;
      if( muon::isHighPtMuon( (*mu), pv ) ) this->Muon_IsHighPt[_nMuon] = 1;

      this->Muon_Iso03_sumPt[_nMuon] = mu->isolationR03().sumPt;
      this->Muon_Iso03_hadEt[_nMuon] = mu->isolationR03().hadEt;
      this->Muon_Iso03_emEt[_nMuon] = mu->isolationR03().emEt;

      this->Muon_PFIso03_Charged[_nMuon] = mu->pfIsolationR03().sumChargedHadronPt;
      this->Muon_PFIso03_Neutral[_nMuon] = mu->pfIsolationR03().sumNeutralHadronEt;
      this->Muon_PFIso03_Photon[_nMuon] = mu->pfIsolationR03().sumPhotonEt;
      this->Muon_PFIso03_SumPU[_nMuon] = mu->pfIsolationR03().sumPUPt;

      this->Muon_PFIso04_Charged[_nMuon] = mu->pfIsolationR04().sumChargedHadronPt;
      this->Muon_PFIso04_Neutral[_nMuon] = mu->pfIsolationR04().sumNeutralHadronEt;
      this->Muon_PFIso04_Photon[_nMuon] = mu->pfIsolationR04().sumPhotonEt;
      this->Muon_PFIso04_SumPU[_nMuon] = mu->pfIsolationR04().sumPUPt;

      reco::MuonRef Ref_mu = reco::MuonRef(Handle_OfflineMuon, _nMuon);

      if( Flag_Valid_Iso03 )
      {
        const edm::ValueMap<float> ECALIso03 = *(Handle_OfflineECALPFIso03);
        const edm::ValueMap<float> HCALIso03 = *(Handle_OfflineHCALPFIso03);
        this->Muon_PFCluster03_ECAL[_nMuon] = ECALIso03[Ref_mu];
        this->Muon_PFCluster03_HCAL[_nMuon] = HCALIso03[Ref_mu];
      }

      if( Flag_Valid_Iso04 )
      {
        const edm::ValueMap<float> ECALIso04 = *(Handle_OfflineECALPFIso04);
        const edm::ValueMap<float> HCALIso04 = *(Handle_OfflineHCALPFIso04);
        this->Muon_PFCluster04_ECAL[_nMuon] = ECALIso04[Ref_mu];
        this->Muon_PFCluster04_HCAL[_nMuon] = HCALIso04[Ref_mu];
      }

      reco::TrackRef GlbTrk = mu->globalTrack();
      if( GlbTrk.isNonnull() )
      {
        this->Muon_NormChi2_GlbTrk[_nMuon] = GlbTrk->normalizedChi2();

        const reco::HitPattern & GlbHit = GlbTrk->hitPattern();
        this->Muon_nTrackerHit_GlbTrk[_nMuon] = GlbHit.numberOfValidTrackerHits();
        this->Muon_nTrackerLayer_GlbTrk[_nMuon] = GlbHit.trackerLayersWithMeasurement();
        this->Muon_nPixelHit_GlbTrk[_nMuon] = GlbHit.numberOfValidPixelHits();
        this->Muon_nMuonHit_GlbTrk[_nMuon] = GlbHit.numberOfValidMuonHits();
      }

      reco::TrackRef InnerTrk = mu->innerTrack();
      if( InnerTrk.isNonnull() )
      {
        this->Muon_NormChi2_InnerTrk[_nMuon] = InnerTrk->normalizedChi2();

        const reco::HitPattern & InnerHit = InnerTrk->hitPattern();
        this->Muon_nTrackerHit_InnerTrk[_nMuon] = InnerHit.numberOfValidTrackerHits();
        this->Muon_nTrackerLayer_InnerTrk[_nMuon] = InnerHit.trackerLayersWithMeasurement();
        this->Muon_nPixelHit_InnerTrk[_nMuon] = InnerHit.numberOfValidPixelHits();
      }

      reco::TrackRef TunePTrk = mu->tunePMuonBestTrack();
      if( TunePTrk.isNonnull() )
      {
        this->Muon_Pt_TunePTrk[_nMuon] = TunePTrk->pt();
        this->Muon_PtError_TunePTrk[_nMuon] = TunePTrk->ptError();
      }

      this->Muon_dxyVTX_BestTrk[_nMuon] = mu->muonBestTrack()->dxy( pv.position() );
      this->Muon_dzVTX_BestTrk[_nMuon] = mu->muonBestTrack()->dz( pv.position() );

      this->Muon_nMatchedStation[_nMuon] = mu->numberOfMatchedStations();
      this->Muon_nMatchedRPCLayer[_nMuon] = mu->numberOfMatchedRPCLayers();
      this->Muon_StationMask[_nMuon] = mu->stationMask();

      _nMuon++;
    }
    this->nMuon = _nMuon;

  }
  // else
  // {
  //  std::cout << "Handle_OfflineMuon is not valid ... muon variables will not be filled" << std::endl;
  // }

}

void ntupler::Fill_HLT(const edm::Event &iEvent)
{
  edm::Handle<edm::TriggerResults>  Handle_TriggerResults;
  iEvent.getByToken(Token_TriggerResults, Handle_TriggerResults);

  edm::Handle<trigger::TriggerEvent> Handle_TriggerEvent;
  iEvent.getByToken(Token_TriggerEvent, Handle_TriggerEvent);

  edm::TriggerNames TrigNames = iEvent.triggerNames(*Handle_TriggerResults);

  for(unsigned int itrig=0; itrig<TrigNames.size(); ++itrig)
  {
    LogDebug("triggers") << TrigNames.triggerName(itrig);

    if( Handle_TriggerResults->accept(itrig) )
    {
      std::string PathName = TrigNames.triggerName(itrig);

      if( PathName.find("HLT_IsoMu") != std::string::npos ||
        PathName.find("HLT_Mu45") != std::string::npos ||
        PathName.find("HLT_Mu5") != std::string::npos ||
        PathName.find("HLT_TkMu5") != std::string::npos ||
        PathName.find("HLT_IsoTkMu") != std::string::npos ||
        PathName.find("HLT_Mu17") != std::string::npos ||
        PathName.find("HLT_Mu8_T") != std::string::npos ) this->vec_FiredTrigger.push_back( PathName );
    } // -- if fired -- //
  } // -- iteration over all trigger names -- //

  const trigger::size_type nFilters(Handle_TriggerEvent->sizeFilters());
  for( trigger::size_type i_filter=0; i_filter<nFilters; i_filter++)
  {
    std::string filterTag = Handle_TriggerEvent->filterTag(i_filter).encode();

    if( filterTag.find("sMu") != std::string::npos &&
      filterTag.find("Tau") == std::string::npos &&
      filterTag.find("EG") == std::string::npos &&
      filterTag.find("MultiFit") == std::string::npos )
    {
      trigger::Keys objectKeys = Handle_TriggerEvent->filterKeys(i_filter);
      const trigger::TriggerObjectCollection& triggerObjects(Handle_TriggerEvent->getObjects());

      for( trigger::size_type i_key=0; i_key<objectKeys.size(); i_key++)
      {
        trigger::size_type objKey = objectKeys.at(i_key);
        const trigger::TriggerObject& triggerObj(triggerObjects[objKey]);

        this->vec_FilterName.push_back( filterTag );
        this->vec_HLTObj_Pt.push_back( triggerObj.pt() );
        this->vec_HLTObj_Eta.push_back( triggerObj.eta() );
        this->vec_HLTObj_Phi.push_back( triggerObj.phi() );
      }
    } // -- end of if( muon filters )-- //
  } // -- end of filter iteration -- //
}

// -- basically same with Fill_HLT except for rho variable part -- //
void ntupler::Fill_MYHLT(const edm::Event &iEvent)
{
  edm::Handle<edm::TriggerResults>  Handle_TriggerResults;
  iEvent.getByToken(Token_MyTriggerResults, Handle_TriggerResults);

  edm::Handle<trigger::TriggerEvent> Handle_TriggerEvent;
  iEvent.getByToken(Token_MyTriggerEvent, Handle_TriggerEvent);

  edm::TriggerNames TrigNames = iEvent.triggerNames(*Handle_TriggerResults);

  for(unsigned int itrig=0; itrig<TrigNames.size(); ++itrig)
  {
    LogDebug("triggers") << TrigNames.triggerName(itrig);

    if( Handle_TriggerResults->accept(itrig) )
    {
      std::string PathName = TrigNames.triggerName(itrig);

      if( PathName.find("HLT_IsoMu") != std::string::npos ||
        PathName.find("HLT_Mu45") != std::string::npos ||
        PathName.find("HLT_Mu5") != std::string::npos ||
        PathName.find("HLT_TkMu5") != std::string::npos ||
        PathName.find("HLT_IsoTkMu") != std::string::npos ||
        PathName.find("HLT_Mu17") != std::string::npos ||
        PathName.find("HLT_Mu8_T") != std::string::npos ) this->vec_MyFiredTrigger.push_back( PathName );
    } // -- if fired -- //
  } // -- iteration over all trigger names -- //

  const trigger::size_type nFilters(Handle_TriggerEvent->sizeFilters());
  for( trigger::size_type i_filter=0; i_filter<nFilters; i_filter++)
  {
    std::string filterTag = Handle_TriggerEvent->filterTag(i_filter).encode();

    if( filterTag.find("sMu") != std::string::npos &&
      filterTag.find("Tau") == std::string::npos &&
      filterTag.find("EG") == std::string::npos &&
      filterTag.find("MultiFit") == std::string::npos )
    {
      trigger::Keys objectKeys = Handle_TriggerEvent->filterKeys(i_filter);
      const trigger::TriggerObjectCollection& triggerObjects(Handle_TriggerEvent->getObjects());

      for( trigger::size_type i_key=0; i_key<objectKeys.size(); i_key++)
      {
        trigger::size_type objKey = objectKeys.at(i_key);
        const trigger::TriggerObject& triggerObj(triggerObjects[objKey]);

        this->vec_MyFilterName.push_back( filterTag );
        this->vec_MyHLTObj_Pt.push_back( triggerObj.pt() );
        this->vec_MyHLTObj_Eta.push_back( triggerObj.eta() );
        this->vec_MyHLTObj_Phi.push_back( triggerObj.phi() );
      }
    } // -- end of if( muon filters )-- //
  } // -- end of filter iteration -- //

  edm::Handle <double>  Handle_Rho;
  if(iEvent.getByToken(Token_Rho, Handle_Rho) && Handle_Rho.isValid())
    this->Rho = *(Handle_Rho.product());

  edm::Handle <double>  Handle_RhoECAL;
  if(iEvent.getByToken(Token_RhoECAL, Handle_RhoECAL) && Handle_RhoECAL.isValid())
    this->RhoECAL = *(Handle_RhoECAL.product());

  edm::Handle <double>  Handle_RhoHCAL;
  if(iEvent.getByToken(Token_RhoHCAL, Handle_RhoHCAL) && Handle_RhoHCAL.isValid())
    this->RhoHCAL = *(Handle_RhoHCAL.product());
}

void ntupler::Fill_HLTMuon(const edm::Event &iEvent)
{
  ///////////////////
  // -- L3 Muon -- //
  ///////////////////
  edm::Handle<reco::RecoChargedCandidateCollection> Handle_L3Muon;
  if( iEvent.getByToken( Token_L3Muon, Handle_L3Muon ) )
  {
    edm::Handle<reco::IsoDepositMap> Handle_ChargedIsoDep;
    iEvent.getByToken(Token_ChargedIsoDep, Handle_ChargedIsoDep);

    edm::Handle<reco::RecoChargedCandidateIsolationMap> Handle_NeutralIsoDep;
    iEvent.getByToken(Token_NeutralIsoDep, Handle_NeutralIsoDep);

    edm::Handle<reco::RecoChargedCandidateIsolationMap> Handle_PhotonIsoDep;
    iEvent.getByToken(Token_PhotonIsoDep, Handle_PhotonIsoDep);

    int _nL3Muon = 0;
    for( unsigned int i_L3=0; i_L3<Handle_L3Muon->size(); i_L3++)
    {
      reco::RecoChargedCandidateRef Ref_L3Mu(Handle_L3Muon, _nL3Muon);

      this->L3Muon_Pt[_nL3Muon] = Ref_L3Mu->pt();
      this->L3Muon_Eta[_nL3Muon] = Ref_L3Mu->eta();
      this->L3Muon_Phi[_nL3Muon] = Ref_L3Mu->phi();
      this->L3Muon_Charge[_nL3Muon] = Ref_L3Mu->charge();

      reco::TrackRef Track = Ref_L3Mu->track();
      this->L3Muon_TrkPt[_nL3Muon] = Track->pt();

      if( Handle_ChargedIsoDep.isValid() )
      {
        reco::IsoDeposit TrkIsoDeposit = (*Handle_ChargedIsoDep)[Ref_L3Mu];
        this->L3Muon_TrkIso[_nL3Muon] = TrkIsoDeposit.depositWithin(0.3);
      }

      if( Handle_NeutralIsoDep.isValid() )
      {
        reco::RecoChargedCandidateIsolationMap::const_iterator HCALIsoDeposit = (*Handle_NeutralIsoDep).find( Ref_L3Mu );
        this->L3Muon_HCALIso[_nL3Muon] = HCALIsoDeposit->val;
      }

      if( Handle_PhotonIsoDep.isValid() )
      {
        reco::RecoChargedCandidateIsolationMap::const_iterator ECALIsoDeposit = (*Handle_PhotonIsoDep).find( Ref_L3Mu );
        this->L3Muon_ECALIso[_nL3Muon] = ECALIsoDeposit->val;
      }

      _nL3Muon++;
    }
    this->nL3Muon = _nL3Muon;
  } // -- if( L3 handle is valid ) -- //


  ///////////////////
  // -- L2 Muon -- //
  ///////////////////
  edm::Handle<reco::RecoChargedCandidateCollection> Handle_L2Muon;
  if( iEvent.getByToken( Token_L2Muon, Handle_L2Muon ) )
  {
    int _nL2Muon = 0;
    for( unsigned int i_L2=0; i_L2<Handle_L2Muon->size(); i_L2++)
    {
      reco::RecoChargedCandidateRef Ref_L2Mu(Handle_L2Muon, _nL2Muon);

      this->L2Muon_Pt[_nL2Muon] = Ref_L2Mu->pt();
      this->L2Muon_Eta[_nL2Muon] = Ref_L2Mu->eta();
      this->L2Muon_Phi[_nL2Muon] = Ref_L2Mu->phi();
      this->L2Muon_Charge[_nL2Muon] = Ref_L2Mu->charge();

      reco::TrackRef Track = Ref_L2Mu->track();
      this->L2Muon_TrkPt[_nL2Muon] = Track->pt();

      _nL2Muon++;
    }
    this->nL2Muon = _nL2Muon;
  }


  ///////////////////
  // -- Tk Muon -- //
  ///////////////////
  edm::Handle<reco::RecoChargedCandidateCollection> Handle_TkMuon;
  if( iEvent.getByToken( Token_TkMuon, Handle_TkMuon ) )
  {
    int _nTkMuon = 0;
    for( unsigned int i_Tk=0; i_Tk<Handle_TkMuon->size(); i_Tk++)
    {
      reco::RecoChargedCandidateRef Ref_TkMu(Handle_TkMuon, _nTkMuon);

      this->TkMuon_Pt[_nTkMuon] = Ref_TkMu->pt();
      this->TkMuon_Eta[_nTkMuon] = Ref_TkMu->eta();
      this->TkMuon_Phi[_nTkMuon] = Ref_TkMu->phi();
      this->TkMuon_Charge[_nTkMuon] = Ref_TkMu->charge();

      reco::TrackRef Track = Ref_TkMu->track();
      this->TkMuon_TrkPt[_nTkMuon] = Track->pt();

      _nTkMuon++;
    }
    this->nTkMuon = _nTkMuon;
  }
}

void ntupler::Fill_L1Muon(const edm::Event &iEvent)
{
  edm::Handle<l1t::MuonBxCollection> Handle_L1Muon;
  if( iEvent.getByToken(Token_L1Muon, Handle_L1Muon) )
  {
    int _nL1Muon = 0;
    for(int ibx = Handle_L1Muon->getFirstBX(); ibx<=Handle_L1Muon->getLastBX(); ++ibx)
    {
      if(ibx != 0) continue; // -- only take when ibx == 0 -- //
      for(auto it=Handle_L1Muon->begin(ibx); it!=Handle_L1Muon->end(ibx); it++)
      {
        l1t::MuonRef Ref_L1Mu(Handle_L1Muon, distance(Handle_L1Muon->begin(Handle_L1Muon->getFirstBX()), it) );

        this->L1Muon_Pt[_nL1Muon] = Ref_L1Mu->pt();
        this->L1Muon_Eta[_nL1Muon] = Ref_L1Mu->eta();
        this->L1Muon_Phi[_nL1Muon] = Ref_L1Mu->phi();
        this->L1Muon_Charge[_nL1Muon] = Ref_L1Mu->charge();
        this->L1Muon_Quality[_nL1Muon] = Ref_L1Mu->hwQual();

        _nL1Muon++;
      }
    }
    this->nL1Muon = _nL1Muon;
  }
}

void ntupler::Fill_GenParticle(const edm::Event &iEvent)
{
  // -- Gen-weight info -- //
  edm::Handle<GenEventInfoProduct> Handle_GenEventInfo;
  iEvent.getByToken(Token_GenEventInfo, Handle_GenEventInfo);
  this->GenEventWeight = Handle_GenEventInfo->weight();

  // -- Gen-particle info -- //
  edm::Handle<reco::GenParticleCollection> Handle_GenParticle;
  iEvent.getByToken(Token_GenParticle, Handle_GenParticle);

  int _nGenParticle = 0;
  for( size_t i=0; i< Handle_GenParticle->size(); ++i)
  {
    const reco::GenParticle &parCand = (*Handle_GenParticle)[i];

    if( abs(parCand.pdgId()) == 13 ) // -- only muons -- //
    {
      this->GenParticle_ID[_nGenParticle] = parCand.pdgId();
      this->GenParticle_Status[_nGenParticle] = parCand.status();
      this->GenParticle_Mother[_nGenParticle] = parCand.mother(0)->pdgId();

      this->GenParticle_Pt[_nGenParticle] = parCand.pt();
      this->GenParticle_Eta[_nGenParticle] = parCand.eta();
      this->GenParticle_Phi[_nGenParticle] = parCand.phi();
      this->GenParticle_Px[_nGenParticle] = parCand.px();
      this->GenParticle_Py[_nGenParticle] = parCand.py();
      this->GenParticle_Pz[_nGenParticle] = parCand.pz();
      this->GenParticle_E[_nGenParticle] = parCand.energy();
      this->GenParticle_Charge[_nGenParticle] = parCand.charge();

      if( parCand.statusFlags().isPrompt() ) this->GenParticle_isPrompt[_nGenParticle] = 1;
      if( parCand.statusFlags().isTauDecayProduct() ) this->GenParticle_isTauDecayProduct[_nGenParticle] = 1;
      if( parCand.statusFlags().isPromptTauDecayProduct() ) this->GenParticle_isPromptTauDecayProduct[_nGenParticle] = 1;
      if( parCand.statusFlags().isDecayedLeptonHadron() ) this->GenParticle_isDecayedLeptonHadron[_nGenParticle] = 1;

      if( parCand.isPromptFinalState() ) this->GenParticle_isPromptFinalState[_nGenParticle] = 1;
      if( parCand.isDirectPromptTauDecayProductFinalState() ) this->GenParticle_isDirectPromptTauDecayProductFinalState[_nGenParticle] = 1;
      if( parCand.isHardProcess() ) this->GenParticle_isHardProcess[_nGenParticle] = 1;
      if( parCand.isLastCopy() ) this->GenParticle_isLastCopy[_nGenParticle] = 1;
      if( parCand.isLastCopyBeforeFSR() ) this->GenParticle_isLastCopyBeforeFSR[_nGenParticle] = 1;

      if( parCand.isPromptDecayed() ) this->GenParticle_isPromptDecayed[_nGenParticle] = 1;
      if( parCand.fromHardProcessBeforeFSR() ) this->GenParticle_fromHardProcessBeforeFSR[_nGenParticle] = 1;
      if( parCand.fromHardProcessDecayed() ) this->GenParticle_fromHardProcessDecayed[_nGenParticle] = 1;
      if( parCand.fromHardProcessFinalState() ) this->GenParticle_fromHardProcessFinalState[_nGenParticle] = 1;
      // if( parCand.isMostlyLikePythia6Status3() ) this->GenParticle_isMostlyLikePythia6Status3[_nGenParticle] = 1;

      _nGenParticle++;
    }
  }

  this->nGenParticle = _nGenParticle;
}

void ntupler::Fill_IterL3(const edm::Event &iEvent)
{
  ////////////////////
  // -- IterL3OI -- //
  ////////////////////
  edm::Handle< std::vector<reco::MuonTrackLinks> > Handle_IterL3OI;
  if( iEvent.getByToken( Token_IterL3OI, Handle_IterL3OI ) )
  {
    int _nIterL3OI = 0;
    for( unsigned int i=0; i<Handle_IterL3OI->size(); i++)
    {
      if( Handle_IterL3OI->at(i).trackerTrack().isNonnull() ) {
        this->IterL3OI_TK_Pt[_nIterL3OI]     = Handle_IterL3OI->at(i).trackerTrack()->pt();
        this->IterL3OI_TK_Eta[_nIterL3OI]    = Handle_IterL3OI->at(i).trackerTrack()->eta();
        this->IterL3OI_TK_Phi[_nIterL3OI]    = Handle_IterL3OI->at(i).trackerTrack()->phi();
        this->IterL3OI_TK_Charge[_nIterL3OI] = Handle_IterL3OI->at(i).trackerTrack()->charge();
      }
      if( Handle_IterL3OI->at(i).standAloneTrack().isNonnull() ) {
        this->IterL3OI_SA_Pt[_nIterL3OI]     = Handle_IterL3OI->at(i).standAloneTrack()->pt();
        this->IterL3OI_SA_Eta[_nIterL3OI]    = Handle_IterL3OI->at(i).standAloneTrack()->eta();
        this->IterL3OI_SA_Phi[_nIterL3OI]    = Handle_IterL3OI->at(i).standAloneTrack()->phi();
        this->IterL3OI_SA_Charge[_nIterL3OI] = Handle_IterL3OI->at(i).standAloneTrack()->charge();
      }
      if( Handle_IterL3OI->at(i).globalTrack().isNonnull() ) {
        this->IterL3OI_GL_Pt[_nIterL3OI]     = Handle_IterL3OI->at(i).globalTrack()->pt();
        this->IterL3OI_GL_Eta[_nIterL3OI]    = Handle_IterL3OI->at(i).globalTrack()->eta();
        this->IterL3OI_GL_Phi[_nIterL3OI]    = Handle_IterL3OI->at(i).globalTrack()->phi();
        this->IterL3OI_GL_Charge[_nIterL3OI] = Handle_IterL3OI->at(i).globalTrack()->charge();
      }
      _nIterL3OI++;
    }
    this->nIterL3OI = _nIterL3OI;
  }

  /////////////////////////////
  // -- IterL3IO_L2Seeded -- //
  /////////////////////////////
  edm::Handle< std::vector<reco::MuonTrackLinks> > Handle_IterL3IO_L2Seeded;
  if( iEvent.getByToken( Token_IterL3IO_L2Seeded, Handle_IterL3IO_L2Seeded ) )
  {
    int _nIterL3IO_L2Seeded = 0;
    for( unsigned int i=0; i<Handle_IterL3IO_L2Seeded->size(); i++)
    {
      if( Handle_IterL3IO_L2Seeded->at(i).trackerTrack().isNonnull() ) {
        this->IterL3IO_L2Seeded_TK_Pt[_nIterL3IO_L2Seeded]     = Handle_IterL3IO_L2Seeded->at(i).trackerTrack()->pt();
        this->IterL3IO_L2Seeded_TK_Eta[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).trackerTrack()->eta();
        this->IterL3IO_L2Seeded_TK_Phi[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).trackerTrack()->phi();
        this->IterL3IO_L2Seeded_TK_Charge[_nIterL3IO_L2Seeded] = Handle_IterL3IO_L2Seeded->at(i).trackerTrack()->charge();
      }
      if( Handle_IterL3IO_L2Seeded->at(i).standAloneTrack().isNonnull() ) {
        this->IterL3IO_L2Seeded_SA_Pt[_nIterL3IO_L2Seeded]     = Handle_IterL3IO_L2Seeded->at(i).standAloneTrack()->pt();
        this->IterL3IO_L2Seeded_SA_Eta[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).standAloneTrack()->eta();
        this->IterL3IO_L2Seeded_SA_Phi[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).standAloneTrack()->phi();
        this->IterL3IO_L2Seeded_SA_Charge[_nIterL3IO_L2Seeded] = Handle_IterL3IO_L2Seeded->at(i).standAloneTrack()->charge();
      }
      if( Handle_IterL3IO_L2Seeded->at(i).globalTrack().isNonnull() ) {
        this->IterL3IO_L2Seeded_GL_Pt[_nIterL3IO_L2Seeded]     = Handle_IterL3IO_L2Seeded->at(i).globalTrack()->pt();
        this->IterL3IO_L2Seeded_GL_Eta[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).globalTrack()->eta();
        this->IterL3IO_L2Seeded_GL_Phi[_nIterL3IO_L2Seeded]    = Handle_IterL3IO_L2Seeded->at(i).globalTrack()->phi();
        this->IterL3IO_L2Seeded_GL_Charge[_nIterL3IO_L2Seeded] = Handle_IterL3IO_L2Seeded->at(i).globalTrack()->charge();
      }
      _nIterL3IO_L2Seeded++;
    }
    this->nIterL3IO_L2Seeded = _nIterL3IO_L2Seeded;
  }

  ///////////////////////////
  // -- IterL3IO_FromL1 -- //
  ///////////////////////////
  edm::Handle< std::vector<reco::Track> > Handle_IterL3IO_FromL1;
  if( iEvent.getByToken( Token_IterL3IO_FromL1, Handle_IterL3IO_FromL1 ) )
  {
    int _nIterL3IO_FromL1 = 0;
    for( unsigned int i=0; i<Handle_IterL3IO_FromL1->size(); i++)
    {
      this->IterL3IO_FromL1_Pt[_nIterL3IO_FromL1]     = Handle_IterL3IO_FromL1->at(i).pt();
      this->IterL3IO_FromL1_Eta[_nIterL3IO_FromL1]    = Handle_IterL3IO_FromL1->at(i).eta();
      this->IterL3IO_FromL1_Phi[_nIterL3IO_FromL1]    = Handle_IterL3IO_FromL1->at(i).phi();
      this->IterL3IO_FromL1_Charge[_nIterL3IO_FromL1] = Handle_IterL3IO_FromL1->at(i).charge();
      _nIterL3IO_FromL1++;
    }
    this->nIterL3IO_FromL1 = _nIterL3IO_FromL1;
  }

  /////////////////////////
  // -- IterL3_FromL2 -- //
  /////////////////////////
  edm::Handle< std::vector<reco::MuonTrackLinks> > Handle_IterL3_FromL2;
  if( iEvent.getByToken( Token_IterL3_FromL2, Handle_IterL3_FromL2 ) )
  {
    int _nIterL3_FromL2 = 0;
    for( unsigned int i=0; i<Handle_IterL3_FromL2->size(); i++)
    {
      if( Handle_IterL3_FromL2->at(i).trackerTrack().isNonnull() ) {
        this->IterL3_FromL2_TK_Pt[_nIterL3_FromL2]     = Handle_IterL3_FromL2->at(i).trackerTrack()->pt();
        this->IterL3_FromL2_TK_Eta[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).trackerTrack()->eta();
        this->IterL3_FromL2_TK_Phi[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).trackerTrack()->phi();
        this->IterL3_FromL2_TK_Charge[_nIterL3_FromL2] = Handle_IterL3_FromL2->at(i).trackerTrack()->charge();
      }
      if( Handle_IterL3_FromL2->at(i).standAloneTrack().isNonnull() ) {
        this->IterL3_FromL2_SA_Pt[_nIterL3_FromL2]     = Handle_IterL3_FromL2->at(i).standAloneTrack()->pt();
        this->IterL3_FromL2_SA_Eta[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).standAloneTrack()->eta();
        this->IterL3_FromL2_SA_Phi[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).standAloneTrack()->phi();
        this->IterL3_FromL2_SA_Charge[_nIterL3_FromL2] = Handle_IterL3_FromL2->at(i).standAloneTrack()->charge();
      }
      if( Handle_IterL3_FromL2->at(i).globalTrack().isNonnull() ) {
        this->IterL3_FromL2_GL_Pt[_nIterL3_FromL2]     = Handle_IterL3_FromL2->at(i).globalTrack()->pt();
        this->IterL3_FromL2_GL_Eta[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).globalTrack()->eta();
        this->IterL3_FromL2_GL_Phi[_nIterL3_FromL2]    = Handle_IterL3_FromL2->at(i).globalTrack()->phi();
        this->IterL3_FromL2_GL_Charge[_nIterL3_FromL2] = Handle_IterL3_FromL2->at(i).globalTrack()->charge();
      }
      _nIterL3_FromL2++;
    }
    this->nIterL3_FromL2 = _nIterL3_FromL2;
  }

}

void ntupler::endJob() {}
void ntupler::beginRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {}
void ntupler::endRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {}

DEFINE_FWK_MODULE(ntupler);
