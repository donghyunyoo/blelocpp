/*******************************************************************************
 * Copyright (c) 2014, 2016  IBM Corporation and others
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

#ifndef SystemModelInBuilding_hpp
#define SystemModelInBuilding_hpp

#include <stdio.h>
#include "RandomWalker.hpp"
#include "RandomWalkerMotion.hpp"
#include "Building.hpp"
#include "AltitudeManager.hpp"
#include "SerializeUtils.hpp"

namespace loc{
    
    class SystemModelInBuildingProperty{
        double probabilityUpStair_ = 0.1;
        double probabilityDownStair_ = 0.1;
        double probabilityStayStair_ = 0.8;
        
        double probabilityUpElevator_ = 0.1;
        double probabilityDownElevator_ = 0.1;
        double probabilityStayElevator_ = 0.8;
        
        double probabilityUpEscalator_ = 0.1;
        double probabilityDownEscalator_ = 0.1;
        double probabilityStayEscalator_ = 0.8;
        
        double probabilityFloorJump_ = 0.0;
        
        double wallCrossingAliveRate_ = 1.0; // fixed value
        double maxIncidenceAngle_ = 45/180*M_PI;
        
        // Field velocity rate
        double velocityRateFloor_ = 1.0;
        double velocityRateStair_ = 0.5;
        double velocityRateElevator_ = 0.5;
        double velocityRateEscalator_ = 0.5;
        
        double relativeVelocityEscalator_ = 0.4;
        
        double weightDecayRate_ = 0.9;
        
        int maxTrial_ = 1; // fixed value
        
    public:
        
        using Ptr = std::shared_ptr<SystemModelInBuildingProperty>;
        
        SystemModelInBuildingProperty& probabilityUpStair(double probabilityUp);
        SystemModelInBuildingProperty& probabilityDownStair(double probabilityDown);
        SystemModelInBuildingProperty& probabilityStayStair(double probabilityStay);
        
        SystemModelInBuildingProperty& probabilityUpElevator(double probabilityUp);
        SystemModelInBuildingProperty& probabilityDownElevator(double probabilityDown);
        SystemModelInBuildingProperty& probabilityStayElevator(double probabilityStay);
        
        SystemModelInBuildingProperty& probabilityUpEscalator(double probabilityUp);
        SystemModelInBuildingProperty& probabilityDownEscalator(double probabilityDown);
        SystemModelInBuildingProperty& probabilityStayEscalator(double probabilityStay);
        
        SystemModelInBuildingProperty& wallCrossingAliveRate(double wallCrossingAliveRate);
        SystemModelInBuildingProperty& maxIncidenceAngle(double maxIncidenceAngle);
        
        SystemModelInBuildingProperty& velocityRateFloor(double velocityRateFloor);
        SystemModelInBuildingProperty& velocityRateStair(double velocityRateStair);
        SystemModelInBuildingProperty& velocityRateElevator(double velocityRateElevator);
        SystemModelInBuildingProperty& velocityRateEscalator(double velocityRateEscalator);
        SystemModelInBuildingProperty& relativeVelocityEscalator(double relativeVelocityEscalator);
        
        SystemModelInBuildingProperty& weightDecayRate(double weightDecayRate);
        double probabilityUpStair() const;
        double probabilityDownStair() const;
        double probabilityStayStair() const;
        
        double probabilityUpElevator() const;
        double probabilityDownElevator() const;
        double probabilityStayElevator() const;
        
        double probabilityUpEscalator() const;
        double probabilityDownEscalator() const;
        double probabilityStayEscalator() const;
        
        double wallCrossingAliveRate() const;
        double maxIncidenceAngle() const;
        double velocityRateFloor() const;
        double velocityRateStair() const;
        double velocityRateElevator() const;
        double velocityRateEscalator() const;
        double relativeVelocityEscalator() const;
        double weightDecayRate() const;
        int maxTrial() const;
        
        SystemModelInBuildingProperty& probabilityFloorJump(double probability);
        double probabilityFloorJump() const;

        template<class Archive>
        void serialize(Archive & ar, std::uint32_t const version)
        {
            //if (0 <= version) {
            OPTIONAL_NVP(ar, probabilityUpStair_);
            OPTIONAL_NVP(ar, probabilityDownStair_);
            OPTIONAL_NVP(ar, probabilityStayStair_);

            OPTIONAL_NVP(ar, probabilityUpElevator_);
            OPTIONAL_NVP(ar, probabilityDownElevator_);
            OPTIONAL_NVP(ar, probabilityStayElevator_);

            OPTIONAL_NVP(ar, probabilityUpEscalator_);
            OPTIONAL_NVP(ar, probabilityDownEscalator_);
            OPTIONAL_NVP(ar, probabilityStayEscalator_);

            OPTIONAL_NVP(ar, probabilityFloorJump_);

            OPTIONAL_NVP(ar, wallCrossingAliveRate_);
            OPTIONAL_NVP(ar, maxIncidenceAngle_);

            OPTIONAL_NVP(ar, velocityRateFloor_);
            OPTIONAL_NVP(ar, velocityRateStair_);
            OPTIONAL_NVP(ar, velocityRateElevator_);
            OPTIONAL_NVP(ar, velocityRateEscalator_);

            OPTIONAL_NVP(ar, relativeVelocityEscalator_);

            OPTIONAL_NVP(ar, weightDecayRate_);
            //}
        }
    };
    
    template<class Tstate, class Tinput>
    class SystemModelInBuilding: public SystemModel<Tstate, Tinput>{
        
    public:
        using SystemModelT = SystemModel<Tstate, Tinput>;
        using Ptr = std::shared_ptr<SystemModelInBuilding>;
        
    private:
        RandomGenerator mRandomGenerator;
        typename SystemModel<Tstate, Tinput>::Ptr mSysModel;
        Building::Ptr mBuilding;
        SystemModelInBuildingProperty::Ptr mProperty;
        AltitudeManager::Ptr mAltManager;
        
        Tstate moveOnElevator(const Tstate& state, Tinput input);
        Tstate moveOnStair(const Tstate& state, Tinput input);
        Tstate moveOnEscalator(const Tstate& state, Tinput input);
        Tstate moveOnFloor(const Tstate& state, Tinput input);
        Tstate moveOnFloorRetry(const Tstate& state, const Tstate& stateNew,  Tinput input);
        Tstate moveFloorJump(const Tstate& state, Tinput input);
        
    public:
        
        SystemModelInBuilding() = default;
        virtual ~SystemModelInBuilding() = default;
        
        SystemModelInBuilding(typename SystemModelT::Ptr poseRandomWalker, Building::Ptr building, SystemModelInBuildingProperty::Ptr property);
        
        SystemModelInBuilding& systemModel(typename SystemModelT::Ptr sysModel);
        SystemModelInBuilding& building(Building::Ptr building);
        SystemModelInBuilding& property(SystemModelInBuildingProperty::Ptr property);
        SystemModelInBuilding& altitudeManager(AltitudeManager::Ptr altManager);
        
        Tstate predict(Tstate state, Tinput input) override;
        std::vector<Tstate> predict(std::vector<Tstate> states, Tinput input) override;
        
        virtual void notifyObservationUpdated() override;
        
    };
    
    // This class is retained for compatibility.
    using PoseRandomWalkerInBuildingProperty = SystemModelInBuildingProperty;
    
    class PoseRandomWalkerInBuilding: public SystemModelInBuilding<State, SystemModelInput>{
        
    public:
        using Ptr = std::shared_ptr<PoseRandomWalkerInBuilding>;
        
        virtual ~PoseRandomWalkerInBuilding() = default;
        
        virtual void poseRandomWalkerInBuildingProperty(SystemModelInBuildingProperty::Ptr property){
            SystemModelInBuilding<State, SystemModelInput>::property(property);
        }
        
        virtual void poseRandomWalker(typename SystemModel<State, SystemModelInput>::Ptr sysModel){
            SystemModelInBuilding<State, SystemModelInput>::systemModel(sysModel);
        }
        
    };
}

// assign version
CEREAL_CLASS_VERSION(loc::SystemModelInBuildingProperty, 0);

#endif /* SystemModelInBuilding_hpp */
