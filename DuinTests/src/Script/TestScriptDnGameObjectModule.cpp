#include <doctest.h>
#include <Duin/Script/Script.h>
#include <Duin/Script/DNLog/Module_DnLog.h>
#include <Duin/Script/GameObject/Module_DnGameObject.h>
#include <Duin/Script/Doctest/Module_Doctest.h>

static const std::string ARTIFACTS_DIR = "./artifacts";

// Helper: write a .das file under ARTIFACTS_DIR and return its path.
static std::string WriteDas(const std::string &name, const std::string &src)
{
    std::string path = ARTIFACTS_DIR + "/" + name;
    FILE *f = fopen(path.c_str(), "w");
    if (f)
    {
        fwrite(src.data(), 1, src.size(), f);
        fclose(f);
    }
    return path;
}

namespace TestScriptDnGameObjectModule
{

// ──────────────────────────────────────────────────────────────────────────────
// Basic lifecycle
// ──────────────────────────────────────────────────────────────────────────────

TEST_SUITE("DnGameObjectModule")
{
    TEST_CASE("dn_gameobject_init creates a C++ handle")
    {
        auto path = WriteDas("_test_dngobj_init.das", "options gen2\n"
                                                      "options rtti\n"
                                                      "require dn_gameobject\n"
                                                      "require rtti\n"
                                                      "\n"
                                                      "class MyObj : dn_GameObject {\n"
                                                      "    def override _init() {}\n"
                                                      "    def override _ready() {}\n"
                                                      "    def override _update(delta : double) {}\n"
                                                      "    def override _physics_update(delta : double) {}\n"
                                                      "    def override _draw() {}\n"
                                                      "    def override _draw_ui() {}\n"
                                                      "    def override _debug() {}\n"
                                                      "}\n"
                                                      "\n"
                                                      "[export] def test_init() {\n"
                                                      "    var obj = new MyObj()\n"
                                                      "    obj |> dn_gameobject_init()\n"
                                                      "    assert(obj._handle != null)\n"
                                                      "    obj |> dn_gameobject_finalize()\n"
                                                      "    assert(obj._handle == null)\n"
                                                      "    unsafe { delete obj; }\n"
                                                      "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_init");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    TEST_CASE("dn_gameobject_finalize is a no-op when _handle is already null")
    {
        auto path =
            WriteDas("_test_dngobj_finalize_null.das", "options gen2\n"
                                                       "options rtti\n"
                                                       "require dn_gameobject\n"
                                                       "require rtti\n"
                                                       "\n"
                                                       "class MyObj : dn_GameObject {\n"
                                                       "    def override _init() {}\n"
                                                       "    def override _ready() {}\n"
                                                       "    def override _update(delta : double) {}\n"
                                                       "    def override _physics_update(delta : double) {}\n"
                                                       "    def override _draw() {}\n"
                                                       "    def override _draw_ui() {}\n"
                                                       "    def override _debug() {}\n"
                                                       "}\n"
                                                       "\n"
                                                       "[export] def test_finalize_null() {\n"
                                                       "    var obj = new MyObj()\n"
                                                       "    // _handle is already null — finalize should not crash\n"
                                                       "    obj |> dn_gameobject_finalize()\n"
                                                       "    assert(obj._handle == null)\n"
                                                       "    unsafe { delete obj; }\n"
                                                       "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_finalize_null");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    // ──────────────────────────────────────────────────────────────────────────
    // Child hierarchy
    // ──────────────────────────────────────────────────────────────────────────

    TEST_CASE("dn_gameobject_add_child attaches a child and reports correct count")
    {
        auto path = WriteDas("_test_dngobj_add_child.das", "options gen2\n"
                                                           "options rtti\n"
                                                           "require dn_gameobject\n"
                                                           "require rtti\n"
                                                           "\n"
                                                           "class Node : dn_GameObject {\n"
                                                           "    def override _init() {}\n"
                                                           "    def override _ready() {}\n"
                                                           "    def override _update(delta : double) {}\n"
                                                           "    def override _physics_update(delta : double) {}\n"
                                                           "    def override _draw() {}\n"
                                                           "    def override _draw_ui() {}\n"
                                                           "    def override _debug() {}\n"
                                                           "}\n"
                                                           "\n"
                                                           "[export] def test_add_child() {\n"
                                                           "    var parent = new Node()\n"
                                                           "    var child  = new Node()\n"
                                                           "    parent |> dn_gameobject_init()\n"
                                                           "    child  |> dn_gameobject_init()\n"
                                                           "    parent |> dn_gameobject_add_child(child)\n"
                                                           "    assert((parent |> dn_get_children_count()) == 1)\n"
                                                           "    parent |> dn_gameobject_finalize()\n"
                                                           "    child  |> dn_gameobject_finalize()\n"
                                                           "    unsafe { delete parent; }\n"
                                                           "    unsafe { delete child; }\n"
                                                           "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_add_child");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    TEST_CASE("dn_gameobject_remove_child detaches a child")
    {
        auto path = WriteDas("_test_dngobj_remove_child.das", "options gen2\n"
                                                              "options rtti\n"
                                                              "require dn_gameobject\n"
                                                              "require rtti\n"
                                                              "\n"
                                                              "class Node : dn_GameObject {\n"
                                                              "    def override _init() {}\n"
                                                              "    def override _ready() {}\n"
                                                              "    def override _update(delta : double) {}\n"
                                                              "    def override _physics_update(delta : double) {}\n"
                                                              "    def override _draw() {}\n"
                                                              "    def override _draw_ui() {}\n"
                                                              "    def override _debug() {}\n"
                                                              "}\n"
                                                              "\n"
                                                              "[export] def test_remove_child() {\n"
                                                              "    var parent = new Node()\n"
                                                              "    var child  = new Node()\n"
                                                              "    parent |> dn_gameobject_init()\n"
                                                              "    child  |> dn_gameobject_init()\n"
                                                              "    parent |> dn_gameobject_add_child(child)\n"
                                                              "    assert((parent |> dn_get_children_count()) == 1)\n"
                                                              "    parent |> dn_gameobject_remove_child(child)\n"
                                                              "    assert((parent |> dn_get_children_count()) == 0)\n"
                                                              "    parent |> dn_gameobject_finalize()\n"
                                                              "    child  |> dn_gameobject_finalize()\n"
                                                              "    unsafe { delete parent; }\n"
                                                              "    unsafe { delete child; }\n"
                                                              "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_remove_child");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    TEST_CASE("dn_get_children_count returns 0 for a leaf node")
    {
        auto path = WriteDas("_test_dngobj_count_zero.das", "options gen2\n"
                                                            "options rtti\n"
                                                            "require dn_gameobject\n"
                                                            "require rtti\n"
                                                            "\n"
                                                            "class Node : dn_GameObject {\n"
                                                            "    def override _init() {}\n"
                                                            "    def override _ready() {}\n"
                                                            "    def override _update(delta : double) {}\n"
                                                            "    def override _physics_update(delta : double) {}\n"
                                                            "    def override _draw() {}\n"
                                                            "    def override _draw_ui() {}\n"
                                                            "    def override _debug() {}\n"
                                                            "}\n"
                                                            "\n"
                                                            "[export] def test_count_zero() {\n"
                                                            "    var node = new Node()\n"
                                                            "    node |> dn_gameobject_init()\n"
                                                            "    assert((node |> dn_get_children_count()) == 0)\n"
                                                            "    node |> dn_gameobject_finalize()\n"
                                                            "    unsafe { delete node; }\n"
                                                            "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_count_zero");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    // ──────────────────────────────────────────────────────────────────────────
    // Enable / disable callbacks
    // ──────────────────────────────────────────────────────────────────────────

    TEST_CASE("dn_enable disables all callbacks and query functions reflect that")
    {
        auto path = WriteDas("_test_dngobj_enable_all.das", "options gen2\n"
                                                            "options rtti\n"
                                                            "require dn_gameobject\n"
                                                            "require rtti\n"
                                                            "\n"
                                                            "class Node : dn_GameObject {\n"
                                                            "    def override _init() {}\n"
                                                            "    def override _ready() {}\n"
                                                            "    def override _update(delta : double) {}\n"
                                                            "    def override _physics_update(delta : double) {}\n"
                                                            "    def override _draw() {}\n"
                                                            "    def override _draw_ui() {}\n"
                                                            "    def override _debug() {}\n"
                                                            "}\n"
                                                            "\n"
                                                            "[export] def test_enable_all() {\n"
                                                            "    var node = new Node()\n"
                                                            "    node |> dn_gameobject_init()\n"
                                                            "    // disable everything\n"
                                                            "    node |> dn_enable(false)\n"
                                                            "    assert(!(node |> dn_is_on_event_enabled()))\n"
                                                            "    assert(!(node |> dn_is_update_enabled()))\n"
                                                            "    assert(!(node |> dn_is_physics_update_enabled()))\n"
                                                            "    assert(!(node |> dn_is_draw_enabled()))\n"
                                                            "    assert(!(node |> dn_is_draw_ui_enabled()))\n"
                                                            "    assert(!(node |> dn_is_debug_enabled()))\n"
                                                            "    // re-enable everything\n"
                                                            "    node |> dn_enable(true)\n"
                                                            "    assert(node |> dn_is_on_event_enabled())\n"
                                                            "    assert(node |> dn_is_update_enabled())\n"
                                                            "    assert(node |> dn_is_physics_update_enabled())\n"
                                                            "    assert(node |> dn_is_draw_enabled())\n"
                                                            "    assert(node |> dn_is_draw_ui_enabled())\n"
                                                            "    assert(node |> dn_is_debug_enabled())\n"
                                                            "    node |> dn_gameobject_finalize()\n"
                                                            "    unsafe { delete node; }\n"
                                                            "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_enable_all");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    TEST_CASE("Individual enable functions toggle independently")
    {
        auto path = WriteDas("_test_dngobj_enable_individual.das",
                             "options gen2\n"
                             "options rtti\n"
                             "require dn_gameobject\n"
                             "require rtti\n"
                             "\n"
                             "class Node : dn_GameObject {\n"
                             "    def override _init() {}\n"
                             "    def override _ready() {}\n"
                             "    def override _update(delta : double) {}\n"
                             "    def override _physics_update(delta : double) {}\n"
                             "    def override _draw() {}\n"
                             "    def override _draw_ui() {}\n"
                             "    def override _debug() {}\n"
                             "}\n"
                             "\n"
                             "[export] def test_enable_individual() {\n"
                             "    var node = new Node()\n"
                             "    node |> dn_gameobject_init()\n"
                             "\n"
                             "    node |> dn_enable_update(false)\n"
                             "    assert(!(node |> dn_is_update_enabled()))\n"
                             "    assert(node |> dn_is_on_event_enabled()) // others unaffected\n"
                             "\n"
                             "    node |> dn_enable_physics_update(false)\n"
                             "    assert(!(node |> dn_is_physics_update_enabled()))\n"
                             "\n"
                             "    node |> dn_enable_draw(false)\n"
                             "    assert(!(node |> dn_is_draw_enabled()))\n"
                             "\n"
                             "    node |> dn_enable_draw_ui(false)\n"
                             "    assert(!(node |> dn_is_draw_ui_enabled()))\n"
                             "\n"
                             "    node |> dn_enable_debug(false)\n"
                             "    assert(!(node |> dn_is_debug_enabled()))\n"
                             "\n"
                             "    node |> dn_enable_on_event(false)\n"
                             "    assert(!(node |> dn_is_on_event_enabled()))\n"
                             "\n"
                             "    node |> dn_enable_children(false)\n"
                             "    assert(!(node |> dn_is_children_enabled()))\n"
                             "\n"
                             "    node |> dn_gameobject_finalize()\n"
                             "    unsafe { delete node; }\n"
                             "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_enable_individual");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }

    TEST_CASE("Newly initialised object has all callbacks enabled by default")
    {
        auto path = WriteDas("_test_dngobj_defaults.das", "options gen2\n"
                                                          "options rtti\n"
                                                          " require dn_doctest\n"
                                                          "require dn_gameobject\n"
                                                          "require rtti\n"
                                                          "\n"
                                                          "class Node : dn_GameObject {\n"
                                                          "    def override _init() {}\n"
                                                          "    def override _ready() {}\n"
                                                          "    def override _update(delta : double) {}\n"
                                                          "    def override _physics_update(delta : double) {}\n"
                                                          "    def override _draw() {}\n"
                                                          "    def override _draw_ui() {}\n"
                                                          "    def override _debug() {}\n"
                                                          "}\n"
                                                          "\n"
                                                          "[export] def test_defaults() {\n"
                                                          "    var node = new Node()\n"
                                                          "    node |> dn_gameobject_init()\n"
                                                          "    assert(node |> dn_is_on_event_enabled())\n"
                                                          "    assert(node |> dn_is_update_enabled())\n"
                                                          "    assert(node |> dn_is_physics_update_enabled())\n"
                                                          "    assert(node |> dn_is_draw_enabled())\n"
                                                          "    assert(node |> dn_is_draw_ui_enabled())\n"
                                                          "    assert(node |> dn_is_debug_enabled())\n"
                                                          "    assert(node |> dn_is_children_enabled())\n"
                                                          "    node |> dn_gameobject_finalize()\n"
                                                          "    unsafe { delete node; }\n"
                                                          "}\n");

        duin::Script script(path);
        script.InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_Doctest);
            NEED_MODULE(Module_DnGameObject);
        });
        REQUIRE(script.Compile());
        REQUIRE(script.SimulateContext());
        auto *fn = script.FindFunction("test_defaults");
        CHECK(script.CallScript(fn));
        script.ResetScript();
    }
}

} // namespace TestScriptDnGameObjectModule
