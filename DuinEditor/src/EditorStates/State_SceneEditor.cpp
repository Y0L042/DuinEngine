#include "State_SceneEditor.h"
#include "Singleton.h"
#include "EditorCommands/SceneTreeCommands.h"
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Assets/AssetsModule.h>
#include <Duin/IO/Filesystem.h>
#include <Duin/IO/VirtualFilesystem.h>
#include <Duin/IO/IOStream.h>
#include <Duin/IO/VirtualIOStream.h>
#include <rfl/json.hpp>
#include <Duin/Assets/AssetRef.h>

const std::string DEBUG_data = R"({
	"version": 2,
	"refs": [
		{
			"uuid": "0xC011E1BE3B65B8CA",
			"rPath": "assets/item_11",
			"fileType": "FS_FILETYPE_AUDIO_EXT",
			"fileExt": "FS_FILEEXT_JPEG"
		},
		{
			"uuid": "0x2A74A270C25EDC46",
			"rPath": "assets/item_5",
			"fileType": "FS_FILETYPE_TEXT_EXT",
			"fileExt": "FS_FILEEXT_CFG"
		},
		{
			"uuid": "0xC4C5CDE8D305C517",
			"rPath": "assets/item_4",
			"fileType": "FS_FILETYPE_MODEL_EXT",
			"fileExt": "FS_FILEEXT_OGG"
		},
		{
			"uuid": "0xB13EBE8D0F09722E",
			"rPath": "assets/item_0",
			"fileType": "FS_FILETYPE_TEXT_EXT",
			"fileExt": "FS_FILEEXT_WMA"
		},
		{
			"uuid": "0x410800E0962553AF",
			"rPath": "assets/item_1",
			"fileType": "FS_FILETYPE_MODEL_EXT",
			"fileExt": "FS_FILEEXT_NULL"
		},
		{
			"uuid": "0x179BB5BF1E09AC62",
			"rPath": "assets/item_2",
			"fileType": "FS_FILETYPE_TEXT_EXT",
			"fileExt": "FS_FILEEXT_MKV"
		},
		{
			"uuid": "0xD35877B4D2AEF162",
			"rPath": "assets/item_7",
			"fileType": "FS_FILETYPE_IMAGE_EXT",
			"fileExt": "FS_FILEEXT_LUA"
		},
		{
			"uuid": "0x85ABDAB11A7E3E00",
			"rPath": "assets/item_6",
			"fileType": "FS_FILETYPE_IMAGE_EXT",
			"fileExt": "FS_FILEEXT_MPEG"
		},
		{
			"uuid": "0x4CB1CC044A2101A0",
			"rPath": "assets/item_3",
			"fileType": "FS_FILETYPE_INVALID_EXT",
			"fileExt": "FS_FILEEXT_TIFF"
		},
		{
			"uuid": "0x97B171270A66798E",
			"rPath": "assets/item_12",
			"fileType": "FS_FILETYPE_MODEL_EXT",
			"fileExt": "FS_FILEEXT_BLEND"
		},
		{
			"uuid": "0xDD713A66D9FACD91",
			"rPath": "assets/item_8",
			"fileType": "FS_FILETYPE_IMAGE_EXT",
			"fileExt": "FS_FILEEXT_CSV"
		},
		{
			"uuid": "0x76A853AE110E737B",
			"rPath": "assets/item_9",
			"fileType": "FS_FILETYPE_VIDEO_EXT",
			"fileExt": "FS_FILEEXT_MB"
		},
		{
			"uuid": "0x89C755F0E398A84B",
			"rPath": "assets/item_14",
			"fileType": "FS_FILETYPE_AUDIO_EXT",
			"fileExt": "FS_FILEEXT_CFG"
		},
		{
			"uuid": "0xAEDCD136592455C2",
			"rPath": "assets/item_10",
			"fileType": "FS_FILETYPE_VIDEO_EXT",
			"fileExt": "FS_FILEEXT_BMP"
		},
		{
			"uuid": "0x7D360065E083C08F",
			"rPath": "assets/item_13",
			"fileType": "FS_FILETYPE_VIDEO_EXT",
			"fileExt": "FS_FILEEXT_AAC"
		}
	]
}
)";

