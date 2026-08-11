#pragma once

#include <cstdint>

namespace ArcDPS
{
    struct CombatEvent;
}

class CombatAnalyzer
{
public:
    CombatAnalyzer();

    void Initialize();
    void Shutdown();

    void ProcessEvent(const ArcDPS::CombatEvent* event);

    uint64_t GetEventCount() const;

private:
    uint64_t eventCount = 0;
};
