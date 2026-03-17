#include "SceneBuilder.h"
#include <Duin/Assets/AssetIndex.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Debug/DNAssert.h>
#include <Duin/Assets/FileTypes.h>

#include <string>

static const std::string INDEX_PATH = "";

SceneBuilder::SceneBuilder(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

SceneBuilder::~SceneBuilder()
{
}

void SceneBuilder::Enter()
{
    duin::AssetIndex assetIndex;

    auto &index = assetIndex.GetIndex();
    
    for (int i = 0; i < 150; i++)
    {
        duin::AssetRef aref;
        aref.uuid = duin::UUID(); // default ctor generates random UUID
        aref.hintPath = "assets/item_" + std::to_string(i);
        aref.fileType = static_cast<duin::FileType>(std::rand() % duin::FS_FILETYPE_COUNT);
        aref.fileExt = static_cast<duin::FileExt>(std::rand() % duin::FS_FILEEXT_COUNT);
        assetIndex.AddAsset(aref);
    }

    std::string output;
    assetIndex.Serialize(output);
    DN_INFO(output);

    duin::AssetIndex assetIndex2;
    assetIndex2.Deserialize(output);
    assetIndex2.AddAsset({"assets/item_420", duin::FS_FILETYPE_MODEL_EXT, duin::FS_FILEEXT_BLEND});
    std::string output2;
    assetIndex2.Serialize(output2);
    DN_INFO("\n\n\n{}\n\n\n{}\n\n\n", output, output2);
    //DN_ASSERT(output == output2, "Output_1 {}\n\n\n must match Output_2!");

}

void SceneBuilder::OnEvent(duin::Event e)
{
}

void SceneBuilder::Update(double delta)
{
}

void SceneBuilder::PhysicsUpdate(double delta)
{
}

void SceneBuilder::Draw()
{
}

void SceneBuilder::DrawUI()
{
}

void SceneBuilder::Exit()
{
}

void SceneBuilder::SetPause()
{
}

void SceneBuilder::SetUnpause()
{
}

void SceneBuilder::Tests()
{
}

void SceneBuilder::Test_Misc()
{
}
