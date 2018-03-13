/*******************************************************************************
 * Copyright (c) 2014-2016  IBM Corporation and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*******************************************************************************/

#include "MetropolisSampler.hpp"

namespace loc{
    
    // Implementation
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::input(const Tinput& input){
        mInput = input;
        initialize();
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::observationModel(std::shared_ptr<ObservationModel<Tstate, Tinput>> obsModel){
        mObsModel = obsModel;
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::statusInitializer(std::shared_ptr<StatusInitializerImpl> statusInitializer){
        mStatusInitializer = statusInitializer;
    }
    
    template <class Tstate, class Tinput>
    State MetropolisSampler<Tstate, Tinput>::findInitialMaxLikelihoodState(){
        Locations locations;
        
        if (mParams.initType == INIT_WITH_SAMPLE_LOCATIONS) {
            locations = mStatusInitializer->extractLocationsCloseToBeacons(mInput, mParams.radius2D);
        } else if (mParams.initType == INIT_WITH_BEACON_LOCATIONS){
            locations = mStatusInitializer->generateLocationsCloseToBeaconsWithPerturbation(mInput, mParams.radius2D);
        }
        if(locations.size()==0){
            BOOST_THROW_EXCEPTION(LocException("No location close to beacons was found in sampler."));
        }
        locations = Location::filterLocationsOnFlatFloor(locations); // Remove locations with an unusual z value.
        locations = mStatusInitializer->extractMovableLocations(locations);
        if(locations.size()==0){
            std::cerr << "All locations were removed at filtering step in sampler.　Not filtered locations are used." << std::endl;
            if (mParams.initType == INIT_WITH_SAMPLE_LOCATIONS) {
                locations = mStatusInitializer->extractLocationsCloseToBeacons(mInput, mParams.radius2D);
            } else if (mParams.initType == INIT_WITH_BEACON_LOCATIONS){
                locations = mStatusInitializer->generateLocationsCloseToBeaconsWithPerturbation(mInput, mParams.radius2D);
            }

        }
        auto states = mStatusInitializer->initializeStatesFromLocations(locations);
        std::vector<double> logLLs = mObsModel->computeLogLikelihood(states, mInput);
        auto iterMax = std::max_element(logLLs.begin(), logLLs.end());
        size_t index = std::distance(logLLs.begin(), iterMax);
        Tstate locMaxLL = states.at(index);
        
        if(isVerbose){
            std::cout << "findInitialMaxLikelihoodState: states.size=" << states.size() << "max state=" << locMaxLL << std::endl;
        }
        
        return locMaxLL;
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::initialize(){
        clear();
        currentState = findInitialMaxLikelihoodState();
        std::vector<Tstate> ss = {currentState};
        currentLogLL = mObsModel->computeLogLikelihood(ss, mInput).at(0);
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::prepare(){
        startBurnIn();
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::startBurnIn(int burnIn){
        for(int i=0; i<burnIn; i++){
            sample();
        }
        isBurnInFinished = true;
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::startBurnIn(){
        startBurnIn(mParams.burnIn);
    }
    
    template <class Tstate, class Tinput>
    std::vector<Tstate> MetropolisSampler<Tstate, Tinput>::getAllStates() const{
        return allStates;
    }
    
    template <class Tstate, class Tinput>
    std::vector<double> MetropolisSampler<Tstate, Tinput>::getAllLogLLs() const{
        return allLogLLs;
    }
    
    template <class Tstate, class Tinput>
    bool MetropolisSampler<Tstate, Tinput>::sample(){
        return sample(true, true);
    }
    
    // This function returns the result whether a proposed sample was accepted or not.
    template <class Tstate, class Tinput>
    bool MetropolisSampler<Tstate, Tinput>::sample(bool transitLoc, bool transitRssiBias){
        Tstate stateNew = transitState(currentState, transitLoc, transitRssiBias);
        
        std::vector<Tstate> ss = {stateNew};
        double logLLNew = mObsModel->computeLogLikelihood(ss, mInput).at(0);
        
        double r = std::exp(logLLNew-currentLogLL); // p(xnew)/p(x) = exp(log(p(xnew))-log(p(xpre)))
        
        bool isAccepted = false;
        if(randGen.nextDouble() < r){
            currentState = stateNew;
            currentLogLL = logLLNew;
            isAccepted = true;
        }
        // Save current state
        allStates.push_back(Tstate(currentState));
        allLogLLs.push_back(currentLogLL);
        
        return isAccepted;
    }
    
    template <class Tstate, class Tinput>
    std::vector<Tstate> MetropolisSampler<Tstate, Tinput>::sampling(int n){
        return sampling(n, mParams.withOrdering);
        /*
        std::vector<Tstate> sampledStates;
        int count = 0;
        int countAccepted = 0;
        for(int i=1; ;i++){
            bool isAccepted = sample();
            count++;
            if(isAccepted){
                countAccepted++;
            }
            if(i%mParams.interval==0){
                sampledStates.push_back(Tstate(currentState));
            }
            if(sampledStates.size()>=n){
                break;
            }
        }
        
        std::cout << "M-H acceptance rate = " << (double)countAccepted / (double) count << " (" << countAccepted << "/" << count << ")" << std::endl;
        
        return sampledStates;
        */
    }
    
    template <class Tstate, class Tinput>
    std::vector<Tstate> MetropolisSampler<Tstate, Tinput>::sampling(int n, bool withOrderging){
        
        if(! isBurnInFinished){
            this->startBurnIn();
        }
        
        std::vector<Tstate> sampledStates;
        int count = 0;
        int countAccepted = 0;
        for(int i=1; ;i++){
            if(n==0){
                break;
            }
            bool isAccepted = sample();
            count++;
            if(isAccepted){
                countAccepted++;
            }
            if(i%mParams.interval==0){
                sampledStates.push_back(Tstate(currentState));
            }
            if(sampledStates.size()>=n){
                break;
            }
        }
        
        if(isVerbose){
            std::cout << "M-H acceptance rate = " << (double)countAccepted / (double) count << " (" << countAccepted << "/" << count << ")" << std::endl;
        }
        
        if(! withOrderging){
            return sampledStates;
        }else{
            sampledStates = std::vector<State>();
            std::vector<int> indices(allStates.size());
            // create indices = {1,2,3,...,n};
            std::iota(indices.begin(), indices.end(), 0);
    
            std::sort(indices.begin(), indices.end(),[&](int a, int b){
                return allLogLLs.at(a) < allLogLLs.at(b);
            });
            
            double logLLMemo = std::numeric_limits<double>::lowest();
            for(int i=0; ;i++){
                //std::cout << "indices.size() = " << indices.size() << std::endl;
                int index = indices.at(indices.size()-1-i);
                double logLL = allLogLLs.at(index);
//                if(logLLMemo!=logLL)
                {
                    logLLMemo = logLL;
                    sampledStates.push_back(allStates.at(index));
                    //std::cout << "state=" << allStates.at(index) << ", logLL=" << logLLMemo << std::endl;
                }
                if(sampledStates.size()>=n){
                    break;
                }
            }
        }
        return sampledStates;
    }
    
    template <class Tstate, class Tinput>
    std::vector<Tstate> MetropolisSampler<Tstate, Tinput>::sampling(int n, const Location &location){
        Locations locs = {location};
        auto states = mStatusInitializer->initializeStatesFromLocations(locs);
        std::vector<Tstate> ss = {states};
        currentState = ss.at(0);
        currentLogLL = mObsModel->computeLogLikelihood(ss, mInput).at(0);
        
        std::vector<Tstate> sampledStates;
        int count = 0;
        int countAccepted = 0;
        for(int i=1; ;i++){
            if(n==0){
                break;
            }
            bool isAccepted = sample(false, true);
            count++;
            if(isAccepted){
                countAccepted++;
            }
            if(i%mParams.interval==0){
                sampledStates.push_back(Tstate(currentState));
            }
            if(sampledStates.size()>=n){
                break;
            }
        }
        
        std::cout << "M-H acceptance rate = " << (double)countAccepted / (double) count << " (" << countAccepted << "/" << count << ")" << std::endl;
        
        return sampledStates;
    }
    
    template <class Tstate, class Tinput>
    State MetropolisSampler<Tstate, Tinput>::transitState(Tstate state) {
        return transitState(state, true, true);
    }
    
    template <class Tstate, class Tinput>
    State MetropolisSampler<Tstate, Tinput>::transitState(Tstate state, bool transitLoc, bool transitRssiBias){
        // update variables that affect mainly likelihood
        Tstate stateNew(state);
        if (transitLoc) {
            auto locNew = mStatusInitializer->perturbLocation(stateNew);
            stateNew.x(locNew.x());
            stateNew.y(locNew.y());
        }
        if (transitRssiBias) {
            stateNew = mStatusInitializer->perturbRssiBias(stateNew);
        }
        
        return stateNew;
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::clear(){
        allStates.clear();
        allLogLLs.clear();
    }
    
    template <class Tstate, class Tinput>
    void MetropolisSampler<Tstate, Tinput>::print() const{
        double averageLogLL = std::accumulate(allLogLLs.begin(), allLogLLs.end(), 0.0)/(allLogLLs.size());
        std::cout << "allStates.size()=" << allStates.size()
                << ",allLogLLs.size()=" << allLogLLs.size()
                << ",averageLogLL=" << averageLogLL << std::endl;
    }
    
    // explicit instantiation
    template class MetropolisSampler<State, Beacons>;
    
}