void State_SceneEditor::Enter()
{
    InitializeManagers();
    CreateUIObjects();
    SetupInput();

    ProcessProject(Singleton::GetActiveProject());
    UpdateAssetIndex();

    duin::AssetManager::Get().CatalogueAssets();
}

void State_SceneEditor::OnEvent(duin::Event e)
{
}

void State_SceneEditor::Update(double delta)
{
}

void State_SceneEditor::PhysicsUpdate(double delta)
{
    if (duin::IsInputActionTriggered("EditorRedo"))
    {
        commandManager->Redo();
    }
    else if (duin::IsInputActionTriggered("EditorUndo"))
    {
        commandManager->Undo();
    }

    if (duin::IsInputActionTriggered("TestA"))
    {
        DN_INFO("Input TestA held.");
    }
}

void State_SceneEditor::Draw()
{
}

void State_SceneEditor::DrawUI()
{
}

void State_SceneEditor::Exit()
{
}

void State_SceneEditor::InitializeManagers()
{
    fileManager = std::make_shared<FileManager>();
    sceneManager = std::make_shared<SceneManager>();
    auto scene = sceneManager->GetActiveScene();
}

void State_SceneEditor::ProcessProject(Project project)
{
    duin::fs::SetWorkspacePath(project.GetPathAsString());
    fileManager->SetRootPath(project.GetPathAsString());
    fileManager->BuildFileSystemTree();
    signals.onUpdateFileManager.Emit(fileManager);
}

void State_SceneEditor::CreateUIObjects()
{
    uiObjects.sceneTabs = CreateChildObject<SceneTabs>();
    uiObjects.sceneTree = CreateChildObject<SceneTree>();
    uiObjects.sceneViewport = CreateChildObject<SceneViewport>();
    uiObjects.fileTree = CreateChildObject<FileTree>();
    uiObjects.entityProperties = CreateChildObject<EntityProperties>();

    ConnectSignals();

    uiObjects.entityProperties->SetCommandManager(commandManager);
}

