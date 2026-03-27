options gen2

require duin_imgui

var slider_val = 0.5f
var check_val = false
var counter = 0

[export]
def draw_ui() {
    if (duin_imgui::Begin("Daslang ImGui Demo", 0)) {
        duin_imgui::Text("Hello from Daslang!")
        duin_imgui::Separator()

        if (duin_imgui::Button("Click Me")) {
            counter += 1
        }
        duin_imgui::SameLine()
        duin_imgui::Text("Count: {counter}")

        duin_imgui::SliderFloat("Slider", slider_val, 0.0f, 1.0f)
        duin_imgui::Checkbox("Check", check_val)

        duin_imgui::Separator()
        duin_imgui::ShowDemoWindow()
    }
    duin_imgui::End()
}
