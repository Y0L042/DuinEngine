options gen2

require dn_log
require math
require imgui
require imgui/imgui_boost_runtime
require imgui/imgui_boost_v2
require imgui/imgui_widgets_builtin
require imgui/imgui_containers_builtin
require imgui/imgui_table_builtin
require imgui/imgui_layout_builtin
require imgui/imgui_style_builtin
require imgui/imgui_drawlist_builtin
require imgui/imgui_colors
require imgui/imgui_theme_daslang

struct DemoRow {
    name : string
    kind : string
    value : string
}

var private ROWS : array<DemoRow>
var private g_time : float = 0.0f
var private ROW_NAME : table<int; NarrativeState>
var private ROW_KIND : table<int; NarrativeState>
var private ROW_VALUE : table<int; NarrativeState>

def private draw_widgets_tab() {
    input_text(W_NAME, (text = "Name"))
    VOLUME.bounds = (0.0f, 1.0f)
    slider_float(VOLUME, (text = "Volume"))
    LEVEL.bounds = (0, 10)
    slider_int(LEVEL, (text = "Level"))
    checkbox(MUTED, (text = "Muted"))
    combo(QUALITY, (text = "Quality", items <- ["Low", "Medium", "High", "Ultra"]))
    color_edit3(ACCENT, (text = "Accent"))
    separator()
    SPEED.bounds = (0.0f, 10.0f)
    drag_float(SPEED, (text = "Speed (drag)", speed = 0.05f, format = "%.2f"))
    drag_float3(POSITION, (text = "Position", speed = 0.1f, format = "%.2f"))
    input_int(COUNT, (text = "Count", step = 1, step_fast = 10))
    separator()
    if (button(SAVE_BTN, (text = "Save"))) {
        print("save: name={W_NAME.value} volume={VOLUME.value} level={LEVEL.value} muted={MUTED.value} quality={QUALITY.value}\n")
    }
    same_line()
    if (button(RESET_BTN, (text = "Reset"))) {
        VOLUME.value = 0.5f
        LEVEL.value = 0
        MUTED.value = false
    }
    text("saves: {SAVE_BTN.click_count}  quality index: {QUALITY.value}")
}

def private draw_containers_tab() {
    collapsing_header(CH_BASIC, (text = "Collapsing header", closable = false, flags = ImGuiTreeNodeFlags.DefaultOpen)) {
        text("Body only runs while the header is open.")
        checkbox(CH_FLAG, (text = "A checkbox inside"))
    }
    tree_node(TREE_ROOT, (text = "Tree", flags = ImGuiTreeNodeFlags.DefaultOpen)) {
        tree_node(TREE_A, (text = "Child A", flags = ImGuiTreeNodeFlags.None)) {
            text("Leaf under A")
        }
        tree_node(TREE_B, (text = "Child B", flags = ImGuiTreeNodeFlags.None)) {
            text("Leaf under B")
        }
    }
    separator()
    if (button(OPEN_POPUP_BTN, (text = "Open popup"))) {
        DEMO_POPUP.pending_open = true
    }
    popup(DEMO_POPUP, (text = "DemoPopup", flags = ImGuiWindowFlags.None)) {
        text("A popup")
        checkbox(POPUP_OPT, (text = "Option"))
        if (button(POPUP_CLOSE_BTN, (text = "Close"))) {
            DEMO_POPUP.pending_close = true
        }
    }
    same_line()
    button(HOVER_BTN, (text = "Hover me"))
    item_tooltip(HOVER_TIP) {
        text("Tooltip body (BeginItemTooltip).")
    }
}

def private draw_table_tab() {
    let tflags = (ImGuiTableFlags.BordersOuter | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable
                | ImGuiTableFlags.Reorderable | ImGuiTableFlags.Hideable | ImGuiTableFlags.ScrollY)
    data_table(DEMO_TABLE, (text = "##rows", columns = 3, flags = tflags,
                            outer_size = float2(0.0f, 0.0f), inner_width = 0.0f)) {
        table_setup_scroll_freeze(0, 1)
        table_setup_column("Name", ImGuiTableColumnFlags.None, 0.0f, 0x0001u)
        table_setup_column("Type", ImGuiTableColumnFlags.None, 0.0f, 0x0002u)
        table_setup_column("Value", ImGuiTableColumnFlags.None, 0.0f, 0x0003u)
        table_headers_row()
        for (i in range(length(ROWS))) {
            table_next_row()
            table_set_column_index(0)
            text(ROW_NAME[i], (text = ROWS[i].name))
            table_set_column_index(1)
            text(ROW_KIND[i], (text = ROWS[i].kind))
            table_set_column_index(2)
            text(ROW_VALUE[i], (text = ROWS[i].value))
        }
    }
}

