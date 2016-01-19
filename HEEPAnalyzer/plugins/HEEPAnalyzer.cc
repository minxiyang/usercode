#include "SHarper/HEEPAnalyzer/interface/HEEPAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"


//delete these
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPDebug.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h" 

HEEPAnalyzer::HEEPAnalyzer(const edm::ParameterSet& iPara):
  evtHelper_(iPara,consumesCollector(),*this),heepEvt_(),nrPass_(0),nrFail_(0)
{

}

void HEEPAnalyzer::beginJob()
{
  edm::Service<TFileService> fs;
  massHist_ = fs->make<TH1D>("massHist","Di-Electron Mass;M_{ee} (GeV/c^{2});# Events / 10 GeV/c^{2}",250,50,2550); 
}

void HEEPAnalyzer::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //make the heep event (see easy isnt it)
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);

  //do what ever you want
  //lets get the heep electrons and count the number that pass / fail cuts
  const std::vector<heep::Ele>& eles = heepEvt_.heepEles();
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){     
    // std::cout <<"eleNr "<<eleNr<< "pass "<<<<" passVID "<<passVid<<std::endl;
    if(eles[eleNr].cutCode()==0x0) nrPass_++;
    else nrFail_++;
  }


  //lets fill a mass hist
  for(size_t ele1Nr=0;ele1Nr<eles.size();ele1Nr++){
    for(size_t ele2Nr=ele1Nr+1;ele2Nr<eles.size();ele2Nr++){
      const heep::Ele& ele1 = eles[ele1Nr];
      const heep::Ele& ele2 = eles[ele2Nr];  

      
      if(ele1.cutCode()==0x0 && ele2.cutCode()==0x0 && !(ele1.isEE() && ele2.isEE())){ //EB-EB, EE-EE
	float mass = (ele1.p4()+ele2.p4()).mag();
	massHist_->Fill(mass);
      }
    }
  }
  
}

//function cross-checks that code that should give identical results does give identical results
//void HEEPAnalyzer::validate()
//{
  


//}

void HEEPAnalyzer::endJob()
{
  edm::LogInfo("HEEPAnalyzer") <<"Nr pass "<<nrPass_<<" nr fail "<<nrFail_<<" eff "<<static_cast<float>(nrPass_)/static_cast<float>(nrFail_+nrPass_);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HEEPAnalyzer);
