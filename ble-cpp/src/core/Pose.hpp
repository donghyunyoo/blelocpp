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

#ifndef Pose_hpp
#define Pose_hpp

#include <stdio.h>
#include <vector>
#include <cmath>
#include "Location.hpp"
#include "MathUtils.hpp"

namespace loc {
    
    class Pose;
    using Poses = std::vector<Pose> ;
    
    
    class Pose : public Location{
        
    private:
        //Location mLocation;
        double mOrientation = 0;
        double mVelocity = 0;
        double mNormalVelocity = 0;
        
    public:
        using Ptr = std::shared_ptr<Pose>;
        
        Pose() = default;
        ~Pose() = default;
        
        //Pose(const Pose& pose) = default;
        Pose(const Location& location) : Location(location){}
        
        double x() const;
        double y() const;
        double floor() const;
        double z() const;
         
        double orientation() const;
        double velocity() const;
        double normalVelocity() const;
        
        double vx() const;
        double vy() const;
        
        //Pose* location(Location);
        
        Pose& x(double);
        Pose& y(double);
        Pose& z(double);
        Pose& floor(double);
        
        Pose& orientation(double);
        Pose& velocity(double);
        Pose& normalVelocity(double);
        
        template<class Tpose>
        static Pose mean(const std::vector<Tpose>& poses);
        template<class Tpose>
        static Pose weightedMean(const std::vector<Tpose>& poses, const std::vector<double>& weights);
        template<class Tpose>
        static DirectionalStatistics computeDirectionalStatistics(const std::vector<Tpose>& poses);
        template<class Tpose>
        static WrappedNormalParameter computeWrappedNormalParameter(const std::vector<Tpose>& poses);
        
        static double normalizeOrientaion(double orientation);
        static double computeOrientationDifference(double o1, double o2);
        
        // for string stream
        static std::string header();
        friend std::ostream& operator<<(std::ostream&os, const Pose& pose);
    };
    
    // Template functions
    template<class Tpose>
    Pose Pose::mean(const std::vector<Tpose>& poses){
        size_t n = poses.size();
        double w = 1.0/n;
        std::vector<double> weights(n, w);
        return weightedMean(poses, weights);
    }
    
    template<class Tpose>
    Pose Pose::weightedMean(const std::vector<Tpose>& poses, const std::vector<double>& weights){
        size_t n = poses.size();
        double xm = 0, ym = 0, zm = 0, floorm = 0;
        double vxm = 0, vym = 0;
        double vxrepm = 0, vyrepm = 0;
        
        //for(Tpose pose: poses){
        double weightSum = 0;
        for(int i=0; i<n; i++){
            weightSum += weights.at(i);
        }
        
        for(int i=0; i<n; i++){
            const Tpose& pose = poses.at(i);
            double w = weights.at(i)/weightSum;
            xm += w * pose.x();
            ym += w * pose.y();
            zm += w * pose.z();
            floorm += w * pose.floor();
            vxm += w * pose.velocity()*std::cos(pose.orientation());
            vym += w * pose.velocity()*std::sin(pose.orientation());
            vxrepm += w * pose.normalVelocity()*cos(pose.orientation());
            vyrepm += w * pose.normalVelocity()*sin(pose.orientation());
        }
        double vm = std::sqrt(vxm*vxm + vym*vym);
        double vrepm = std::sqrt(vxrepm*vxrepm + vyrepm*vyrepm);
        
        double orientationm = atan2(vyrepm, vxrepm); // orientation must be calculated by representative velocity.
        
        Pose poseMean;
        poseMean.x(xm).y(ym).z(zm).floor(floorm);
        poseMean.orientation(orientationm).velocity(vm).normalVelocity(vrepm);
        
        return poseMean;
    }
    
    template <class Tpose>
    DirectionalStatistics Pose::computeDirectionalStatistics(const std::vector<Tpose>& poses){
        std::vector<double> oris;
        for(const auto& p: poses){
            oris.push_back(p.orientation());
        }
        return MathUtils::computeDirectionalStatistics(oris);
    }
    
    template <class Tpose>
    WrappedNormalParameter Pose::computeWrappedNormalParameter(const std::vector<Tpose>& poses){
        std::vector<double> oris;
        for(const auto& p: poses){
            oris.push_back(p.orientation());
        }
        return MathUtils::computeWrappedNormalParameters(oris);
    }
    
    // Pose property //
    
    class PoseProperty: public LocationProperty{
        
        double meanVelocity_ = 1.0; // [m/s]
        double stdVelocity_ = 0.25; // [m/s]
        double diffusionVelocity_ = 0.05; // [m/s/s]
        double minVelocity_ = 0.1; // [m/s]
        double maxVelocity_ = 1.5; // [m/s]
        
        double stdOrientation_ = 3.0/180*M_PI; // [radian/s]
        
    public:
        using Ptr = std::shared_ptr<PoseProperty>;
        
        PoseProperty& meanVelocity(double meanVelocity);
        double meanVelocity() const;
        PoseProperty& stdVelocity(double stdVelocity);
        double stdVelocity() const;
        PoseProperty& diffusionVelocity(double diffusionVelocity);
        double diffusionVelocity() const;
        PoseProperty& minVelocity(double minVelocity);
        double minVelocity() const;
        PoseProperty& maxVelocity(double maxVelocity);
        double maxVelocity() const;
        PoseProperty& stdOrientation(double stdOrientation);
        double stdOrientation() const;
        
        template<class Archive>
        void serialize(Archive & ar, std::uint32_t const version)
        {
            //if (0 <= version) {
            ar(CEREAL_NVP(meanVelocity_));
            ar(CEREAL_NVP(stdVelocity_));
            ar(CEREAL_NVP(diffusionVelocity_));
            ar(CEREAL_NVP(minVelocity_));
            ar(CEREAL_NVP(maxVelocity_));
            ar(CEREAL_NVP(stdOrientation_));
            //}
        }
    };
}
// assign version
CEREAL_CLASS_VERSION(loc::PoseProperty, 0);
#endif /* Pose_hpp */
