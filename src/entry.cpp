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

/* proto */
void AddonLoad(AddonAPI_t* aApi);
void AddonUnload();
void AddonRender();
void AddonOptions();

/* globals */
AddonDefinition_t AddonDef  = {};
HMODULE hSelf               = nullptr;
AddonAPI_t* APIDefs         = nullptr;
NexusLinkData_t* NexusLink  = nullptr;
Mumble::Data* MumbleLink    = nullptr;
MetricRegistry metricRegistry;
HUDWindow hudWindow;
ConfigManager configManager;


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
	configManager.Initialize();

	// Add an options window and a regular render callback
	APIDefs->GUI_Register(RT_Render, AddonRender);
	APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);

	APIDefs->Log(LOGL_DEBUG, "MetricHUD", "My <c=#00ff00>first addon</c> loaded successfully.");
}

///----------------------------------------------------------------------------------------------------
/// AddonUnload:
/// 	Everything you registered in AddonLoad, you should "undo" here.
///----------------------------------------------------------------------------------------------------
void AddonUnload()
{
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
void AddonRender()
{
	static float combatTime = 0.0f;

	metricRegistry.SetFPS(ImGui::GetIO().Framerate);

	if (MumbleLink != nullptr && MumbleLink->Context.IsInCombat)
	{
		combatTime += ImGui::GetIO().DeltaTime;
	}
	else
	{
		combatTime = 0.0f;
	}
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
	}

	metricRegistry.SetMetricValue(MetricID::CombatTime, combatTime);

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

	MetricDefinition* fpsMetric = metricRegistry.GetMetric(MetricID::FPS);

	if (fpsMetric != nullptr)
	{
		bool fpsEnabled = fpsMetric->enabled;

		if (ImGui::Checkbox("Show FPS", &fpsEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::FPS, fpsEnabled);
		}
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
	MetricDefinition* pingMetric = metricRegistry.GetMetric(MetricID::Ping);

	if (pingMetric != nullptr)
	{
		bool pingEnabled = pingMetric->enabled;

		if (ImGui::Checkbox("Show Ping", &pingEnabled))
		{
			metricRegistry.SetMetricEnabled(MetricID::Ping, pingEnabled);
		}
	}

	ImGui::Separator();

	ImGui::Text("Version 0.1.0");
	ImGui::TextDisabled("Settings will be added during development.");
}