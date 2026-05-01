#include <doctest.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/modules/moduleDNLog/Module_DnLog.h>
#include <Duin/Script/modules/moduleGameObject/Module_DnGameObject.h>
#include <Duin/Core/Application.h>

static const std::string ARTIFACTS_DIR = "./artifacts";

// Helper: write a .das file under ARTIFACTS_DIR and return its path.
static std::string WriteDas(const std::string &name, const std::string &src)
{
    std::string path = ARTIFACTS_DIR + "/" + name;
    FILE *f = fopen(path.c_str(), "w");
    if (f) { fwrite(src.data(), 1, src.size(), f); fclose(f); }
    return path;
}

namespace TestGameScript
{

TEST_SUITE("GameScript")
{

}

} // namespace TestGameScript