def private draw_layout_tab() {
    dock_left(SIDEBAR, (init = 160.0f, bounds = (80.0f, 300.0f))) {
        text("Sidebar")
        button(SIDEBAR_BTN, (text = "Action"))
    }
    split_v(SPLIT_VERT, (init = 0.6f, bounds = (0.1f, 0.9f)),
            ${
                split_h(SPLIT_TOP, (init = 0.5f, bounds = (0.1f, 0.9f)),
                        ${
                            text("Left pane")
                        },
                        ${
                            text("Right pane")
                        })
            },
            ${
                text("Bottom pane - drag the splitters")
            })
}

def private draw_style_tab() {
    with_style((ImGuiCol.Button, ImVec4(0.85f, 0.20f, 0.20f, 1.0f)),
               (ImGuiCol.ButtonHovered, ImVec4(0.95f, 0.30f, 0.30f, 1.0f)),
               (ImGuiCol.ButtonActive, ImVec4(0.70f, 0.12f, 0.12f, 1.0f)),
               (ImGuiStyleVar.FrameRounding, 8.0f)) {
        button(RED_BTN, (text = "Red rounded"))
    }
    let accent = ACCENT.value
    with_style((ImGuiCol.Text, ImVec4(accent.x, accent.y, accent.z, 1.0f))) {
        text("This text uses the Accent colour from the Widgets tab.")
    }
    button(PLAIN_BTN, (text = "Plain button"))
}

def private draw_drawing_tab() {
    progress_bar(PROGRESS, (fraction = 0.5f + 0.5f * sin(g_time), size = float2(-1.0f, 0.0f), overlay = ""))
    let origin = GetCursorScreenPos()
    let pulse = 20.0f + 10.0f * sin(g_time * 2.0f)
    with_window_drawlist() $(var dl) {
        dl |> add_rect_filled(float2(origin.x + 8.0f, origin.y + 8.0f), float2(origin.x + 420.0f, origin.y + 150.0f),
                              rgba(35u, 35u, 50u, 200u), 6.0f)
        dl |> add_circle_filled(float2(origin.x + 70.0f, origin.y + 80.0f), pulse, rgba(70u, 130u, 240u, 255u), 0)
        dl |> add_triangle_filled(float2(origin.x + 180.0f, origin.y + 40.0f), float2(origin.x + 145.0f, origin.y + 120.0f),
                                  float2(origin.x + 215.0f, origin.y + 120.0f), rgba(60u, 220u, 100u, 255u))
        dl |> add_line(float2(origin.x + 250.0f, origin.y + 40.0f), float2(origin.x + 400.0f, origin.y + 120.0f),
                       rgba(240u, 220u, 60u, 255u), 2.0f)
        dl |> add_text(float2(origin.x + 16.0f, origin.y + 128.0f), rgba(200u, 200u, 210u, 255u), "window drawlist")
    }
    dummy(DRAW_SPACE, (size = float2(420.0f, 160.0f)))
}

[export]
def game_ready() {
    ROWS <- [DemoRow(name = "player", kind = "entity", value = "1"),
             DemoRow(name = "gravity", kind = "float", value = "9.81"),
             DemoRow(name = "title", kind = "string", value = "Duin"),
             DemoRow(name = "fullscreen", kind = "bool", value = "false")]
}

[export]
def game_update(delta : double) {
    g_time += float(delta)
}

// [export]
// def game_physics_update(delta : double) {
// }
//
// [export]
// def game_draw() {
// }

[export]
def game_draw_ui() {
    begin_frame()
    SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond.FirstUseEver)
    SetNextWindowSize(ImVec2(760.0f, 560.0f), ImGuiCond.FirstUseEver)
    window(DEMO_WIN, (text = "dasImgui v2 demo", closable = false, flags = ImGuiWindowFlags.MenuBar)) {
        menu_bar(DEMO_BAR) {
            menu(VIEW_MENU, (text = "View", enabled = true)) {
                if (menu_label(THEME_ITEM, (text = "Apply daslang theme"))) {
                    apply_daslang_theme()
                }
            }
        }
        tab_bar(DEMO_TABS, (text = "DemoTabs", flags = ImGuiTabBarFlags.Reorderable)) {
            tab_item(TAB_WIDGETS, (text = "Widgets", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_widgets_tab()
            }
            tab_item(TAB_CONTAINERS, (text = "Containers", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_containers_tab()
            }
            tab_item(TAB_TABLE, (text = "Table", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_table_tab()
            }
            tab_item(TAB_LAYOUT, (text = "Layout", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_layout_tab()
            }
            tab_item(TAB_STYLE, (text = "Style", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_style_tab()
            }
            tab_item(TAB_DRAWING, (text = "Drawing", closable = false, flags = ImGuiTabItemFlags.None)) {
                draw_drawing_tab()
            }
        }
    }
    end_of_frame()
}

[export]
def main() {
}
