#include "MultiboxUtils.h"

#include <GWCA/GWCA.h>

#include <GWCA/Utilities/Hooker.h>

#include <GWCA/GameEntities/Agent.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/CtoSMgr.h>
#include <GWCA/Managers/GameThreadMgr.h>
#include <GWCA/GameEntities/Party.h>
#include <GWCA/Managers/PartyMgr.h>
#include <GWCA/GameContainers/GamePos.h>
#include <GWCA/Managers/MapMgr.h>



#define GAME_CMSG_INTERACT_GADGET                   (0x004F) // 79
#define GAME_CMSG_SEND_SIGNPOST_DIALOG              (0x0051) // 81

bool IsFollowing = false;


DLLAPI ToolboxPlugin* ToolboxPluginInstance()
{
    static SmartFollow instance;
    return &instance;
}

void SmartFollow::Initialize(ImGuiContext* ctx, const ImGuiAllocFns fns, const HMODULE toolbox_dll)
{
    ToolboxPlugin::Initialize(ctx, fns, toolbox_dll);

    GW::Initialize();
    
    GW::Chat::CreateCommand(L"follow", [](const wchar_t*, int, const LPWSTR*) {
        GW::GameThread::Enqueue([] {
            IsFollowing = !IsFollowing;
            if (IsFollowing) {
                WriteChat(GW::Chat::CHANNEL_GWCA1, L"Follow Activated", L"SmartFollow");
            }
            else {
                WriteChat(GW::Chat::CHANNEL_GWCA1, L"Follow Deactivated", L"SmartFollow");
            }
        });
    });
    WriteChat(GW::Chat::CHANNEL_GWCA1, L"Initialized\nUse /follow to toggle Follow Party Leader", L"SmartFollow");
}

void SmartFollow::Update(float)
{
    uint32_t tLeaderid = 0;

    if ((GW::Map::GetInstanceType() == GW::Constants::InstanceType::Explorable) && (GW::PartyMgr::GetIsPartyLoaded())) {
        const auto tSelf = GW::Agents::GetPlayerAsAgentLiving();
        const GW::PartyInfo* Party = GW::PartyMgr::GetPartyInfo();
        if ((tSelf) && ((Party) && (Party->players[0].connected()))) {
            tLeaderid = GW::Agents::GetAgentIdByLoginNumber(Party->players[0].login_number);
            if (tLeaderid) {
                auto tLeader = GW::Agents::GetAgentByID(tLeaderid);
                const auto tLeaderliving = tLeader->GetAsAgentLiving();
                if (tLeaderliving) {
                    const auto dist = GW::GetDistance(tSelf->pos, tLeaderliving->pos);
                    if ((IsFollowing) && 
                        (!tSelf->GetIsMoving()) && 
                        (!tSelf->GetIsCasting()) && 
                        (dist > GW::Constants::Range::Area)) {
                        GW::Agents::InteractAgent(tLeader, false);
                    };
                };
            }
        };
    };
}

void SmartFollow::SignalTerminate() 
{
    GW::Chat::DeleteCommand(L"follow");
    GW::DisableHooks();
}

bool SmartFollow::CanTerminate()
{
    return GW::HookBase::GetInHookCount() == 0;
}

void SmartFollow::Terminate()
{
    ToolboxPlugin::Terminate();
    GW::Terminate();
}