void State_SceneEditor::ConnectSignals()
{
    std::shared_ptr<duin::ScopedConnection> conn = nullptr;

    // --- onSetActiveScene ---
    // Scoped
    // Local activeScene handle
    onSetActiveSceneHandle =
        signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) { CacheActiveScene(scene); });

    // set sceneViewport activeScene
    conn = signals.onSetActiveScene.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneViewport->CacheActiveScene(scene); });
    uiObjects.sceneViewport->AddConnectionHandle(conn);

    // Set entityProperties activeScene
    conn = signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) {
        uiObjects.entityProperties->CacheActiveScene(scene);
        uiObjects.entityProperties->SetFocusedEntity(duin::Entity()); // clear
    });
    uiObjects.entityProperties->AddConnectionHandle(conn);

    // Set sceneTree activeScene
    conn = signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) {
        DN_INFO("onSetActiveScene - sceneTree cache active scene");
        uiObjects.sceneTree->CacheActiveScene(scene);
    });
    uiObjects.sceneTree->AddConnectionHandle(conn);

    conn = signals.onSetSceneFromFile.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneTabs->AddTab(scene); });
    uiObjects.sceneTabs->AddConnectionHandle(conn);

    // --- SceneTabs ---
    // Unscoped
    // Set sceneManager activeScene from sceneTabs
    uiObjects.sceneTabs->onSceneTabSelect.Connect([&](std::weak_ptr<Scene> scene) {
        if (!scene.expired())
        {
            auto handle = sceneManager->SetActiveScene(scene.lock()->uuid);

            if (handle)
            {
                signals.onSetActiveScene.Emit(sceneManager->GetScene(handle));
            }
        }
    });

    // --- FileTree ---
    // Scoped
    // Open selected packedScene in /
    conn = uiObjects.fileTree->onPackedSceneFileSelect.ConnectScoped([&](std::weak_ptr<FSNode> sceneFile) {
        if (sceneFile.expired())
        {
            DN_WARN("onPackedSceneFileSelect - Scenefile expired!");
            return;
        }
        auto sf = sceneFile.lock();
        DN_INFO("onPackedSceneFileSelect - Scenefile selected: {}", sf->name);
        EnsureInstantiatedScene(sf);
        DN_INFO("onPackedSceneFileSelect - Scene opened in tab: {}", sf->name);
    });
    uiObjects.sceneTabs->AddConnectionHandle(conn);

    // --- SceneTree ---
    // Scoped
    conn = uiObjects.sceneTree->onSelectEntity.ConnectScoped(
        [&](duin::Entity e) { uiObjects.entityProperties->SetFocusedEntity(e); });
    uiObjects.entityProperties->AddConnectionHandle(conn);

    // Unscoped
    uiObjects.sceneTree->onAddEntity.Connect([&]() {
        auto scene = cachedActiveScene.lock();
        if (scene && scene->GetWorld().lock())
        {
            commandManager->Do(std::make_shared<AddEntityCommand>(scene->GetWorld().lock()));
        }
    });

    uiObjects.sceneTree->onAddChildEntity.Connect([&](duin::Entity parent) {
        auto scene = cachedActiveScene.lock();
        if (scene && scene->GetWorld().lock())
        {
            commandManager->Do(std::make_shared<AddChildEntityCommand>(scene->GetWorld().lock(), parent));
        }
    });

    uiObjects.sceneTree->onRemoveEntity.Connect([&](duin::Entity e) {
        auto scene = cachedActiveScene.lock();
        if (scene && scene->GetWorld().lock())
        {
            commandManager->Do(std::make_shared<RemoveEntityCommand>(scene->GetWorld().lock(), e));
        }
    });

    uiObjects.sceneTree->onReparentEntity.Connect([&](duin::Entity child, duin::Entity newParent) {
        auto scene = cachedActiveScene.lock();
        if (scene && scene->GetWorld().lock())
        {
            commandManager->Do(std::make_shared<ReparentEntityCommand>(scene->GetWorld().lock(), child, newParent));
        }
    });

    uiObjects.sceneTree->onReparentEntityToRoot.Connect([&](duin::Entity child) {
        auto scene = cachedActiveScene.lock();
        if (scene && scene->GetWorld().lock())
        {
            commandManager->Do(std::make_shared<ReparentEntityToRootCommand>(scene->GetWorld().lock(), child));
        }
    });

    // --- onUpdateFileManager ---
    // Scoped
    conn = signals.onUpdateFileManager.ConnectScoped(
        [&](std::shared_ptr<FileManager> fm) { uiObjects.fileTree->SetFileManager(fm); });
    uiObjects.fileTree->AddConnectionHandle(conn);
}

void State_SceneEditor::SetupInput()
{
    duin::AddInputActionBinding("EditorUndo", DN_KEYBOARD_01, DN_SCANCODE_Z, DN_KEVENT_PRESSED, DN_KEY_MOD_LCTRL);
    duin::AddInputActionBinding("EditorRedo", DN_KEYBOARD_01, DN_SCANCODE_Z, DN_KEVENT_PRESSED,
                                DN_KEY_MOD_LCTRL | DN_KEY_MOD_LSHIFT);
    duin::AddInputActionBinding("TestA", DN_KEYBOARD_01, DN_SCANCODE_Z, DN_KEVENT_HELD);
}

