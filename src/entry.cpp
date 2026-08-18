///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// This code is licensed under the MIT license.
/// You should have received a copy of the license along with this source file.
/// You may obtain a copy of the license at: https://opensource.org/license/MIT
/// 
/// Name         :  entry.cpp
/// Description  :  Simple example of a Nexus addon implementation.
///----------------------------------------------------------------------------------------------------

#include <Windows.h>
#include "ConfigManager.h"
#include "MapRegistry.h"
#include "nexus/Nexus.h"
#include "mumble/Mumble.h"
#include "imgui/imgui.h"
#include "HUDWindow.h"
#include "Core/MetricRegistry.h"
#include "ArcDPS.h"
#include <string>
#include <cmath>
#include "CombatAnalyzer.h"
#include "EVTCAnalyzer.h"


/* proto */
void AddonLoad(AddonAPI_t* aApi);
void AddonUnload();
void AddonRender();
void AddonOptions();
void OnArcDPSCombat(void* eventArgs);
void OnArcDPSSquadCombat(void* eventArgs);

/* globals */
AddonDefinition_t AddonDef  = {};
HMODULE hSelf               = nullptr;
AddonAPI_t* APIDefs         = nullptr;
NexusLinkData_t* NexusLink  = nullptr;
Mumble::Data* MumbleLink    = nullptr;
MetricRegistry metricRegistry;
HUDWindow hudWindow;
ConfigManager configManager;
CombatAnalyzer combatAnalyzer;
EVTCAnalyzer evtcAnalyzer;

static bool arcPlayerInCombat = false;
static unsigned int debugLastArcStateChange = 0;




static float debugMumbleCombatTime = 0.0f;



///----------------------------------------------------------------------------------------------------
/// DllMain:
/// 	Main entry point for DLL.
/// 	We are not interested in this, all we get is our own HMODULE in case we need it.
///----------------------------------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: hSelf = hModule; break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

///----------------------------------------------------------------------------------------------------
/// GetAddonDef:
/// 	Export needed to give Nexus information about the addon.
///----------------------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) AddonDefinition_t* GetAddonDef()
{
	AddonDef.Signature = (uint32_t)-12345; // set to random unused negative integer
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "MetricHUD";
	AddonDef.Version.Major = 0;
	AddonDef.Version.Minor = 1;
	AddonDef.Version.Build = 0;
	AddonDef.Version.Revision = 1;
	AddonDef.Author = "Scott K";
	AddonDef.Description = "A customizable combat metrics HUD for Guild Wars 2";
	AddonDef.Load = AddonLoad;
	AddonDef.Unload = AddonUnload;
	AddonDef.Flags = AF_None;

	/* not necessary if hosted on Raidcore, but shown anyway for the example also useful as a backup resource */
	//AddonDef.Provider = UP_GitHub;
	//AddonDef.UpdateLink = "https://github.com/RaidcoreGG/GW2Nexus-AddonTemplate";

	return &AddonDef;
}

///----------------------------------------------------------------------------------------------------
/// AddonLoad:
/// 	Load function for the addon, will receive a pointer to the API.
/// 	(You probably want to store it.)
///----------------------------------------------------------------------------------------------------
void AddonLoad(AddonAPI_t* aApi)
{
	APIDefs = aApi; // store the api somewhere easily accessible

	ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext); // cast to ImGuiContext*
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree); // on imgui 1.80+

	NexusLink = (NexusLinkData_t*)APIDefs->DataLink_Get("DL_NEXUS_LINK");
	MumbleLink = (Mumble::Data*)APIDefs->DataLink_Get("DL_MUMBLE_LINK");
	metricRegistry.Initialize();
	combatAnalyzer.Initialize();
	configManager.Initialize();
	APIDefs->Events_Subscribe(
		"EV_ARCDPS_COMBATEVENT_LOCAL_RAW",
		OnArcDPSCombat
	);
	APIDefs->Events_Subscribe(
		"EV_ARCDPS_COMBATEVENT_SQUAD_RAW",
		OnArcDPSSquadCombat
	);
	

	// Add an options window and a regular render callback
	APIDefs->GUI_Register(RT_Render, AddonRender); const auto& skillUseCounts =
		combatAnalyzer.GetSkillUseCounts();
	APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);

	APIDefs->Log(LOGL_DEBUG, "MetricHUD", "My <c=#00ff00>first addon</c> loaded successfully.");
}

