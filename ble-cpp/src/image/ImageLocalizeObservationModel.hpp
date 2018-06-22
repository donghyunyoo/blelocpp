/*******************************************************************************
 * Copyright (c) 2018  IBM Corporation, Carnegie Mellon University and others
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

#ifndef ImageLocalizeObservationModel_hpp
#define ImageLocalizeObservationModel_hpp

#include <stdio.h>
#include "bleloc.h"
#include "ObservationModel.hpp"

namespace loc{
    
    template<class Tstate, class Tinput>
    class ImageLocalizeObservationModel;
    
    template<class Tstate, class Tinput>
    class ImageLocalizeObservationModel : public ObservationModel<Tstate, Tinput>{
        
    private:
        double mSigmaDistImageLikelihood = 5.0;
        double mSigmaAngleImageLikelihood = 30.0;
        
    public:
        ImageLocalizeObservationModel() = default;
        ~ImageLocalizeObservationModel() = default;
        
        std::function<double(double, double, double)> normFunc = MathUtils::logProbaNormal;
        
        std::vector<Tstate>* update(const std::vector<Tstate> & states, const Tinput & input) override {
            std::cout << "ImageLocalizeObservationModel::update is not supported." << std::endl;
            std::vector<Tstate>* statesCopy = new std::vector<Tstate>(states);
            return statesCopy;
        }
        
        double computeLogLikelihood(const Tstate& state, const Tinput& input);
        std::vector<double> computeLogLikelihood(const std::vector<Tstate> & states, const Tinput& input) override;
        
        std::vector<double> computeLogLikelihoodRelatedValues(const Tstate& state, const Tinput& input);
        std::vector<std::vector<double>> computeLogLikelihoodRelatedValues(const std::vector<Tstate> & states, const Tinput& input) override;
        
        ImageLocalizeObservationModel& sigmaDistImageLikelihood(double dist);
        double sigmaDistImageLikelihood() const;

        ImageLocalizeObservationModel& sigmaAngleImageLikelihood(double angle);
        double sigmaAngleImageLikelihood() const;
    };
    
}

#endif /* ImageLocalizeObservationModel_hpp */