void State_SceneEditor::UpdateAssetIndex()
{
    std::string idxPath = "wrk://.dncfg/assetIndex.json";

    // Read assetIndex
    duin::JSONValue v = duin::JSONValue::ParseFromFile(duin::fs::MapVirtualToSystemPath(idxPath));
    assetIndex.Deserialize(v.Write());

    std::vector<std::shared_ptr<FSNode>> assetFiles;

    // Check existance cached AssetRefs in assetIndex
    // TODO give user a warning/option to resolve missing dependencies
    std::vector<duin::UUID> missingAssets;
    for (const auto &asset : assetIndex.GetIndex())
    {
        if (!duin::vfs::GetPathInfo(asset.second.rPath))
        {
            missingAssets.push_back(asset.first);
        }
    }
    for (const auto &uuid : missingAssets)
    {
        assetIndex.RemoveAsset(uuid);
    }
    missingAssets.clear();

    // Get list of asset files
    fileManager->WalkTree([&assetFiles](std::shared_ptr<FSNode> node) {
        if (node->fileExt == duin::FS_FILEEXT_PSCN)
        {
            assetFiles.push_back(node);
        }
    });

    // For each asset file, check metafile for AssetRef data. Else generate metafile.
    // If has metafile, match and update assetIndex
    for (auto assetFile : assetFiles)
    {
        std::string assetMetaFile = assetFile->path + ".meta";
        if (duin::fs::GetPathInfo(assetMetaFile))
        {
            // Metafile exist
            size_t fileSize = 0;
            char *raw = (char *)duin::io::IOStream::LoadFile(assetMetaFile, &fileSize);
            std::string data(raw);
            duin::AssetRef ref = rfl::json::read<duin::AssetRef>(data).value();
            assetIndex.EnsureAsset(ref);
        }
        else
        {
            // Metafile does not exist
            duin::io::IOStream stream = duin::io::IOStream::FromFile(assetMetaFile, "w");
            std::string rPath =
                duin::vfs::OverridePathPrefix(duin::fs::MapSystemToVirtualPath(assetFile->path), duin::vfs::APPROOT());
            if (rPath.empty())
            {
                DN_FATAL("Invalid rPath {}", rPath);
                return;
            }
            duin::AssetRef ref(rPath, assetFile->fileType, assetFile->fileExt);
            assetIndex.EnsureAsset(ref);
            std::string strRef = rfl::json::write(ref);
            stream.Write(strRef.c_str(), strRef.size());
            stream.Flush();
        }
    }

    std::string out;
    assetIndex.Serialize(out);
    duin::io::IOStream stream = duin::io::IOStream::FromFile(duin::fs::MapVirtualToSystemPath(idxPath), "w");
    stream.Write(out.c_str(), out.size());
    stream.Flush();
    DN_INFO("UPDATED assetIndex: \n\n\n{}\n\n\n", out);
}

void State_SceneEditor::LoadSceneFromFile(const FSNode *sceneFile)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sceneFile->path);
    sceneManager->LoadSceneFromJSON(v);
}

/*
 * If scene already loaded, set as active.
 * Else load scene, then set as active.
 */
void State_SceneEditor::EnsureInstantiatedScene(std::weak_ptr<FSNode> sceneFile)
{
    if (sceneFile.expired())
    {
        DN_WARN("SceneFile is expired!");
        return;
    }
    auto sf = sceneFile.lock();
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sf->path);
    auto uuid = duin::UUID::FromStringHex(v[duin::PackedScene::TAG_SCENEUUID].GetString());
    if (sceneManager->GetScene(uuid))
    {
        sceneManager->SetActiveScene(uuid);
        signals.onSetActiveScene.Emit(sceneManager->GetActiveScene());
        signals.onSetSceneFromFile.Emit(sceneManager->GetActiveScene());
    }
    else
    {
        auto handle = sceneManager->LoadSceneFromJSON(v, true);
        if (sceneManager->InstantiateScene(handle))
        {
            sceneManager->SetActiveScene(handle);
            signals.onSetActiveScene.Emit(sceneManager->GetActiveScene());
            signals.onSetSceneFromFile.Emit(sceneManager->GetActiveScene());
        }
        else
        {
            DN_WARN("Scene {} failed to instantiate!", handle.ToStrHex());
        }
    }
}

void State_SceneEditor::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    cachedActiveScene = scene;
}
