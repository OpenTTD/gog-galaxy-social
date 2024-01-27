#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif /* WIN32 */

#include <string>

#include "galaxy/GalaxyApi.h"
#include "openttd_social_integration_api/openttd_social_integration_api.h"

static const std::string PLUGIN_SOCIAL_PLATFORM = "GOG Galaxy";
static const std::string PLUGIN_NAME = "GOG Galaxy Integration";
static const std::string PLUGIN_VERSION = "dev";

static std::string _last_status = "";
static bool _is_authorized = false;
static bool _authorized_failed = false;

class GOGAuthListener : public galaxy::api::IAuthListener {
public:
    void OnAuthSuccess()
    {
        _is_authorized = true;
    }

    void OnAuthFailure(galaxy::api::IAuthListener::FailureReason)
    {
        _authorized_failed = true;
    }

    void OnAuthLost()
    {
        _authorized_failed = true;
    }
};
GOGAuthListener _auth_listener;

bool run_callbacks()
{
    /* If at any point authorization with GOG Galaxy fails, unload the plugin. */
    if (_authorized_failed) {
        return false;
    }

    galaxy::api::ProcessData();

    /* We need to delay setting our presence the first time till we are authenticated. */
    static bool first_time = true;
    if (_is_authorized && first_time) {
        first_time = false;
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }

    return true;
}

void shutdown()
{
    galaxy::api::Friends()->ClearRichPresence();
    galaxy::api::ProcessData();

    galaxy::api::Shutdown();
}

void event_enter_main_menu()
{
    _last_status = "In Main Menu";
    if (_is_authorized) {
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }
}

void event_enter_scenario_editor(unsigned int map_width, unsigned int map_height)
{
    _last_status = "Scenario Editor - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    if (_is_authorized) {
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }
}

void event_enter_singleplayer(unsigned int map_width, unsigned int map_height)
{
    _last_status = "Singleplayer - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    if (_is_authorized) {
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }
}

void event_enter_multiplayer(unsigned int map_width, unsigned int map_height)
{
    _last_status = "Multiplayer - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    if (_is_authorized) {
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }
}

void event_joining_multiplayer()
{
    _last_status = "Joining Multiplayer";
    if (_is_authorized) {
        galaxy::api::Friends()->SetRichPresence("status", _last_status.c_str());
    }
}

extern "C" DLL_EXPORT void SocialIntegration_v1_GetInfo(OpenTTD_SocialIntegration_v1_PluginInfo *plugin_info)
{
    static OpenTTD_SocialIntegration_v1_PluginInfo info {
        .social_platform = PLUGIN_SOCIAL_PLATFORM.c_str(),
        .name = PLUGIN_NAME.c_str(),
        .version = PLUGIN_VERSION.c_str(),
    };

    *plugin_info = info;
}

extern "C" DLL_EXPORT int SocialIntegration_v1_Init(OpenTTD_SocialIntegration_v1_PluginApi *plugin_api, const OpenTTD_SocialIntegration_v1_OpenTTDInfo *)
{
    static OpenTTD_SocialIntegration_v1_PluginApi api {
        .shutdown = shutdown,
        .run_callbacks = run_callbacks,

        .event_enter_main_menu = event_enter_main_menu,
        .event_enter_scenario_editor = event_enter_scenario_editor,
        .event_enter_singleplayer = event_enter_singleplayer,
        .event_enter_multiplayer = event_enter_multiplayer,
        .event_joining_multiplayer = event_joining_multiplayer,
    };

    *plugin_api = api;

    galaxy::api::InitOptions initOptions(
        GOG_GALAXY_CLIENT_ID,
        GOG_GALAXY_CLIENT_SECRET
    );

    galaxy::api::Init(initOptions);
    if (galaxy::api::GetError()) {
        galaxy::api::Shutdown();
        return OTTD_SOCIAL_INTEGRATION_V1_INIT_FAILED;
    }

    galaxy::api::User()->SignInGalaxy(false, 15, &_auth_listener);
    if (galaxy::api::GetError()) {
        galaxy::api::Shutdown();
        return OTTD_SOCIAL_INTEGRATION_V1_INIT_FAILED;
    }

    return OTTD_SOCIAL_INTEGRATION_V1_INIT_SUCCESS;
}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    /* Ensure our library remains loaded. */
    return TRUE;
}
#endif /* WIN32 */
