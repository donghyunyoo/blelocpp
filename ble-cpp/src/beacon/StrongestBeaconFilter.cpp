/*******************************************************************************
 * Copyright (c) 2014, 2015  IBM Corporation and others
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

#include "StrongestBeaconFilter.hpp"

namespace loc{
    
    StrongestBeaconFilter::StrongestBeaconFilter(int nStrongest){
        nStrongest_ = nStrongest;
    }
    
    Beacons StrongestBeaconFilter::filter(const Beacons& beacons) const{
        Beacons beaconsSorted = Beacon::sortByRssi(beacons);
        Beacons beaconsFiltered(beaconsSorted);
        beaconsFiltered.clear();
        int n = static_cast<int>( beaconsSorted.size() );
        
        int nFiltered = std::min(n, nStrongest_);
        for(int i=0; i<nFiltered; i++){
            int idx = n-i-1;
            if(beaconsSorted.at(idx).rssi() > cutoffRssi_){
                beaconsFiltered.push_back(beaconsSorted.at(idx));
            }
        }
        return beaconsFiltered;
    }
    
    StrongestBeaconFilter& StrongestBeaconFilter::nStrongest(int nStrongest){
        nStrongest_ = nStrongest;
        return *this;
    }
    
    StrongestBeaconFilter& StrongestBeaconFilter::cutoffRssi(double cutoffRssi){
        cutoffRssi_ = cutoffRssi;
        return *this;
    }
}
