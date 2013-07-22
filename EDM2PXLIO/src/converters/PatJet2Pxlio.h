#ifndef _PATJET2PXLIO_H_
#define _PATJET2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

#include "CMGTools/External/interface/PileupJetIdentifier.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/PuJetIdProvider.h"

class PatJet2Pxlio: public Pat2Pxlio<pat::Jet>
{
	protected:
		PuJetIdProvider* puJetIdProvider_;

    public:
        PatJet2Pxlio(std::string name):
            Pat2Pxlio<pat::Jet>(name),
            puJetIdProvider_(0)
        {
        	puJetIdProvider_=createProvider<PuJetIdProvider>();
        }
                
        virtual void convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setUserRecord<int>("numberOfDaughters",patObject.numberOfDaughters());
            pxlParticle->setUserRecord<int>("electronMultiplicity",patObject.electronMultiplicity());
            pxlParticle->setUserRecord<int>("muonMultiplicity",patObject.muonMultiplicity());
            pxlParticle->setUserRecord<float>("trackCountingHighPurBJetTags",patObject.bDiscriminator("trackCountingHighPurBJetTags"));
            pxlParticle->setUserRecord<float>("trackCountingHighEffBJetTags",patObject.bDiscriminator("trackCountingHighEffBJetTags"));
            pxlParticle->setUserRecord<float>("combinedSecondaryVertexBJetTags",patObject.bDiscriminator("combinedSecondaryVertexBJetTags"));
            pxlParticle->setUserRecord<float>("jetBProbabilityBJetTags",patObject.bDiscriminator("jetBProbabilityBJetTags"));
            pxlParticle->setUserRecord<float>("jetProbabilityBJetTags",patObject.bDiscriminator("jetProbabilityBJetTags"));
            
            
            float nhf =  ( patObject.neutralHadronEnergy() + patObject.HFHadronEnergy() ) /patObject.energy();
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",nhf);
	        
	        pxlParticle->setUserRecord<float>("neutralEmEnergyFraction",patObject.neutralEmEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedEmEnergyFraction",patObject.chargedEmEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedHadronEnergyFraction",patObject.chargedHadronEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedMultiplicity",patObject.chargedMultiplicity());
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());

            
            
            const reco::SecondaryVertexTagInfo *svTagInfo = patObject.tagInfoSecondaryVertex();
            if (svTagInfo  &&  svTagInfo->nVertices() > 0)
            {
                pxlParticle->setUserRecord<float>("secondaryVertexMass",svTagInfo->secondaryVertex(0).p4().mass());
            }
            if (patObject.genParton()) 
            {
                pxlParticle->setUserRecord<int>("partonFlavour",patObject.partonFlavour());
            }

        }
        
        virtual void convertCollection(const edm::Handle<edm::View<pat::Jet>> patObjectList, std::vector<pxl::Particle*> pxlParticleList)
        {
        	if (puJetIdProvider_->getPuJetIds())
        	{
        		if (puJetIdProvider_->getPuJetIds()->size()!=patObjectList->size())
        		{
        			throw cms::Exception("PatJet2Pxlio::convertCollection") << "jet's pu id value map differs in size compared to provided pat jets";
        		}
				for (unsigned ijet=0; ijet<patObjectList->size(); ++ijet)
				{
					StoredPileupJetIdentifier puid = (*puJetIdProvider_->getPuJetIds())[patObjectList->refAt(ijet)];
					pxlParticleList[ijet]->setUserRecord<float>("puRMS",puid.RMS());
				}
        	}

        }
        
        ~PatJet2Pxlio()
        {
        }
};

#endif