void OnArcDPSCombat(void* eventArgs)
{
	EvCombatData* combatData = static_cast<EvCombatData*>(eventArgs);

	if (combatData == nullptr || combatData->ev == nullptr)
	{
		return;
	}

	ArcDPS::CombatEvent* ev = combatData->ev;
	



	if (combatData->src == nullptr)
	{
		return;
	}

	if (combatData->src->IsSelf != 0)
	{
		combatAnalyzer.SetPlayerInstanceID(ev->SourceInstanceID);
	}
	if (combatData->src->IsSelf == 0 &&
		ev->SrcMasterInstanceID != combatAnalyzer.GetPlayerInstanceID())
	{


		return;
	}

	if (ev->IsStatechange != 0)
	{
		debugLastArcStateChange =
			static_cast<unsigned int>(ev->IsStatechange);

		if (ev->IsStatechange == 1 &&
			combatData->src != nullptr &&
			combatData->src->IsSelf != 0)
		{
			arcPlayerInCombat = true;
			combatAnalyzer.ResetSession();
			combatAnalyzer.SetCombatStartTime(ev->Time);

		}
		if (ev->IsStatechange == 2 &&
			combatData->src != nullptr &&
			combatData->src->IsSelf != 0)
		{
			arcPlayerInCombat = false;
			combatAnalyzer.CaptureLastFight();
			
		}
		return;
	}

	combatAnalyzer.ProcessEvent(
		ev,
		combatData->skillname,
		combatData->id

	);
}
void OnArcDPSSquadCombat(void* eventArgs)
{
	EvCombatData* combatData =
		static_cast<EvCombatData*>(eventArgs);

	if (combatData == nullptr || combatData->ev == nullptr)
	{
		return;
	}

	ArcDPS::CombatEvent* ev = combatData->ev;

	if (ev->IsStatechange == 5)
	{
		if (combatData->src != nullptr &&
			combatData->src->IsSelf != 0)
		{
			
			combatAnalyzer.IncrementDownedCount();
		}
	}
	if (ev->IsStatechange == 4)
	{
		if (combatData->src != nullptr &&
			combatData->src->IsSelf != 0)
		{
			combatAnalyzer.IncrementDeathCount();

			if (!arcPlayerInCombat)
			{
				combatAnalyzer.RefreshLastFightSurvivalCounts();
			}
		}
	}
}


	


///----------------------------------------------------------------------------------------------------
/// AddonUnload:
/// 	Everything you registered in AddonLoad, you should "undo" here.
///----------------------------------------------------------------------------------------------------
void AddonUnload()
{
	APIDefs->Events_Unsubscribe(
		"EV_ARCDPS_COMBATEVENT_LOCAL_RAW",
		OnArcDPSCombat
	);
	APIDefs->Events_Unsubscribe(
		"EV_ARCDPS_COMBATEVENT_SQUAD_RAW",
		OnArcDPSSquadCombat
	);
	

	metricRegistry.Shutdown();
	configManager.Shutdown();

	/* let's clean up after ourselves */
	APIDefs->GUI_Deregister(AddonRender);
	APIDefs->GUI_Deregister(AddonOptions);

	APIDefs->Log(LOGL_DEBUG, "MetricHUD", "<c=#ff0000>Signing off</c>, it was an honor commander.");
}

///----------------------------------------------------------------------------------------------------
/// AddonRender:
/// 	Called every frame. Safe to render any ImGui.
/// 	You can control visibility on loading screens with NexusLink->IsGameplay.
///----------------------------------------------------------------------------------------------------

