#pragma once

#include <ToolboxPlugin.h>

class SmartFollow : public ToolboxPlugin {
public:
    const char* Name() const override { return "SmartFollow"; }
    const char* Icon() const override { return ICON_FA_LOCK_OPEN; }

    virtual void Update(float) override;
    void Initialize(ImGuiContext*, ImGuiAllocFns, HMODULE) override;
    void SignalTerminate() override;
    bool CanTerminate() override;
    void Terminate() override;

private:
    struct PlayerMapping
    {
        uint32_t id;
        uint32_t party_idx;
    };
};
