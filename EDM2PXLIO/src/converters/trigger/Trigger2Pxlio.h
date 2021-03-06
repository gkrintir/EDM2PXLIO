#ifndef _TRIGGER2PXLIO_H_
#define _TRIGGER2PXLIO_H_

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Common/interface/TriggerNames.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "boost/regex.hpp"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class Trigger2Pxlio: public Collection2Pxlio<edm::TriggerResults>
{
    protected:
        std::vector<boost::regex> regexList_;

    public:
        Trigger2Pxlio(std::string name):
            Collection2Pxlio<edm::TriggerResults>(name)
        {
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<Trigger2Pxlio>("Trigger2Pxlio"));
        }
        
        void parseParameter(const edm::ParameterSet& globalConfig)
        {
            if (globalConfig.exists(name_))
        	{
        	    const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(name_);   
        	
                if (iConfig.exists("regex")) 
                {
                    std::vector<std::string> list = iConfig.getParameter<std::vector<std::string>>("regex");
                    for (unsigned iregex=0; iregex<list.size(); ++iregex)
                    {
                        regexList_.push_back(boost::regex(list[iregex]));
                    }
                }
                if (iConfig.exists("targetEventViews"))
                {
                    eventViewNames_= iConfig.getParameter<std::vector<std::string> >("targetEventViews");
                    if (eventViewNames_.size()!=srcs_.size() && eventViewNames_.size()>0)
                    {
                        edm::LogInfo(name_) << "will use the same eventviewname for all sources:"<<eventViewNames_[0];    
                    }
                }
            }
            else
            {
            }
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            Collection2Pxlio<edm::TriggerResults>::convert(edmEvent, iSetup, pxlEvent);
        
            edm::Handle<edm::TriggerResults> trigResults;
            edm::InputTag trigResultsTag("TriggerResults","","HLT");
            edmEvent->getByLabel(trigResultsTag,trigResults);
            //fix dirty hack for the name
            pxl::EventView* pxlEventView = Collection2Pxlio<edm::TriggerResults>::findEventView(pxlEvent,"Reconstructed");
            const edm::TriggerNames& trigNames = edmEvent->triggerNames(*trigResults);
            for (unsigned inames=0; inames<trigNames.size(); ++inames) 
            {
                const std::string pathName=trigNames.triggerName(inames);
                bool accept = false;
                for (unsigned iregex=0; (iregex<regexList_.size()) && !accept; ++iregex)
                {
                    accept = accept || regex_search(pathName,regexList_[iregex]);
                    if (accept)
                    {
                        break;
                    }
                }
                if (accept)
                {
                    bool passTrig=trigResults->accept(trigNames.triggerIndex(pathName));
                    pxlEventView->setUserRecord(pathName,passTrig);
                }
            }
        }
        
        ~Trigger2Pxlio()
        {
        }
};

#endif

