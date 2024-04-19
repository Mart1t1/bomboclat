#pragma once

#include <IPluginInterface.h>

#include <Glacier/SGameUpdateEvent.h>

class bomboclat : public IPluginInterface {
public:
    void OnEngineInitialized() override;
    ~bomboclat() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
    void SpawnRepositoryProp(const ZRepositoryID& p_RepositoryId, const bool addToWorld);
    DECLARE_PLUGIN_DETOUR(bomboclat, void, OnLoadScene, ZEntitySceneContext* th, ZSceneData& p_SceneData);


private:
    bool m_ShowMessage = false;

    bool hasItemsAlreadyBeenGiven = false;
};

DEFINE_ZHM_PLUGIN(bomboclat)
