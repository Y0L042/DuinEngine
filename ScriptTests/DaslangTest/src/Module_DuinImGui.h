#pragma once

#include "daScript/daScript.h"
#include <external/imgui.h>

using namespace das;

// Thin wrappers over ImGui calls for daslang binding

bool duin_imgui_begin(const char* name, int flags) {
    return ImGui::Begin(name, nullptr, flags);
}

void duin_imgui_end() {
    ImGui::End();
}

void duin_imgui_text(const char* text) {
    ImGui::TextUnformatted(text);
}

bool duin_imgui_button(const char* label) {
    return ImGui::Button(label);
}

bool duin_imgui_checkbox(const char* label, bool& v) {
    return ImGui::Checkbox(label, &v);
}

bool duin_imgui_slider_float(const char* label, float& v, float v_min, float v_max) {
    return ImGui::SliderFloat(label, &v, v_min, v_max);
}

void duin_imgui_separator() {
    ImGui::Separator();
}

void duin_imgui_same_line() {
    ImGui::SameLine();
}

void duin_imgui_show_demo_window() {
    ImGui::ShowDemoWindow();
}

class Module_DuinImGui : public Module {
public:
    Module_DuinImGui() : Module("duin_imgui") {
        ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addExtern<DAS_BIND_FUN(duin_imgui_begin)>(*this, lib, "Begin",
            SideEffects::modifyExternal, "duin_imgui_begin");

        addExtern<DAS_BIND_FUN(duin_imgui_end)>(*this, lib, "End",
            SideEffects::modifyExternal, "duin_imgui_end");

        addExtern<DAS_BIND_FUN(duin_imgui_text)>(*this, lib, "Text",
            SideEffects::modifyExternal, "duin_imgui_text");

        addExtern<DAS_BIND_FUN(duin_imgui_button)>(*this, lib, "Button",
            SideEffects::modifyExternal, "duin_imgui_button");

        addExtern<DAS_BIND_FUN(duin_imgui_checkbox)>(*this, lib, "Checkbox",
            SideEffects::modifyArgument, "duin_imgui_checkbox");

        addExtern<DAS_BIND_FUN(duin_imgui_slider_float)>(*this, lib, "SliderFloat",
            SideEffects::modifyArgument, "duin_imgui_slider_float");

        addExtern<DAS_BIND_FUN(duin_imgui_separator)>(*this, lib, "Separator",
            SideEffects::modifyExternal, "duin_imgui_separator");

        addExtern<DAS_BIND_FUN(duin_imgui_same_line)>(*this, lib, "SameLine",
            SideEffects::modifyExternal, "duin_imgui_same_line");

        addExtern<DAS_BIND_FUN(duin_imgui_show_demo_window)>(*this, lib, "ShowDemoWindow",
            SideEffects::modifyExternal, "duin_imgui_show_demo_window");
    }
};

REGISTER_MODULE(Module_DuinImGui);
