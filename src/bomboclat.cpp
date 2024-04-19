#include "bomboclat.h"

#include <Logging.h>
#include <IconsMaterialDesign.h>
#include <Globals.h>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZScene.h>
#include <Glacier/ZHitman5.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZRepository.h>

#include <Glacier/ZItem.h>
#include <Glacier/ZInventory.h>


void bomboclat::OnEngineInitialized()
{
    Logger::Info("bomboclat has been initialized!");

    // Register a function to be called on every game frame while the game is in play mode.
    const ZMemberDelegate<bomboclat, void(const SGameUpdateEvent&)> s_Delegate(this, &bomboclat::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    // Install a hook to print the name of the scene every time the game loads a new one.
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &bomboclat::OnLoadScene);
}

bomboclat::~bomboclat() {
    // Unregister our frame update function when the mod unloads.
    const ZMemberDelegate<bomboclat, void(const SGameUpdateEvent&)> s_Delegate(this, &bomboclat::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
}


void bomboclat::OnDrawMenu() {
    // Toggle our message when the user presses our button.
    if (ImGui::Button(ICON_MD_LOCAL_FIRE_DEPARTMENT " bomboclat")) {
        m_ShowMessage = !m_ShowMessage;
    }
}

void bomboclat::OnDrawUI(bool p_HasFocus) {
    if (m_ShowMessage) {
        // Show a window for our mod.
        if (ImGui::Begin("bomboclat", &m_ShowMessage)) {
            // Only show these when the window is expanded.
            ImGui::Text("Hello from bomboclat!");
        }
        ImGui::End();
    }
}

void bomboclat::OnFrameUpdate(const SGameUpdateEvent &p_UpdateEvent) {
    // This function is called every frame while the game is in play mode.
    if (!hasItemsAlreadyBeenGiven && GetAsyncKeyState('K'))
    {
        Logger::Debug("adding cx demo block");

        Logger::Debug("Instanciating player ref");
        TEntityRef<ZHitman5> s_LocalHitman;

        Logger::Debug("Instanciating cx repo");
        const ZRepositoryID& p_RepositoryId = ZRepositoryID("8bde9428-eb24-41e1-85fd-9a5a82dd81d7");

        Logger::Debug("Spawning cx");
        SpawnRepositoryProp(p_RepositoryId, true);

        hasItemsAlreadyBeenGiven = true;
        Logger::Debug("Added CX Demo block");
    }


    if (!hasItemsAlreadyBeenGiven && GetAsyncKeyState('L'))
    {
        Logger::Debug("adding cx demo block");

        Logger::Debug("Instanciating player ref");
        TEntityRef<ZHitman5> s_LocalHitman;

        Logger::Debug("Instanciating cx repo");
        const ZRepositoryID& p_RepositoryId = ZRepositoryID("a8a0c154-c36f-413e-8f29-b83a1b7a22f0");

        Logger::Debug("Spawning cx");
        SpawnRepositoryProp(p_RepositoryId, true);
        if (!GetAsyncKeyState(0x10)) // if the SHIFT KEY is pressed, we don't want the boolean to go to false because we want an item to spawn next frame
        {
            hasItemsAlreadyBeenGiven = true;
        }

        Logger::Debug("Added CX Demo block");
    }

    if (!GetAsyncKeyState('L') && !GetAsyncKeyState('K'))
    {
        hasItemsAlreadyBeenGiven = false;
    }
}


void bomboclat::SpawnRepositoryProp(const ZRepositoryID& p_RepositoryId, const bool addToWorld)
{
    TEntityRef<ZHitman5> s_LocalHitman;
    Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

    if (!s_LocalHitman)
    {
        Logger::Debug("No local hitman");
        return;
    }

    if (!addToWorld)
    {
        const TArray<TEntityRef<ZCharacterSubcontroller>>* s_Controllers = &s_LocalHitman.m_pInterfaceRef->m_pCharacter.m_pInterfaceRef->m_rSubcontrollerContainer.m_pInterfaceRef->m_aReferencedControllers;
        auto* s_Inventory = static_cast<ZCharacterSubcontrollerInventory*>(s_Controllers->operator[](6).m_pInterfaceRef);

        TArray<ZRepositoryID> s_ModifierIds;
        Functions::ZCharacterSubcontrollerInventory_AddDynamicItemToInventory->Call(s_Inventory, p_RepositoryId, "", &s_ModifierIds, 2);

        return;
    }

    const auto s_Scene = Globals::Hitman5Module->m_pEntitySceneContext->m_pScene;

    if (!s_Scene)
    {
        Logger::Debug("Scene not loaded.");
        return;
    }

    const auto s_ID = ResId<"[modules:/zitemspawner.class].pc_entitytype">;
    const auto s_ID2 = ResId<"[modules:/zitemrepositorykeyentity.class].pc_entitytype">;

    TResourcePtr<ZTemplateEntityFactory> s_Resource, s_Resource2;

    Globals::ResourceManager->GetResourcePtr(s_Resource, s_ID, 0);
    Globals::ResourceManager->GetResourcePtr(s_Resource2, s_ID2, 0);

    Logger::Debug("Resource: {} {}", s_Resource.m_nResourceIndex, fmt::ptr(s_Resource.GetResource()));

    if (!s_Resource)
    {
        Logger::Debug("Resource is not loaded.");
        return;
    }

    ZEntityRef s_NewEntity, s_NewEntity2;

    Functions::ZEntityManager_NewEntity->Call(Globals::EntityManager, s_NewEntity, "", s_Resource, s_Scene.m_ref, nullptr, -1);
    Functions::ZEntityManager_NewEntity->Call(Globals::EntityManager, s_NewEntity2, "", s_Resource2, s_Scene.m_ref, nullptr, -1);

    if (!s_NewEntity)
    {
        Logger::Debug("Failed to spawn entity.");
        return;
    }

    if (!s_NewEntity2)
    {
        Logger::Debug("Failed to spawn entity2.");
        return;
    }

    const auto s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();

    const auto s_ItemSpawner = s_NewEntity.QueryInterface<ZItemSpawner>();

    SMatrix destMat= s_HitmanSpatialEntity->GetWorldMatrix();

    Logger::Debug("P Pos: {} {} {} {}", destMat.XAxis, destMat.YAxis, destMat.ZAxis, destMat.Trans);

    destMat.Trans.x += 1.0f;
    destMat.Trans.z += 1.0f;

    s_ItemSpawner->m_ePhysicsMode = ZItemSpawner::EPhysicsMode::EPM_DYNAMIC;
    s_ItemSpawner->m_rMainItemKey.m_ref = s_NewEntity2;
    s_ItemSpawner->m_rMainItemKey.m_pInterfaceRef = s_NewEntity2.QueryInterface<ZItemRepositoryKeyEntity>();
    s_ItemSpawner->m_rMainItemKey.m_pInterfaceRef->m_RepositoryId = p_RepositoryId;
    s_ItemSpawner->m_bUsePlacementAttach = false;
    s_ItemSpawner->SetWorldMatrix(destMat);

    Functions::ZItemSpawner_RequestContentLoad->Call(s_ItemSpawner);
}

DEFINE_PLUGIN_DETOUR(bomboclat, void, OnLoadScene, ZEntitySceneContext* th, ZSceneData& p_SceneData) {
    Logger::Debug("Loading scene: {}", p_SceneData.m_sceneName);
    return HookResult<void>(HookAction::Continue());
}

DECLARE_ZHM_PLUGIN(bomboclat);
