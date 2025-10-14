#include <doctest.h>
#include <DuinEditor/src/Editor.h>

TEST_CASE("Editor Singleton") {
    Editor& e1 = Editor::Get();
    Editor& e2 = Editor::Get();
    CHECK(&e1 == &e2);
}

TEST_CASE("Editor Lifecycle Methods") {
    Editor& editor = Editor::Get();
    CHECK_NOTHROW(editor.Initialize());
    CHECK_NOTHROW(editor.Ready());
    CHECK_NOTHROW(editor.Update(0.016));
    CHECK_NOTHROW(editor.PhysicsUpdate(0.016));
    CHECK_NOTHROW(editor.Draw());
    CHECK_NOTHROW(editor.DrawUI());
    CHECK_NOTHROW(editor.Debug());
}

TEST_CASE("Editor Event Handling") {
    Editor& editor = Editor::Get();
    duin::Event event;
    CHECK_NOTHROW(editor.OnEvent(event));
}

TEST_CASE("Editor Config Save/Load") {
    duin::JSONValue value;
    CHECK_NOTHROW(Editor::SaveProjectEditorConfig(value));
    CHECK_NOTHROW(Editor::LoadProjectEditorConfig());
}