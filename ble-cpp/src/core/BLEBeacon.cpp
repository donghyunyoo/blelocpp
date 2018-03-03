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

#include "BLEBeacon.hpp"

#include "SerializeUtils.hpp"

namespace loc{
    
    template<class Archive>
    void BLEBeacon::serialize(Archive& ar){
        ar(CEREAL_NVP(x_));
        ar(CEREAL_NVP(y_));
        ar(CEREAL_NVP(z_));
        ar(CEREAL_NVP(floor_));
        ar(CEREAL_NVP(uuid_));
        ar(CEREAL_NVP(major_));
        ar(CEREAL_NVP(minor_));
    }
    
    template void BLEBeacon::serialize<cereal::JSONInputArchive> (cereal::JSONInputArchive& archive);
    template void BLEBeacon::serialize<cereal::JSONOutputArchive> (cereal::JSONOutputArchive& archive);
    template void BLEBeacon::serialize<cereal::PortableBinaryInputArchive> (cereal::PortableBinaryInputArchive& archive);
    template void BLEBeacon::serialize<cereal::PortableBinaryOutputArchive> (cereal::PortableBinaryOutputArchive& archive);
    
    std::ostream& operator<<(std::ostream& os, const BLEBeacon& bleBeacon){
        os << bleBeacon.x() <<"," << bleBeacon.y() <<","<< bleBeacon.z()<<","<<bleBeacon.floor() << "," <<bleBeacon.major() <<","<< bleBeacon.minor();
        return os;
    }
    
    Beacons BLEBeacon::filter(const Beacons& beacons, const BLEBeacons& blebeacons){
        std::set<long> idset;
        for(const auto& reg: blebeacons){
            idset.insert(reg.id());
        }
        Beacons os(beacons);
        os.clear();
        assert(os.size()==0);
        for(const auto&obs: beacons){
            if(idset.count(obs.id())!=0){
                os.push_back(obs);
            }
        }
        return os;
    }
    
}
