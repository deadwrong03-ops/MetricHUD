#pragma once

#include <cstdint>

namespace ArcDPS
{
    struct CombatEvent
    {
        uint64_t Time;
        uint64_t SourceAgent;
        uint64_t DestinationAgent;
        int32_t Value;
        int32_t BuffDamage;
        uint32_t OverstackValue;
        uint32_t SkillID; 
        uint16_t SourceInstanceID;
        uint16_t DestinationInstanceID;
        uint16_t SrcMasterInstanceID;
        uint16_t DestinationMasterInstanceID;
        uint8_t IFF;
        uint8_t Buff;
        uint8_t Result;
        uint8_t IsActivation;
        uint8_t IsBuffRemove;
        uint8_t IsNinety;
        uint8_t IsFifty;
        uint8_t IsMoving;
        uint8_t IsStatechange;
        uint8_t IsFlanking;
        uint8_t IsShields;
        uint8_t IsOffcycle;
        uint8_t PAD61;
        uint8_t PAD62;
        uint8_t PAD63;
        uint8_t PAD64;
    };
   

    struct Agent
    {
        char* Name;
        uintptr_t ID;
        uint32_t Profession;
        uint32_t Specialization;
        uint32_t IsSelf;
        uint16_t Team;
    };
}

struct EvCombatData
{
    ArcDPS::CombatEvent* ev;
    ArcDPS::Agent* src;
    ArcDPS::Agent* dst;
    char* skillname;
    uint64_t id;
    uint64_t revision;
};
