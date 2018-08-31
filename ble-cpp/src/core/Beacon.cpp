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

#include "Beacon.hpp"
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

namespace loc{
    
    Beacon::Beacon(int major, int minor, double rssi){
        major_ = major;
        minor_ = minor;
        rssi_ = rssi;
    }
    
    Beacon::Beacon(std::string uuid, int major, int minor, double rssi){
        uuid_ = uuid;
        major_ = major;
        minor_ = minor;
        rssi_ = rssi;
    }
    
    Beacon::~Beacon(){}

    std::string Beacon::uuid() const{
        return uuid_;
    }

    Beacon& Beacon::uuid(std::string uuid){
        this->uuid_ = uuid;
        return *this;
    }

    int Beacon::major() const{
        return major_;
    }
    
    Beacon& Beacon::major(int major){
        this->major_ = major;
        return *this;
    }
    
    int Beacon::minor() const{
        return minor_;
    }
    
    Beacon& Beacon::minor(int minor){
        this->minor_ = minor;
        return *this;
    }
    
    double Beacon::rssi() const{
        return rssi_;
    }
    
    Beacon& Beacon::rssi(double rssi){
        this->rssi_ = rssi;
        return *this;
    }
    
    Beacon::Id Beacon::id() const{
        return Beacon::Id(uuid_,major_,minor_);
    }
    
    std::string Beacon::toString(){
        std::stringstream stream;
        stream << major_ << "," << minor_ << "," << rssi_;
        return stream.str();
    }
    
    template<class Tbeacons>
    Tbeacons Beacon::meanBeaconsVector(std::vector<Tbeacons> beaconsVector){
        std::map<Beacon::Id, int> id_count;
        std::map<Beacon::Id, double> id_rssiSum;
        
        for(int i=0; i<beaconsVector.size(); i++){
            Tbeacons beacons = beaconsVector.at(i);
            for(int j=0; j<beacons.size(); j++){
                Beacon b = beacons.at(j);
                Beacon::Id id(b.uuid(), b.major(), b.minor());
                if(id_count.count(id)==0){
                    id_count[id]=1;
                    id_rssiSum[id] = b.rssi();
                }else{
                    id_count[id]+=1;
                    id_rssiSum[id] += b.rssi();
                }
            }
        }
        
        Tbeacons beaconsAveraged;
        for(auto iter=id_count.begin(); iter!=id_count.end(); iter++){
            auto id = iter->first;
            
            auto uuid = id.uuid();
            int major = id.major();
            int minor = id.minor();
            
            double rssi = id_rssiSum[id]/iter->second;
            Beacon b(uuid, major,minor,rssi);
            beaconsAveraged.push_back(b);
        }
        return beaconsAveraged;
        
    }
    
    class Beacon::Impl{
    public:
        static const long large_value = 100000;
    };
    
    Beacons Beacon::filter(const Beacons& beacons, double minRssi, double maxRssi){
        Beacons beaconsFiltered(beacons);
        beaconsFiltered.clear();
        for(Beacon b: beacons){
            double rssi = b.rssi();
            if( minRssi<rssi && rssi<maxRssi){
                beaconsFiltered.push_back(b);
            }
        }
        return beaconsFiltered;
    }
    
    struct LessRSSI{
        bool operator()(const Beacon& b1, const Beacon& b2){
            return b1.rssi() < b2.rssi();
        }
    };
    
    Beacons Beacon::sortByRssi(const Beacons& beacons){
        Beacons beaconsSorted(beacons);
        std::sort(beaconsSorted.begin(), beaconsSorted.end(), LessRSSI());
        return beaconsSorted;
    }
    
    std::ostream& operator<<(std::ostream&os, const Beacon& beacon){
        os << beacon.major() <<"," << beacon.minor() <<"," << beacon.rssi();
        return os;
    }
    
    template
    Beacons Beacon::meanBeaconsVector<Beacons>(std::vector<Beacons> beaconsVector);
    
    template
    std::vector<Beacon> Beacon::meanBeaconsVector<std::vector<Beacon>>(std::vector<std::vector<Beacon>> beaconsVector);
    
    
    // Implementation of Beacons class
    
    Beacons& Beacons::timestamp(long timestamp){
        mTimestamp = timestamp;
        return *this;
    }
    
    long Beacons::timestamp() const{
        return mTimestamp;
    }
    

    
    // Beacon Id class
    
    Beacon::Id::Id(const std::string& uuid, int major, int minor){
        uuid_ = uuid;
        std::transform(uuid_.begin(), uuid_.end(), uuid_.begin(), ::tolower);
        major_ = major;
        minor_ = minor;
    }
    
    std::string Beacon::Id::uuid() const{
        return uuid_;
    }
    
    int Beacon::Id::major() const{
        return major_;
    }
    
    int Beacon::Id::minor() const{
        return minor_;
    }
    
    bool Beacon::Id::operator==(const Id& id) const{
        if(this->uuid_.length()==0 || id.uuid_.length()==0){
            return this->major_ == id.major_ && this->minor_ == id.minor_;
        }else{
            return this->major_ == id.major_ && this->minor_ == id.minor_ && this->uuid_ == id.uuid_;
        }
    }
    
    bool Beacon::Id::operator<(const Id& id) const{
        if(this->uuid_.length()==0 || id.uuid_.length()==0){
            if (this->major_ == id.major_){
                return this->minor_ < id.minor_;
            }else{
                return this->major_ < id.major_;
            }
        }else{
            if( this->uuid_ == id.uuid_ ){
                if (this->major_ == id.major_){
                    return this->minor_ < id.minor_;
                }else{
                    return this->major_ < id.major_;
                }
            }else{
                return this->uuid_ < id.uuid_;
            }
        }
    }
        
    Beacon::Id Beacon::Id::convertLongIdToId(long long_id){
        int major = convertIdToMajor(long_id);
        int minor = convertIdToMinor(long_id);
        Id id;
        id.major_ = major;
        id.minor_ = minor;
        return id;
    }
    
    // Encoding rule
    // id = large_value*major + minor
    // major = id/large_value;
    // minor = id%large_value;
    
    /*
     Beacon::Id Beacon::convertMajorMinorToId(int major, int minor){
     long large_value = Impl::large_value;
     long id = major*large_value + minor;
     return id;
     }
     */
    
    int Beacon::Id::convertIdToMajor(long id){
        long large_value = Impl::large_value;
        int major = static_cast<int>(id/large_value);
        return major;
    }
    
    int Beacon::Id::convertIdToMinor(long id){
        long large_value = Impl::large_value;
        int minor = static_cast<int>(id%large_value);
        return minor;
    }
    
    
}