std::string WideToUTF8(const wchar_t* wideText)
{
	if (wideText == nullptr || wideText[0] == L'\0')
	{
		return "";
	}

	int sizeNeeded = WideCharToMultiByte(
		CP_UTF8,
		0,
		wideText,
		-1,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	if (sizeNeeded <= 0)
	{
		return "";
	}

	std::string result(sizeNeeded - 1, '\0');

	WideCharToMultiByte(
		CP_UTF8,
		0,
		wideText,
		-1,
		&result[0],
		sizeNeeded,
		nullptr,
		nullptr
	);

	return result;
}
std::string GetCharacterName(const std::string& identity)
{
	const std::string key = "\"name\":\"";

	size_t start = identity.find(key);

	if (start == std::string::npos)
	{
		return "Unknown";
	}

	start += key.length();

	size_t end = identity.find('"', start);

	if (end == std::string::npos)
	{
		return "Unknown";
	}

	return identity.substr(start, end - start);
}
void AddonRender()
{
	static float combatTime = 0.0f;
	static bool wasInCombat = false;
	static Vector2 previousPosition = { 0.0f, 0.0f };
	static bool hasPreviousPosition = false;

	metricRegistry.SetFPS(ImGui::GetIO().Framerate);

	bool isInCombat =
		MumbleLink != nullptr &&
		MumbleLink->Context.IsInCombat;

	if (isInCombat && !wasInCombat)
	{
		// CombatAnalyzer session boundaries are handled
		// by ArcDPS ENTERCOMBAT / EXITCOMBAT state changes.
	}

	if (isInCombat)
	{
		combatTime += ImGui::GetIO().DeltaTime;
	}
	else
	{
		combatTime = 0.0f;
	}

	wasInCombat = isInCombat;
	Vector2 currentPosition = MumbleLink->Context.Compass.PlayerPosition;

	float playerSpeed = 0.0f;

	if (hasPreviousPosition)
	{
		float deltaX = currentPosition.X - previousPosition.X;
		float deltaY = currentPosition.Y - previousPosition.Y;
		float distance = sqrtf((deltaX * deltaX) + (deltaY * deltaY));

		float deltaTime = ImGui::GetIO().DeltaTime;

		if (deltaTime > 0.0f)
		{
			playerSpeed = distance / deltaTime;
		}
	}

	previousPosition = currentPosition;
	hasPreviousPosition = true;

	metricRegistry.SetMetricValue(MetricID::PlayerSpeed, playerSpeed);
	if (MumbleLink != nullptr)
	{
		unsigned int mapID = MumbleLink->Context.MapID;

		metricRegistry.SetMetricValue(
			MetricID::MapID,
			static_cast<float>(mapID)
		);

		metricRegistry.SetMetricText(
			MetricID::MapName,
			MapRegistry::GetMapName(mapID)
		);

		std::string identity = WideToUTF8(MumbleLink->Identity);
		std::string characterName = GetCharacterName(identity);

		metricRegistry.SetMetricText(
			MetricID::CharacterName,
			characterName.c_str()
		);
	}
	debugMumbleCombatTime = combatTime;
	metricRegistry.SetMetricValue(MetricID::CombatTime, combatTime);
	
	if (MumbleLink != nullptr && MumbleLink->Context.IsInCombat)
	{
		metricRegistry.SetDPS(combatAnalyzer.GetDPS());

		metricRegistry.SetDamage(
			static_cast<double>(
				combatAnalyzer.GetTotalDirectDamage() +
				combatAnalyzer.GetTotalBuffDamage()
				)
		);
	}
	else
	{
		metricRegistry.SetDPS(0.0);
		metricRegistry.SetDamage(0.0);
	}

	metricRegistry.SetDownedCount(
		static_cast<double>(
			combatAnalyzer.GetDownedCount()
			)
	);

	metricRegistry.SetDeathCount(
		static_cast<double>(
			combatAnalyzer.GetDeathCount()
			)
	);
	metricRegistry.SetLastFightDPS(combatAnalyzer.GetLastFightDPS());
	metricRegistry.SetLastFightTime(
		combatAnalyzer.GetLastFightDurationSeconds()
	);
	metricRegistry.SetLastFightDamage(
		combatAnalyzer.GetLastFightDamage()
	);
	metricRegistry.SetLastFightDownedCount(
		static_cast<double>(
			combatAnalyzer.GetLastFightDownedCount()
			)
	);

	metricRegistry.SetLastFightDeathCount(
		static_cast<double>(
			combatAnalyzer.GetLastFightDeathCount()
			)
	);
	if (configManager.ShowHUD())
	{
		hudWindow.Render(configManager, metricRegistry);
	}
}


///----------------------------------------------------------------------------------------------------
/// AddonOptions:
/// 	Basically an ImGui callback that doesn't need its own Begin/End calls.
///----------------------------------------------------------------------------------------------------
void AddonOptions()
{
	ImGui::Text("MetricHUD");
	ImGui::Separator();

	bool showHUD = configManager.ShowHUD();
	bool locked = configManager.IsLocked();

	if (ImGui::Checkbox("Show HUD", &showHUD))
	{
		configManager.SetShowHUD(showHUD);
	}

	if (ImGui::Checkbox("Lock HUD Position", &locked))
	{
		configManager.SetLocked(locked);
	}

	MetricDefinition* combatMetric = metricRegistry.GetMetric(MetricID::CombatTime);

	if (combatMetric != nullptr)
	{
		bool combatEnabled = combatMetric->enabled;

		if (ImGui::Checkbox("Show Combat Time", &combatEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::CombatTime, combatEnabled);
		}
	}

	MetricDefinition* dpsMetric = metricRegistry.GetMetric(MetricID::DPS);

	if (dpsMetric != nullptr)
	{
		bool dpsEnabled = dpsMetric->enabled;

		if (ImGui::Checkbox("Show DPS", &dpsEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::DPS, dpsEnabled);
		}
	}
	MetricDefinition* damageMetric =
		metricRegistry.GetMetric(MetricID::Damage);

	if (damageMetric != nullptr)
	{
		bool damageEnabled = damageMetric->enabled;

		if (ImGui::Checkbox("Show Damage", &damageEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::Damage,
				damageEnabled
			);
		}
	}
	MetricDefinition* lastFightDpsMetric =
		metricRegistry.GetMetric(MetricID::LastFightDPS);
	MetricDefinition* downedCountMetric =
		metricRegistry.GetMetric(MetricID::DownedCount);

	if (downedCountMetric != nullptr)
	{
		bool downedCountEnabled = downedCountMetric->enabled;

		if (ImGui::Checkbox("Show Downed Count", &downedCountEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::DownedCount,
				downedCountEnabled
			);
		}
	}
	MetricDefinition* deathCountMetric =
		metricRegistry.GetMetric(MetricID::DeathCount);

	if (deathCountMetric != nullptr)
	{
		bool deathCountEnabled = deathCountMetric->enabled;

		if (ImGui::Checkbox("Show Death Count", &deathCountEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::DeathCount,
				deathCountEnabled
			);
		}
	}

	if (lastFightDpsMetric != nullptr)
	{
		bool lastFightDpsEnabled = lastFightDpsMetric->enabled;

		if (ImGui::Checkbox("Show Last Fight DPS", &lastFightDpsEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::LastFightDPS,
				lastFightDpsEnabled
			);
		}
	}
	MetricDefinition* lastFightTimeMetric =
		metricRegistry.GetMetric(MetricID::LastFightTime);

	if (lastFightTimeMetric != nullptr)
	{
		bool lastFightTimeEnabled = lastFightTimeMetric->enabled;

		if (ImGui::Checkbox("Show Last Fight Time", &lastFightTimeEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::LastFightTime,
				lastFightTimeEnabled
			);
		}
	}
	MetricDefinition* lastFightDamageMetric =
		metricRegistry.GetMetric(MetricID::LastFightDamage);

	if (lastFightDamageMetric != nullptr)
	{
		bool lastFightDamageEnabled = lastFightDamageMetric->enabled;

		if (ImGui::Checkbox("Show Last Fight Damage", &lastFightDamageEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::LastFightDamage,
				lastFightDamageEnabled
			);
		}
	}
	MetricDefinition* lastFightDownedMetric =
		metricRegistry.GetMetric(MetricID::LastFightDownedCount);

	if (lastFightDownedMetric != nullptr)
	{
		bool lastFightDownedEnabled = lastFightDownedMetric->enabled;

		if (ImGui::Checkbox("Show Last Fight Downs", &lastFightDownedEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::LastFightDownedCount,
				lastFightDownedEnabled
			);
		}
	}

	MetricDefinition* lastFightDeathMetric =
		metricRegistry.GetMetric(MetricID::LastFightDeathCount);

	if (lastFightDeathMetric != nullptr)
	{
		bool lastFightDeathEnabled = lastFightDeathMetric->enabled;

		if (ImGui::Checkbox("Show Last Fight Deaths", &lastFightDeathEnabled))
		{
			metricRegistry.SetMetricEnabled(
				MetricID::LastFightDeathCount,
				lastFightDeathEnabled
			);
		}
	}

	MetricDefinition* mapNameMetric = metricRegistry.GetMetric(MetricID::MapName);

	if (mapNameMetric != nullptr)
	{
		bool mapNameEnabled = mapNameMetric->enabled;

		if (ImGui::Checkbox("Show Map Name", &mapNameEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::MapName, mapNameEnabled);
		}
	}

	MetricDefinition* mapIDMetric = metricRegistry.GetMetric(MetricID::MapID);

	if (mapIDMetric != nullptr)
	{
		bool mapIDEnabled = mapIDMetric->enabled;

		if (ImGui::Checkbox("Show Map ID", &mapIDEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::MapID, mapIDEnabled);
		}
	}

	MetricDefinition* characterMetric = metricRegistry.GetMetric(MetricID::CharacterName);

	if (characterMetric != nullptr)
	{
		bool characterEnabled = characterMetric->enabled;

		if (ImGui::Checkbox("Show Character Name", &characterEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::CharacterName, characterEnabled);
		}
	}

	MetricDefinition* speedMetric = metricRegistry.GetMetric(MetricID::PlayerSpeed);

	if (speedMetric != nullptr)
	{
		bool speedEnabled = speedMetric->enabled;

		if (ImGui::Checkbox("Show Player Speed", &speedEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::PlayerSpeed, speedEnabled);
		}
	}

	MetricDefinition* pingMetric = metricRegistry.GetMetric(MetricID::Ping);

	if (pingMetric != nullptr)
	{
		bool pingEnabled = pingMetric->enabled;

		if (ImGui::Checkbox("Show Ping", &pingEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::Ping, pingEnabled);
		}
	}

	MetricDefinition* fpsMetric = metricRegistry.GetMetric(MetricID::FPS);

	if (fpsMetric != nullptr)
	{
		bool fpsEnabled = fpsMetric->enabled;

		if (ImGui::Checkbox("Show FPS", &fpsEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::FPS, fpsEnabled);
		}
	}

	if (ImGui::CollapsingHeader("Combat Analyzer Debug"))


	{
		
		ImGui::Text("Last Arc StateChange: %u", debugLastArcStateChange);
		

		
		
		ImGui::Text(
			"Analyzer Downed Count: %u",
			combatAnalyzer.GetDownedCount()
		);
		
		
		
		ImGui::Separator();

		ImGui::Text("Version 0.1.0");
		ImGui::TextDisabled("Settings will be added during development.");
		ImGui::Text("Combat Events: %llu", combatAnalyzer.GetEventCount());
		ImGui::Text("Damage Events: %llu", combatAnalyzer.GetDamageEventCount());
		ImGui::Text("Zero-Skill Damage Events: %llu", combatAnalyzer.GetZeroSkillDamageEventCount());

		ImGui::Text("Direct Damage: %lld", combatAnalyzer.GetTotalDirectDamage());
		ImGui::Text("Buff Damage: %lld", combatAnalyzer.GetTotalBuffDamage());
		
		
		ImGui::Text(
			"First Damage Time: %llu",
			static_cast<unsigned long long>(combatAnalyzer.GetFirstDamageTime())
		);

		ImGui::Text(
			"Last Damage Time: %llu",
			static_cast<unsigned long long>(combatAnalyzer.GetLastDamageTime())
		);

		ImGui::Text(
			"Analyzer Combat Time: %.2f",
			combatAnalyzer.GetCombatDurationSeconds()
		);
		ImGui::Text("Arc Start -> Last Damage: %.2f",
			combatAnalyzer.GetArcStartToLastDamageSeconds());
		
		ImGui::Text("Mumble Combat Time: %.2f", debugMumbleCombatTime);
		
		ImGui::Text(
			"Analyzer DPS: %.1f",
			combatAnalyzer.GetDPS()
		);

		ImGui::Separator();

		ImGui::Text("Last Fight Damage: %lld", combatAnalyzer.GetLastFightDamage());

		ImGui::Text(
			"Last Fight Time: %.2f",
			combatAnalyzer.GetLastFightDurationSeconds()
		);

		ImGui::Text(
			"Last Fight DPS: %.1f",
			combatAnalyzer.GetLastFightDPS()
		);
		ImGui::Text("Last Skill ID: %u", combatAnalyzer.GetLastSkillID());
		ImGui::Text("Last Activation: %u", combatAnalyzer.GetLastActivation());
		const auto& recentSkills = combatAnalyzer.GetRecentSkills();

		ImGui::Text("Recent Skills:");

		for (uint32_t skillID : recentSkills)
		{
			const std::string skillName =
				combatAnalyzer.GetSkillName(skillID);

			ImGui::Text(
				"  %s (%u)",
				skillName.c_str(),
				skillID
			);
		}

		ImGui::Separator();

		ImGui::Text("Skill Usage:");

		const auto& skillUseCounts =
			combatAnalyzer.GetSkillUseCounts();

		for (const auto& entry : skillUseCounts)
		{
			const uint32_t skillID = entry.first;
			const uint32_t count = entry.second;

			const std::string skillName =
				combatAnalyzer.GetSkillName(skillID);

			ImGui::Text(
				"  %s (%u): %u",
				skillName.c_str(),
				skillID,
				count
			);
		}

		ImGui::Separator();

		ImGui::Text("Damage By Skill:");

		const auto& damageBySkill =
			combatAnalyzer.GetDamageBySkill();

		for (const auto& entry : damageBySkill)
		{
			const uint32_t skillID = entry.first;
			const int64_t damage = entry.second;

			const std::string skillName =
				combatAnalyzer.GetSkillName(skillID);

			ImGui::Text(
				"  %s (%u): %lld",
				skillName.c_str(),
				skillID,
				static_cast<long long>(damage)
			);
		}

		const auto& recentRecords =
			combatAnalyzer.GetRecentRecords();

		ImGui::Separator();
		ImGui::Text("Recent Records:");

		for (const CombatRecord& record : recentRecords)
		{
			ImGui::Text(
				"Time:%llu Event:%llu ID:%u Src:%llu SrcInst:%u Dst:%llu DstInst:%u Master:%u Val:%d BuffDmg:%d Over:%u Buff:%u Result:%u IFF:%u Act:%u 90:%u 50:%u Move:%u",
				record.time,
				record.eventID,
				record.skillID,
				record.sourceAgent,
				record.sourceInstanceID,
				record.destinationAgent,
				record.destinationInstanceID,
				record.sourceMasterInstanceID,
				record.value,
				record.buffDamage,
				record.overstackValue,
				record.buff,
				record.result,
				record.iff,
				record.isActivation,
				record.isNinety,
				record.isFifty,
				record.isMoving
			);
		}

		ImGui::Separator();
		ImGui::Text("Activation Events:");

		for (const CombatRecord& record : recentRecords)
		{
			if (record.isActivation != 0)
			{
				const std::string skillName =
					combatAnalyzer.GetSkillName(record.skillID);

				ImGui::Text(
					"  %s (%u) Act:%u",
					skillName.c_str(),
					record.skillID,
					record.isActivation
				);
			}
		}

		ImGui::Separator();
		ImGui::Text("EVTC Analyzer Test:");

		static bool evtcLoaded = false;

		if (!evtcLoaded)
		{
			evtcLoaded = evtcAnalyzer.LoadFile(
				R"(C:\Users\deadw\OneDrive\Documents\Guild Wars 2\addons\arcdps\arcdps.cbtlogs\Standard Kitty Golem (16199)\20260814-235252.zevtc)"
			);
		}

		ImGui::Text(
			"EVTC Load: %s",
			evtcLoaded ? "PASS" : "FAIL"
		);

		const EVTCHeader& evtcHeader =
			evtcAnalyzer.GetHeader();

		ImGui::Text(
			"EVTC Version: %s",
			evtcHeader.version.c_str()
		);

		ImGui::Text(
			"EVTC Revision: %u",
			static_cast<unsigned int>(evtcHeader.revision)
		);

		ImGui::Text(
			"EVTC Encounter ID: %u",
			static_cast<unsigned int>(evtcHeader.encounterID)
		);
		ImGui::Text(
			"EVTC Agent Count: %u",
			static_cast<unsigned int>(evtcHeader.agentCount)
		);
		const auto& evtcAgents = evtcAnalyzer.GetAgents();
		ImGui::Text(
			"EVTC Skill Count: %u",
			static_cast<unsigned int>(evtcHeader.skillCount)
		);
		ImGui::Text(
			"EVTC Extracted Size: %llu",
			static_cast<unsigned long long>(evtcHeader.extractedSize)
		);

		ImGui::Text(
			"EVTC Combat Event Offset: %llu",
			static_cast<unsigned long long>(evtcHeader.combatEventOffset)
		);

		ImGui::Text(
			"EVTC Combat Event Count: %llu",
			static_cast<unsigned long long>(evtcHeader.combatEventCount)
		);
		const auto& evtcCombatEvents = evtcAnalyzer.GetCombatEvents();

		ImGui::Text(
			"Parsed Combat Events: %u",
			static_cast<unsigned int>(evtcCombatEvents.size())
		);



		// STEP 140 - Count damage-looking combat events.
		size_t damageEventCount = 0;

		for (const EVTCCombatEvent& event : evtcCombatEvents)
		{
			if (event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buff == 0 &&
				event.value > 0)
			{
				++damageEventCount;
			}

		}
		ImGui::Text("Owned-Agent Candidates:");

		size_t shownOwnedCandidates = 0;

		for (const EVTCCombatEvent& event : evtcCombatEvents)
		{
			if (event.srcMasterInstid != 0)
			{
				ImGui::Text(
					"Src:%llu SrcInst:%u Master:%u Skill:%u Value:%d BuffDmg:%d",
					static_cast<unsigned long long>(event.srcAgent),
					static_cast<unsigned int>(event.srcInstid),
					static_cast<unsigned int>(event.srcMasterInstid),
					static_cast<unsigned int>(event.skillID),
					event.value,
					event.buffDmg
				);

				++shownOwnedCandidates;

				if (shownOwnedCandidates >= 12)
				{
					break;
				}
			}
		}

		ImGui::Text(
			"Damage Combat Events: %u",
			static_cast<unsigned int>(damageEventCount)
		);


		size_t shownDamageEvents = 0;

		for (const EVTCCombatEvent& event : evtcCombatEvents)
		{
			if (event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buff == 0 &&
				event.value > 0)
			{
				ImGui::Text(
					"DMG %u: Src:%llu Dst:%llu Skill:%u Value:%d",
					static_cast<unsigned int>(shownDamageEvents),
					static_cast<unsigned long long>(event.srcAgent),
					static_cast<unsigned long long>(event.dstAgent),
					static_cast<unsigned int>(event.skillID),
					event.value
				);

				++shownDamageEvents;

				if (shownDamageEvents >= 8)
				{
					break;
				}
			}
		}

		ImGui::Text("Damage Source Lookup:");

		uint16_t playerInstanceID = 0;

		if (!evtcCombatEvents.empty())
		{
			uint64_t damageSource = 0;

			for (const EVTCCombatEvent& event : evtcCombatEvents)
			{
				if (event.isStateChange == 0 &&
					event.isActivation == 0 &&
					event.buff == 0 &&
					event.value > 0)
				{
					damageSource = event.srcAgent;
					playerInstanceID = event.srcInstid;
					break;;
				}
			}

			for (const EVTCAgent& agent : evtcAgents)
			{
				if (agent.address == damageSource)
				{
					ImGui::Text(
						"Src %llu = %s | Prof:%u Elite:%u",
						static_cast<unsigned long long>(damageSource),
						agent.name.c_str(),
						static_cast<unsigned int>(agent.profession),
						static_cast<unsigned int>(agent.elite)
					);

					break;
				}
			}
		}
		ImGui::Text(
			"Player Instance ID: %u",
			static_cast<unsigned int>(playerInstanceID)
		);
		uint64_t playerAgentAddress = 0;

		for (const EVTCAgent& agent : evtcAgents)
		{
			if (agent.elite == 0xFFFFFFFF)
			{
				continue;
			}

			playerAgentAddress = agent.address;
			break;
		}
		uint64_t playerDamage = 0;
		size_t playerDamageEvents = 0;
		uint64_t playerBuffDamage = 0;
		size_t playerBuffDamageEvents = 0;
		uint64_t playerTotalDamage = 0;
		uint64_t firstPlayerDamageTime = 0;
		uint64_t lastPlayerDamageTime = 0;
		uint64_t ownedDirectDamage = 0;
		size_t ownedDirectDamageEvents = 0;
		uint64_t ownedBuffDamage = 0;
		size_t ownedBuffDamageEvents = 0;
		uint64_t combinedTotalDamage = 0;
		double combinedDPS = 0.0;
		uint64_t firstOwnedDamageTime = 0;
		uint64_t lastOwnedDamageTime = 0;
		uint64_t combinedFirstDamageTime = 0;
		uint64_t combinedLastDamageTime = 0;
		
		for (const EVTCCombatEvent& event : evtcCombatEvents)
		{
			if (event.srcMasterInstid == playerInstanceID &&
				event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buff == 0 &&
				event.value > 0)
			{
				ownedDirectDamage += static_cast<uint64_t>(event.value);
				++ownedDirectDamageEvents;
				if (firstOwnedDamageTime == 0)
				{
					firstOwnedDamageTime = event.time;
				}

				lastOwnedDamageTime = event.time;
			}
			if (event.srcAgent == playerAgentAddress &&
				event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buff == 0 &&
				event.value > 0)
			{
				if (event.srcMasterInstid == playerInstanceID &&
					event.isStateChange == 0 &&
					event.isActivation == 0 &&
					event.buff != 0 &&
					event.buffDmg > 0)
				{
					ownedBuffDamage += static_cast<uint64_t>(event.buffDmg);
					++ownedBuffDamageEvents;
					if (firstOwnedDamageTime == 0)
					{
						firstOwnedDamageTime = event.time;
					}

					lastOwnedDamageTime = event.time;
				}
				playerDamage += static_cast<uint64_t>(event.value);
				++playerDamageEvents;
				if (firstPlayerDamageTime == 0)
				{
					firstPlayerDamageTime = event.time;
				}

				lastPlayerDamageTime = event.time;
			}
			if (event.srcAgent == playerAgentAddress &&
				event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buff != 0 &&
				event.buffDmg > 0)
			{
				playerBuffDamage += static_cast<uint64_t>(event.buffDmg);
				++playerBuffDamageEvents;
			}
			if (event.isStateChange == 0 &&
				event.isActivation == 0 &&
				event.buffDmg != 0)
			{
				ImGui::Text(
					"BUFF EVT: Src:%llu Dst:%llu Skill:%u BuffFlag:%u BuffDmg:%d SrcInst:%u SrcMaster:%u",
					static_cast<unsigned long long>(event.srcAgent),
					static_cast<unsigned long long>(event.dstAgent),
					static_cast<unsigned int>(event.skillID),
					static_cast<unsigned int>(event.buff),
					event.buffDmg,
					static_cast<unsigned int>(event.srcInstid),
					static_cast<unsigned int>(event.srcMasterInstid)
				);
			}

		}

		playerTotalDamage = playerDamage + playerBuffDamage;

		combinedTotalDamage =
			playerTotalDamage +
			ownedDirectDamage +
			ownedBuffDamage;

		if (firstPlayerDamageTime != 0 && firstOwnedDamageTime != 0)
		{
			combinedFirstDamageTime =
				(firstPlayerDamageTime < firstOwnedDamageTime)
				? firstPlayerDamageTime
				: firstOwnedDamageTime;
		}
		else if (firstPlayerDamageTime != 0)
		{
			combinedFirstDamageTime = firstPlayerDamageTime;
		}
		else
		{
			combinedFirstDamageTime = firstOwnedDamageTime;
		}

		if (lastPlayerDamageTime != 0 && lastOwnedDamageTime != 0)
		{
			combinedLastDamageTime =
				(lastPlayerDamageTime > lastOwnedDamageTime)
				? lastPlayerDamageTime
				: lastOwnedDamageTime;
		}
		else if (lastPlayerDamageTime != 0)
		{
			combinedLastDamageTime = lastPlayerDamageTime;
		}
		else
		{
			combinedLastDamageTime = lastOwnedDamageTime;
		}

		if (combinedFirstDamageTime != 0 &&
			combinedLastDamageTime > combinedFirstDamageTime)
		{
			double combinedDurationSeconds =
				static_cast<double>(
					combinedLastDamageTime - combinedFirstDamageTime
					) / 1000.0;

			combinedDPS =
				static_cast<double>(combinedTotalDamage) /
				combinedDurationSeconds;
		}
		ImGui::Text(
			"Player Direct Damage: %llu",
			static_cast<unsigned long long>(playerDamage)
		);

		ImGui::Text(
			"Player Direct Damage Events: %u",
			static_cast<unsigned int>(playerDamageEvents)
		);
		ImGui::Text(
			"Player Buff Damage: %llu",
			static_cast<unsigned long long>(playerBuffDamage)
		);
		ImGui::Text(
			"Player Total Damage: %llu",
			static_cast<unsigned long long>(playerTotalDamage)
		);

		ImGui::Text(
			"Player Buff Damage Events: %u",
			static_cast<unsigned int>(playerBuffDamageEvents)
		);
		ImGui::Text(
			"First Player Damage Time: %llu",
			static_cast<unsigned long long>(firstPlayerDamageTime)
		);

		ImGui::Text(
			"Last Player Damage Time: %llu",
			static_cast<unsigned long long>(lastPlayerDamageTime)
		);
		ImGui::Separator();

		ImGui::Text(
			"Owned Direct Damage: %llu",
			static_cast<unsigned long long>(ownedDirectDamage)
		);

		ImGui::Text(
			"Owned Direct Damage Events: %u",
			static_cast<unsigned int>(ownedDirectDamageEvents)
		);

		ImGui::Text(
			"Owned Buff Damage: %llu",
			static_cast<unsigned long long>(ownedBuffDamage)
		);

		ImGui::Text(
			"Owned Buff Damage Events: %u",
			static_cast<unsigned int>(ownedBuffDamageEvents)
		);

		ImGui::Text(
			"Owned Total Damage: %llu",
			static_cast<unsigned long long>(ownedDirectDamage + ownedBuffDamage)
		);
		ImGui::Text(
			"Combined Player + Owned Damage: %llu",
			static_cast<unsigned long long>(combinedTotalDamage)
		);
		ImGui::Text(
			"Combined Player + Owned DPS: %.1f",
			combinedDPS
		);
		ImGui::Text(
			"Combined First Damage Time: %llu",
			static_cast<unsigned long long>(combinedFirstDamageTime)
		);

		ImGui::Text(
			"Combined Last Damage Time: %llu",
			static_cast<unsigned long long>(combinedLastDamageTime)
		);

		uint64_t playerDamageDuration = 0;

		if (lastPlayerDamageTime >= firstPlayerDamageTime &&
			firstPlayerDamageTime > 0)
		{
			playerDamageDuration =
				lastPlayerDamageTime - firstPlayerDamageTime;
		}

		ImGui::Text(
			"Player Damage Duration: %llu",
			static_cast<unsigned long long>(playerDamageDuration)
		);
		double playerEVTCDPS = 0.0;

		if (playerDamageDuration > 0)
		{
			const double durationSeconds =
				static_cast<double>(playerDamageDuration) / 1000.0;

			playerEVTCDPS =
				static_cast<double>(playerDamage) / durationSeconds;
		}

		ImGui::Text(
			"Player EVTC DPS: %.1f",
			playerEVTCDPS
		);
		double playerTotalEVTCDPS = 0.0;

		if (playerDamageDuration > 0)
		{
			const double durationSeconds =
				static_cast<double>(playerDamageDuration) / 1000.0;

			playerTotalEVTCDPS =
				static_cast<double>(playerTotalDamage) / durationSeconds;
		}

		ImGui::Text(
			"Player Total EVTC DPS: %.1f",
			playerTotalEVTCDPS
		);
		double playerAndOwnedEVTCDPS = 0.0;

		if (combinedFirstDamageTime != 0 &&
			combinedLastDamageTime > combinedFirstDamageTime)
		{
			const double combinedDurationSeconds =
				static_cast<double>(
					combinedLastDamageTime - combinedFirstDamageTime
					) / 1000.0;

			playerAndOwnedEVTCDPS =
				static_cast<double>(combinedTotalDamage) /
				combinedDurationSeconds;
		}

		ImGui::Text(
			"Player + Owned EVTC DPS: %.1f",
			playerAndOwnedEVTCDPS
		);



		if (!evtcCombatEvents.empty())
		
		{
			const EVTCCombatEvent& event = evtcCombatEvents.front();

			ImGui::Text(
				"First Event Time: %llu",
				static_cast<unsigned long long>(event.time)
			);

			ImGui::Text(
				"First Event Src: %llu",
				static_cast<unsigned long long>(event.srcAgent)
			);

			ImGui::Text(
				"First Event Dst: %llu",
				static_cast<unsigned long long>(event.dstAgent)
			);

			ImGui::Text(
				"First Event Value: %d",
				event.value
			);

			ImGui::Text(
				"First Event Skill ID: %u",
				static_cast<unsigned int>(event.skillID)
			);

			ImGui::Text(
				"First Event StateChange: %u",
				static_cast<unsigned int>(event.isStateChange)
			);

			ImGui::Text(
				"First Event Activation: %u",
				static_cast<unsigned int>(event.isActivation)
			);
		}

		const auto& evtcSkills = evtcAnalyzer.GetSkills();

		ImGui::Text(
			"Parsed Skills: %u",
			static_cast<unsigned int>(evtcSkills.size())
		);
		
		if (!evtcSkills.empty())
		{
			ImGui::Text("First Skills:");

			const size_t displaySkillCount =
				evtcSkills.size() < 8 ? evtcSkills.size() : 8;

			for (size_t i = 0; i < displaySkillCount; ++i)
			{
				const EVTCSkill& skill = evtcSkills[i];

				ImGui::Text(
					"%u: %s (%u)",
					static_cast<unsigned int>(i),
					skill.name.c_str(),
					static_cast<unsigned int>(skill.id)
				);
			}
		}

		if (!evtcSkills.empty())
		{
			const EVTCSkill& skill = evtcSkills.front();

			ImGui::Text(
				"First Skill: %s (%u)",
				skill.name.c_str(),
				skill.id
			);
		}
		ImGui::Text(
			"Parsed Agents: %u",
			static_cast<unsigned int>(evtcAgents.size())
		);

		if (!evtcAgents.empty())
		{
			ImGui::Text("Player Agents:");

			unsigned int playerIndex = 0;

			for (const EVTCAgent& agent : evtcAgents)
			{
				// EVTC rule:
				// elite == 0xFFFFFFFF = NPC/gadget
				// anything else = player
				if (agent.elite == 0xFFFFFFFF)
				{
					continue;
				}

				ImGui::Text(
					"%u: %s | Prof:%u | Elite:%u",
					playerIndex,
					agent.name.c_str(),
					agent.profession,
					agent.elite
				);

				++playerIndex;
			}
		}
			}
}