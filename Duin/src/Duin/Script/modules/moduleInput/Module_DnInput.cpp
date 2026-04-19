#include "dnpch.h"
#include "Module_DnInput.h"

#include <daScript/simulate/bind_enum.h>
#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_typefactory_bind.h>
#include <daScript/ast/ast_interop.h>

#include "Duin/Core/Events/Input.h"
#include "Duin/Core/Events/InputActions.h"
#include "Duin/Core/Events/InputDevice_Keyboard.h"
#include "Duin/Core/Events/InputDevice_Mouse.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/DaslangConversionHelpers.h"

// ---- Enum bindings — must appear before any das:: usage ----

DAS_BIND_ENUM_CAST_98_IN_NAMESPACE(duin::Input::KeyEvent, KeyEvent)
DAS_BASE_BIND_ENUM_98(duin::Input::KeyEvent, KeyEvent, PRESSED, PRESSED_REPEATED, HELD, RELEASED, IDLE)

// DAS_FOR_EACH on MSVC is capped at 125 args; DN_Scancode exceeds that limit.
// Use the GEN variant for cast+typeFactory, then build the Enumeration manually.
DAS_BIND_ENUM_CAST_98(DN_Scancode)
DAS_BASE_BIND_ENUM_98_GEN(DN_Scancode, DN_Scancode)

class EnumerationDN_Scancode : public das::Enumeration
{
  public:
    EnumerationDN_Scancode() : das::Enumeration("DN_Scancode")
    {
        external = true;
        cppName = "DN_Scancode";
        baseType = (das::Type)das::ToBasicType<das::underlying_type<DN_Scancode>::type>::type;
        struct E
        {
            const char *name;
            int64_t value;
        };
        // clang-format off
        static const E entries[] = {
          {"DN_SCANCODE_UNKNOWN",       (int64_t)DN_SCANCODE_UNKNOWN},
          {"DN_SCANCODE_A",             (int64_t)DN_SCANCODE_A},
          {"DN_SCANCODE_B",             (int64_t)DN_SCANCODE_B},
          {"DN_SCANCODE_C",             (int64_t)DN_SCANCODE_C},
          {"DN_SCANCODE_D",             (int64_t)DN_SCANCODE_D},
          {"DN_SCANCODE_E",             (int64_t)DN_SCANCODE_E},
          {"DN_SCANCODE_F",             (int64_t)DN_SCANCODE_F},
          {"DN_SCANCODE_G",             (int64_t)DN_SCANCODE_G},
          {"DN_SCANCODE_H",             (int64_t)DN_SCANCODE_H},
          {"DN_SCANCODE_I",             (int64_t)DN_SCANCODE_I},
          {"DN_SCANCODE_J",             (int64_t)DN_SCANCODE_J},
          {"DN_SCANCODE_K",             (int64_t)DN_SCANCODE_K},
          {"DN_SCANCODE_L",             (int64_t)DN_SCANCODE_L},
          {"DN_SCANCODE_M",             (int64_t)DN_SCANCODE_M},
          {"DN_SCANCODE_N",             (int64_t)DN_SCANCODE_N},
          {"DN_SCANCODE_O",             (int64_t)DN_SCANCODE_O},
          {"DN_SCANCODE_P",             (int64_t)DN_SCANCODE_P},
          {"DN_SCANCODE_Q",             (int64_t)DN_SCANCODE_Q},
          {"DN_SCANCODE_R",             (int64_t)DN_SCANCODE_R},
          {"DN_SCANCODE_S",             (int64_t)DN_SCANCODE_S},
          {"DN_SCANCODE_T",             (int64_t)DN_SCANCODE_T},
          {"DN_SCANCODE_U",             (int64_t)DN_SCANCODE_U},
          {"DN_SCANCODE_V",             (int64_t)DN_SCANCODE_V},
          {"DN_SCANCODE_W",             (int64_t)DN_SCANCODE_W},
          {"DN_SCANCODE_X",             (int64_t)DN_SCANCODE_X},
          {"DN_SCANCODE_Y",             (int64_t)DN_SCANCODE_Y},
          {"DN_SCANCODE_Z",             (int64_t)DN_SCANCODE_Z},
          {"DN_SCANCODE_1",             (int64_t)DN_SCANCODE_1},
          {"DN_SCANCODE_2",             (int64_t)DN_SCANCODE_2},
          {"DN_SCANCODE_3",             (int64_t)DN_SCANCODE_3},
          {"DN_SCANCODE_4",             (int64_t)DN_SCANCODE_4},
          {"DN_SCANCODE_5",             (int64_t)DN_SCANCODE_5},
          {"DN_SCANCODE_6",             (int64_t)DN_SCANCODE_6},
          {"DN_SCANCODE_7",             (int64_t)DN_SCANCODE_7},
          {"DN_SCANCODE_8",             (int64_t)DN_SCANCODE_8},
          {"DN_SCANCODE_9",             (int64_t)DN_SCANCODE_9},
          {"DN_SCANCODE_0",             (int64_t)DN_SCANCODE_0},
          {"DN_SCANCODE_RETURN",        (int64_t)DN_SCANCODE_RETURN},
          {"DN_SCANCODE_ESCAPE",        (int64_t)DN_SCANCODE_ESCAPE},
          {"DN_SCANCODE_BACKSPACE",     (int64_t)DN_SCANCODE_BACKSPACE},
          {"DN_SCANCODE_TAB",           (int64_t)DN_SCANCODE_TAB},
          {"DN_SCANCODE_SPACE",         (int64_t)DN_SCANCODE_SPACE},
          {"DN_SCANCODE_MINUS",         (int64_t)DN_SCANCODE_MINUS},
          {"DN_SCANCODE_EQUALS",        (int64_t)DN_SCANCODE_EQUALS},
          {"DN_SCANCODE_LEFTBRACKET",   (int64_t)DN_SCANCODE_LEFTBRACKET},
          {"DN_SCANCODE_RIGHTBRACKET",  (int64_t)DN_SCANCODE_RIGHTBRACKET},
          {"DN_SCANCODE_BACKSLASH",     (int64_t)DN_SCANCODE_BACKSLASH},
          {"DN_SCANCODE_NONUSHASH",     (int64_t)DN_SCANCODE_NONUSHASH},
          {"DN_SCANCODE_SEMICOLON",     (int64_t)DN_SCANCODE_SEMICOLON},
          {"DN_SCANCODE_APOSTROPHE",    (int64_t)DN_SCANCODE_APOSTROPHE},
          {"DN_SCANCODE_GRAVE",         (int64_t)DN_SCANCODE_GRAVE},
          {"DN_SCANCODE_COMMA",         (int64_t)DN_SCANCODE_COMMA},
          {"DN_SCANCODE_PERIOD",        (int64_t)DN_SCANCODE_PERIOD},
          {"DN_SCANCODE_SLASH",         (int64_t)DN_SCANCODE_SLASH},
          {"DN_SCANCODE_CAPSLOCK",      (int64_t)DN_SCANCODE_CAPSLOCK},
          {"DN_SCANCODE_F1",            (int64_t)DN_SCANCODE_F1},
          {"DN_SCANCODE_F2",            (int64_t)DN_SCANCODE_F2},
          {"DN_SCANCODE_F3",            (int64_t)DN_SCANCODE_F3},
          {"DN_SCANCODE_F4",            (int64_t)DN_SCANCODE_F4},
          {"DN_SCANCODE_F5",            (int64_t)DN_SCANCODE_F5},
          {"DN_SCANCODE_F6",            (int64_t)DN_SCANCODE_F6},
          {"DN_SCANCODE_F7",            (int64_t)DN_SCANCODE_F7},
          {"DN_SCANCODE_F8",            (int64_t)DN_SCANCODE_F8},
          {"DN_SCANCODE_F9",            (int64_t)DN_SCANCODE_F9},
          {"DN_SCANCODE_F10", (int64_t)DN_SCANCODE_F10},
          {"DN_SCANCODE_F11", (int64_t)DN_SCANCODE_F11},
          {"DN_SCANCODE_F12", (int64_t)DN_SCANCODE_F12},
          {"DN_SCANCODE_PRINTSCREEN", (int64_t)DN_SCANCODE_PRINTSCREEN},
          {"DN_SCANCODE_SCROLLLOCK", (int64_t)DN_SCANCODE_SCROLLLOCK},
          {"DN_SCANCODE_PAUSE", (int64_t)DN_SCANCODE_PAUSE},
          {"DN_SCANCODE_INSERT", (int64_t)DN_SCANCODE_INSERT},
          {"DN_SCANCODE_HOME", (int64_t)DN_SCANCODE_HOME},
          {"DN_SCANCODE_PAGEUP", (int64_t)DN_SCANCODE_PAGEUP},
          {"DN_SCANCODE_DELETE", (int64_t)DN_SCANCODE_DELETE},
          {"DN_SCANCODE_END", (int64_t)DN_SCANCODE_END},
          {"DN_SCANCODE_PAGEDOWN", (int64_t)DN_SCANCODE_PAGEDOWN},
          {"DN_SCANCODE_RIGHT", (int64_t)DN_SCANCODE_RIGHT},
          {"DN_SCANCODE_LEFT", (int64_t)DN_SCANCODE_LEFT},
          {"DN_SCANCODE_DOWN", (int64_t)DN_SCANCODE_DOWN},
          {"DN_SCANCODE_UP", (int64_t)DN_SCANCODE_UP},
          {"DN_SCANCODE_NUMLOCKCLEAR", (int64_t)DN_SCANCODE_NUMLOCKCLEAR},
          {"DN_SCANCODE_KP_DIVIDE", (int64_t)DN_SCANCODE_KP_DIVIDE},
          {"DN_SCANCODE_KP_MULTIPLY", (int64_t)DN_SCANCODE_KP_MULTIPLY},
          {"DN_SCANCODE_KP_MINUS", (int64_t)DN_SCANCODE_KP_MINUS},
          {"DN_SCANCODE_KP_PLUS", (int64_t)DN_SCANCODE_KP_PLUS},
          {"DN_SCANCODE_KP_ENTER", (int64_t)DN_SCANCODE_KP_ENTER},
          {"DN_SCANCODE_KP_1", (int64_t)DN_SCANCODE_KP_1},
          {"DN_SCANCODE_KP_2", (int64_t)DN_SCANCODE_KP_2},
          {"DN_SCANCODE_KP_3", (int64_t)DN_SCANCODE_KP_3},
          {"DN_SCANCODE_KP_4", (int64_t)DN_SCANCODE_KP_4},
          {"DN_SCANCODE_KP_5", (int64_t)DN_SCANCODE_KP_5},
          {"DN_SCANCODE_KP_6", (int64_t)DN_SCANCODE_KP_6},
          {"DN_SCANCODE_KP_7", (int64_t)DN_SCANCODE_KP_7},
          {"DN_SCANCODE_KP_8", (int64_t)DN_SCANCODE_KP_8},
          {"DN_SCANCODE_KP_9", (int64_t)DN_SCANCODE_KP_9},
          {"DN_SCANCODE_KP_0", (int64_t)DN_SCANCODE_KP_0},
          {"DN_SCANCODE_KP_PERIOD", (int64_t)DN_SCANCODE_KP_PERIOD},
          {"DN_SCANCODE_KP_EQUALS", (int64_t)DN_SCANCODE_KP_EQUALS},
          {"DN_SCANCODE_NONUSBACKSLASH", (int64_t)DN_SCANCODE_NONUSBACKSLASH},
          {"DN_SCANCODE_APPLICATION", (int64_t)DN_SCANCODE_APPLICATION},
          {"DN_SCANCODE_POWER", (int64_t)DN_SCANCODE_POWER},
          {"DN_SCANCODE_F13", (int64_t)DN_SCANCODE_F13},
          {"DN_SCANCODE_F14", (int64_t)DN_SCANCODE_F14},
          {"DN_SCANCODE_F15", (int64_t)DN_SCANCODE_F15},
          {"DN_SCANCODE_F16", (int64_t)DN_SCANCODE_F16},
          {"DN_SCANCODE_F17", (int64_t)DN_SCANCODE_F17},
          {"DN_SCANCODE_F18", (int64_t)DN_SCANCODE_F18},
          {"DN_SCANCODE_F19", (int64_t)DN_SCANCODE_F19},
          {"DN_SCANCODE_F20", (int64_t)DN_SCANCODE_F20},
          {"DN_SCANCODE_F21", (int64_t)DN_SCANCODE_F21},
          {"DN_SCANCODE_F22", (int64_t)DN_SCANCODE_F22},
          {"DN_SCANCODE_F23", (int64_t)DN_SCANCODE_F23},
          {"DN_SCANCODE_F24", (int64_t)DN_SCANCODE_F24},
          {"DN_SCANCODE_EXECUTE", (int64_t)DN_SCANCODE_EXECUTE},
          {"DN_SCANCODE_HELP", (int64_t)DN_SCANCODE_HELP},
          {"DN_SCANCODE_MENU", (int64_t)DN_SCANCODE_MENU},
          {"DN_SCANCODE_SELECT", (int64_t)DN_SCANCODE_SELECT},
          {"DN_SCANCODE_STOP", (int64_t)DN_SCANCODE_STOP},
          {"DN_SCANCODE_AGAIN", (int64_t)DN_SCANCODE_AGAIN},
          {"DN_SCANCODE_UNDO", (int64_t)DN_SCANCODE_UNDO},
          {"DN_SCANCODE_CUT", (int64_t)DN_SCANCODE_CUT},
          {"DN_SCANCODE_COPY", (int64_t)DN_SCANCODE_COPY},
          {"DN_SCANCODE_PASTE", (int64_t)DN_SCANCODE_PASTE},
          {"DN_SCANCODE_FIND", (int64_t)DN_SCANCODE_FIND},
          {"DN_SCANCODE_MUTE", (int64_t)DN_SCANCODE_MUTE},
          {"DN_SCANCODE_VOLUMEUP", (int64_t)DN_SCANCODE_VOLUMEUP},
          {"DN_SCANCODE_VOLUMEDOWN", (int64_t)DN_SCANCODE_VOLUMEDOWN},
          {"DN_SCANCODE_KP_COMMA", (int64_t)DN_SCANCODE_KP_COMMA},
          {"DN_SCANCODE_KP_EQUALSAS400", (int64_t)DN_SCANCODE_KP_EQUALSAS400},
          {"DN_SCANCODE_ALTERASE", (int64_t)DN_SCANCODE_ALTERASE},
          {"DN_SCANCODE_SYSREQ", (int64_t)DN_SCANCODE_SYSREQ},
          {"DN_SCANCODE_CANCEL", (int64_t)DN_SCANCODE_CANCEL},
          {"DN_SCANCODE_CLEAR", (int64_t)DN_SCANCODE_CLEAR},
          {"DN_SCANCODE_PRIOR", (int64_t)DN_SCANCODE_PRIOR},
          {"DN_SCANCODE_RETURN2", (int64_t)DN_SCANCODE_RETURN2},
          {"DN_SCANCODE_SEPARATOR", (int64_t)DN_SCANCODE_SEPARATOR},
          {"DN_SCANCODE_OUT", (int64_t)DN_SCANCODE_OUT},
          {"DN_SCANCODE_OPER", (int64_t)DN_SCANCODE_OPER},
          {"DN_SCANCODE_CLEARAGAIN", (int64_t)DN_SCANCODE_CLEARAGAIN},
          {"DN_SCANCODE_CRSEL", (int64_t)DN_SCANCODE_CRSEL},
          {"DN_SCANCODE_EXSEL", (int64_t)DN_SCANCODE_EXSEL},
          {"DN_SCANCODE_LCTRL", (int64_t)DN_SCANCODE_LCTRL},
          {"DN_SCANCODE_LSHIFT", (int64_t)DN_SCANCODE_LSHIFT},
          {"DN_SCANCODE_LALT", (int64_t)DN_SCANCODE_LALT},
          {"DN_SCANCODE_LGUI", (int64_t)DN_SCANCODE_LGUI},
          {"DN_SCANCODE_RCTRL", (int64_t)DN_SCANCODE_RCTRL},
          {"DN_SCANCODE_RSHIFT", (int64_t)DN_SCANCODE_RSHIFT},
          {"DN_SCANCODE_RALT", (int64_t)DN_SCANCODE_RALT},
          {"DN_SCANCODE_RGUI", (int64_t)DN_SCANCODE_RGUI},
          {"DN_SCANCODE_MODE", (int64_t)DN_SCANCODE_MODE},
          {"DN_SCANCODE_SLEEP", (int64_t)DN_SCANCODE_SLEEP},
          {"DN_SCANCODE_WAKE", (int64_t)DN_SCANCODE_WAKE},
          {"DN_SCANCODE_MEDIA_PLAY", (int64_t)DN_SCANCODE_MEDIA_PLAY},
          {"DN_SCANCODE_MEDIA_PAUSE", (int64_t)DN_SCANCODE_MEDIA_PAUSE},
          {"DN_SCANCODE_MEDIA_RECORD", (int64_t)DN_SCANCODE_MEDIA_RECORD},
          {"DN_SCANCODE_MEDIA_FAST_FORWARD", (int64_t)DN_SCANCODE_MEDIA_FAST_FORWARD},
          {"DN_SCANCODE_MEDIA_REWIND", (int64_t)DN_SCANCODE_MEDIA_REWIND},
          {"DN_SCANCODE_MEDIA_NEXT_TRACK", (int64_t)DN_SCANCODE_MEDIA_NEXT_TRACK},
          {"DN_SCANCODE_MEDIA_PREVIOUS_TRACK", (int64_t)DN_SCANCODE_MEDIA_PREVIOUS_TRACK},
          {"DN_SCANCODE_MEDIA_STOP", (int64_t)DN_SCANCODE_MEDIA_STOP},
          {"DN_SCANCODE_MEDIA_EJECT", (int64_t)DN_SCANCODE_MEDIA_EJECT},
          {"DN_SCANCODE_MEDIA_PLAY_PAUSE", (int64_t)DN_SCANCODE_MEDIA_PLAY_PAUSE},
          {"DN_SCANCODE_MEDIA_SELECT", (int64_t)DN_SCANCODE_MEDIA_SELECT},
          {"DN_SCANCODE_AC_NEW", (int64_t)DN_SCANCODE_AC_NEW},
          {"DN_SCANCODE_AC_OPEN", (int64_t)DN_SCANCODE_AC_OPEN},
          {"DN_SCANCODE_AC_CLOSE", (int64_t)DN_SCANCODE_AC_CLOSE},
          {"DN_SCANCODE_AC_EXIT", (int64_t)DN_SCANCODE_AC_EXIT},
          {"DN_SCANCODE_AC_SAVE", (int64_t)DN_SCANCODE_AC_SAVE},
          {"DN_SCANCODE_AC_PRINT", (int64_t)DN_SCANCODE_AC_PRINT},
          {"DN_SCANCODE_AC_PROPERTIES", (int64_t)DN_SCANCODE_AC_PROPERTIES},
          {"DN_SCANCODE_AC_SEARCH", (int64_t)DN_SCANCODE_AC_SEARCH},
          {"DN_SCANCODE_AC_HOME", (int64_t)DN_SCANCODE_AC_HOME},
          {"DN_SCANCODE_AC_BACK", (int64_t)DN_SCANCODE_AC_BACK},
          {"DN_SCANCODE_AC_FORWARD", (int64_t)DN_SCANCODE_AC_FORWARD},
          {"DN_SCANCODE_AC_STOP", (int64_t)DN_SCANCODE_AC_STOP},
          {"DN_SCANCODE_AC_REFRESH", (int64_t)DN_SCANCODE_AC_REFRESH},
          {"DN_SCANCODE_AC_BOOKMARKS", (int64_t)DN_SCANCODE_AC_BOOKMARKS},
          {"DN_SCANCODE_SOFTLEFT", (int64_t)DN_SCANCODE_SOFTLEFT},
          {"DN_SCANCODE_SOFTRIGHT", (int64_t)DN_SCANCODE_SOFTRIGHT},
          {"DN_SCANCODE_CALL", (int64_t)DN_SCANCODE_CALL},
          {"DN_SCANCODE_ENDCALL", (int64_t)DN_SCANCODE_ENDCALL},
          {"DN_SCANCODE_COUNT", (int64_t)DN_SCANCODE_COUNT},
        };
        // clang-format on
        for (const auto &e : entries)
            addI(e.name, e.value, das::LineInfo());
    }
};

// ---- Keyboard _impl functions ----

static int dn_is_key_pressed_impl(DN_Scancode c)
{
    return duin::Input::IsKeyPressed(c);
}

static int dn_is_key_pressed_again_impl(DN_Scancode c)
{
    return duin::Input::IsKeyPressedAgain(c);
}

static int dn_is_key_down_impl(DN_Scancode c)
{
    return duin::Input::IsKeyDown(c);
}

static int dn_is_key_up_impl(DN_Scancode c)
{
    return duin::Input::IsKeyUp(c);
}

static int dn_is_key_released_impl(DN_Scancode c)
{
    return duin::Input::IsKeyReleased(c);
}

static DN_Scancode dn_get_key_pressed_impl()
{
    return duin::Input::GetKeyPressed();
}

static int dn_is_modifier_down_impl(uint32_t mod)
{
    return duin::Input::IsModifierDown(static_cast<DN_Keymod>(mod));
}

static int dn_is_modifier_exact_impl(uint32_t mod)
{
    return duin::Input::IsModifierExact(static_cast<DN_Keymod>(mod));
}

static int dn_is_input_vector_pressed_impl(DN_Scancode up, DN_Scancode dn, DN_Scancode left, DN_Scancode right)
{
    return duin::Input::IsInputVectorPressed(up, dn, left, right);
}

static das::float2 dn_get_input_vector_impl(DN_Scancode up, DN_Scancode dn, DN_Scancode left, DN_Scancode right)
{
    return duin::to_f2(duin::Input::GetInputVector(up, dn, left, right));
}

// ---- Mouse _impl functions ----

static void dn_capture_mouse_impl(int enable)
{
    duin::Input::CaptureMouse(enable);
}

static int dn_is_mouse_btn_pressed_impl(uint32_t b)
{
    return duin::Input::IsMouseButtonPressed(static_cast<DN_MouseButtonFlags>(b));
}

static int dn_is_mouse_btn_down_impl(uint32_t b)
{
    return duin::Input::IsMouseButtonDown(static_cast<DN_MouseButtonFlags>(b));
}

static int dn_is_mouse_btn_released_impl(uint32_t b)
{
    return duin::Input::IsMouseButtonReleased(static_cast<DN_MouseButtonFlags>(b));
}

static int dn_is_mouse_btn_up_impl(uint32_t b)
{
    return duin::Input::IsMouseButtonUp(static_cast<DN_MouseButtonFlags>(b));
}

static das::float2 dn_get_mouse_position_impl()
{
    return duin::to_f2(duin::Input::GetMousePosition());
}

static das::float2 dn_get_mouse_global_position_impl()
{
    return duin::to_f2(duin::Input::GetMouseGlobalPosition());
}

static das::float2 dn_get_mouse_delta_impl()
{
    return duin::to_f2(duin::Input::GetMouseDelta());
}

static float dn_get_mouse_wheel_move_impl()
{
    return duin::Input::GetMouseWheelMove();
}

static das::float2 dn_get_mouse_wheel_move_v_impl()
{
    return duin::to_f2(duin::Input::GetMouseWheelMoveV());
}

static void dn_set_mouse_position_impl(int x, int y)
{
    duin::Input::SetMousePosition(x, y);
}

static void dn_set_mouse_offset_impl(int ox, int oy)
{
    duin::Input::SetMouseOffset(ox, oy);
}

static void dn_set_mouse_scale_impl(float sx, float sy)
{
    duin::Input::SetMouseScale(sx, sy);
}

static void dn_set_mouse_cursor_impl(int c)
{
    duin::Input::SetMouseCursor(c);
}

// ---- InputActions _impl functions ----

static std::shared_ptr<duin::InputDevice_Keyboard> s_keyboard_device;
static std::shared_ptr<duin::InputDevice_Mouse> s_mouse_device;

static void dn_create_input_action_impl(const char *name)
{
    if (name)
        duin::CreateInputAction(name);
}

static bool dn_is_input_action_triggered_impl(const char *name)
{
    return name && duin::IsInputActionTriggered(name);
}

static void dn_remove_input_action_impl(const char *name)
{
    if (name)
        duin::RemoveInputAction(name);
}

static void dn_add_keyboard_binding_impl(
    const char *action, DN_Scancode key, duin::Input::KeyEvent event, uint32_t modifier)
{
    if (!action)
        return;
    if (!s_keyboard_device)
        s_keyboard_device = std::make_shared<duin::InputDevice_Keyboard>();
    duin::AddInputActionBinding(
        action, s_keyboard_device, static_cast<DN_InputCode>(key), event, static_cast<DN_InputCode>(modifier));
}

static void dn_add_mouse_binding_impl(const char *action, uint32_t button, duin::Input::KeyEvent event)
{
    if (!action)
        return;
    if (!s_mouse_device)
        s_mouse_device = std::make_shared<duin::InputDevice_Mouse>();
    duin::AddInputActionBinding(action, s_mouse_device, static_cast<DN_InputCode>(button), event, 0);
}

static void dn_remove_keyboard_binding_impl(
    const char *action, DN_Scancode key, duin::Input::KeyEvent event, uint32_t modifier)
{
    if (!action || !s_keyboard_device)
        return;
    duin::RemoveInputActionBinding(
        action, s_keyboard_device, static_cast<DN_InputCode>(key), event, static_cast<DN_InputCode>(modifier));
}

static void dn_remove_mouse_binding_impl(const char *action, uint32_t button, duin::Input::KeyEvent event)
{
    if (!action || !s_mouse_device)
        return;
    duin::RemoveInputActionBinding(action, s_mouse_device, static_cast<DN_InputCode>(button), event, 0);
}

// ---- Module ----

class Module_DnInput : public das::Module
{
    bool initialized = false;

  public:
    Module_DnInput() : das::Module("dn_input_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
            DN_CORE_ERROR("dn_input: failed to find required module 'dn_log'");
        addBuiltinDependency(lib, logMod);

        // Enums
        addEnumeration(das::make_smart<EnumerationKeyEvent>());
        addEnumeration(das::make_smart<EnumerationDN_Scancode>());

        // Mouse button constants
        addConstant(*this, "DN_MOUSE_BUTTON_LEFT", static_cast<uint32_t>(DN_MOUSE_BUTTON_LEFT));
        addConstant(*this, "DN_MOUSE_BUTTON_MIDDLE", static_cast<uint32_t>(DN_MOUSE_BUTTON_MIDDLE));
        addConstant(*this, "DN_MOUSE_BUTTON_RIGHT", static_cast<uint32_t>(DN_MOUSE_BUTTON_RIGHT));
        addConstant(*this, "DN_MOUSE_BUTTON_X1", static_cast<uint32_t>(DN_MOUSE_BUTTON_X1));
        addConstant(*this, "DN_MOUSE_BUTTON_X2", static_cast<uint32_t>(DN_MOUSE_BUTTON_X2));

        // Key modifier constants
        addConstant(*this, "DN_KEY_MOD_NONE", static_cast<uint32_t>(DN_KEY_MOD_NONE));
        addConstant(*this, "DN_KEY_MOD_LSHIFT", static_cast<uint32_t>(DN_KEY_MOD_LSHIFT));
        addConstant(*this, "DN_KEY_MOD_RSHIFT", static_cast<uint32_t>(DN_KEY_MOD_RSHIFT));
        addConstant(*this, "DN_KEY_MOD_LCTRL", static_cast<uint32_t>(DN_KEY_MOD_LCTRL));
        addConstant(*this, "DN_KEY_MOD_RCTRL", static_cast<uint32_t>(DN_KEY_MOD_RCTRL));
        addConstant(*this, "DN_KEY_MOD_LALT", static_cast<uint32_t>(DN_KEY_MOD_LALT));
        addConstant(*this, "DN_KEY_MOD_RALT", static_cast<uint32_t>(DN_KEY_MOD_RALT));
        addConstant(*this, "DN_KEY_MOD_CTRL", static_cast<uint32_t>(DN_KEY_MOD_CTRL));
        addConstant(*this, "DN_KEY_MOD_SHIFT", static_cast<uint32_t>(DN_KEY_MOD_SHIFT));
        addConstant(*this, "DN_KEY_MOD_ALT", static_cast<uint32_t>(DN_KEY_MOD_ALT));

        // Keyboard functions
        das::addExtern<DAS_BIND_FUN(dn_is_key_pressed_impl)>(
            *this, lib, "dn_is_key_pressed_impl", das::SideEffects::accessGlobal, "dn_is_key_pressed_impl")
            ->args({"code"});
        das::addExtern<DAS_BIND_FUN(dn_is_key_pressed_again_impl)>(
            *this, lib, "dn_is_key_pressed_again_impl", das::SideEffects::accessGlobal, "dn_is_key_pressed_again_impl")
            ->args({"code"});
        das::addExtern<DAS_BIND_FUN(dn_is_key_down_impl)>(
            *this, lib, "dn_is_key_down_impl", das::SideEffects::accessGlobal, "dn_is_key_down_impl")
            ->args({"code"});
        das::addExtern<DAS_BIND_FUN(dn_is_key_up_impl)>(
            *this, lib, "dn_is_key_up_impl", das::SideEffects::accessGlobal, "dn_is_key_up_impl")
            ->args({"code"});
        das::addExtern<DAS_BIND_FUN(dn_is_key_released_impl)>(
            *this, lib, "dn_is_key_released_impl", das::SideEffects::accessGlobal, "dn_is_key_released_impl")
            ->args({"code"});
        das::addExtern<DAS_BIND_FUN(dn_get_key_pressed_impl)>(
            *this, lib, "dn_get_key_pressed_impl", das::SideEffects::accessGlobal, "dn_get_key_pressed_impl");
        das::addExtern<DAS_BIND_FUN(dn_is_modifier_down_impl)>(
            *this, lib, "dn_is_modifier_down_impl", das::SideEffects::accessGlobal, "dn_is_modifier_down_impl")
            ->args({"mod"});
        das::addExtern<DAS_BIND_FUN(dn_is_modifier_exact_impl)>(
            *this, lib, "dn_is_modifier_exact_impl", das::SideEffects::accessGlobal, "dn_is_modifier_exact_impl")
            ->args({"mod"});
        das::addExtern<DAS_BIND_FUN(dn_is_input_vector_pressed_impl)>(
            *this,
            lib,
            "dn_is_input_vector_pressed_impl",
            das::SideEffects::accessGlobal,
            "dn_is_input_vector_pressed_impl")
            ->args({"up", "dn", "left", "right"});
        das::addExtern<DAS_BIND_FUN(dn_get_input_vector_impl)>(
            *this, lib, "dn_get_input_vector_impl", das::SideEffects::accessGlobal, "dn_get_input_vector_impl")
            ->args({"up", "dn", "left", "right"});

        // Mouse functions
        das::addExtern<DAS_BIND_FUN(dn_capture_mouse_impl)>(
            *this, lib, "dn_capture_mouse_impl", das::SideEffects::modifyExternal, "dn_capture_mouse_impl")
            ->args({"enable"});
        das::addExtern<DAS_BIND_FUN(dn_is_mouse_btn_pressed_impl)>(
            *this, lib, "dn_is_mouse_btn_pressed_impl", das::SideEffects::accessGlobal, "dn_is_mouse_btn_pressed_impl")
            ->args({"button"});
        das::addExtern<DAS_BIND_FUN(dn_is_mouse_btn_down_impl)>(
            *this, lib, "dn_is_mouse_btn_down_impl", das::SideEffects::accessGlobal, "dn_is_mouse_btn_down_impl")
            ->args({"button"});
        das::addExtern<DAS_BIND_FUN(dn_is_mouse_btn_released_impl)>(
            *this,
            lib,
            "dn_is_mouse_btn_released_impl",
            das::SideEffects::accessGlobal,
            "dn_is_mouse_btn_released_impl")
            ->args({"button"});
        das::addExtern<DAS_BIND_FUN(dn_is_mouse_btn_up_impl)>(
            *this, lib, "dn_is_mouse_btn_up_impl", das::SideEffects::accessGlobal, "dn_is_mouse_btn_up_impl")
            ->args({"button"});
        das::addExtern<DAS_BIND_FUN(dn_get_mouse_position_impl)>(
            *this, lib, "dn_get_mouse_position_impl", das::SideEffects::accessGlobal, "dn_get_mouse_position_impl");
        das::addExtern<DAS_BIND_FUN(dn_get_mouse_global_position_impl)>(
            *this,
            lib,
            "dn_get_mouse_global_position_impl",
            das::SideEffects::accessGlobal,
            "dn_get_mouse_global_position_impl");
        das::addExtern<DAS_BIND_FUN(dn_get_mouse_delta_impl)>(
            *this, lib, "dn_get_mouse_delta_impl", das::SideEffects::accessGlobal, "dn_get_mouse_delta_impl");
        das::addExtern<DAS_BIND_FUN(dn_get_mouse_wheel_move_impl)>(
            *this, lib, "dn_get_mouse_wheel_move_impl", das::SideEffects::accessGlobal, "dn_get_mouse_wheel_move_impl");
        das::addExtern<DAS_BIND_FUN(dn_get_mouse_wheel_move_v_impl)>(
            *this,
            lib,
            "dn_get_mouse_wheel_move_v_impl",
            das::SideEffects::accessGlobal,
            "dn_get_mouse_wheel_move_v_impl");
        das::addExtern<DAS_BIND_FUN(dn_set_mouse_position_impl)>(
            *this, lib, "dn_set_mouse_position_impl", das::SideEffects::modifyExternal, "dn_set_mouse_position_impl")
            ->args({"x", "y"});
        das::addExtern<DAS_BIND_FUN(dn_set_mouse_offset_impl)>(
            *this, lib, "dn_set_mouse_offset_impl", das::SideEffects::modifyExternal, "dn_set_mouse_offset_impl")
            ->args({"ox", "oy"});
        das::addExtern<DAS_BIND_FUN(dn_set_mouse_scale_impl)>(
            *this, lib, "dn_set_mouse_scale_impl", das::SideEffects::modifyExternal, "dn_set_mouse_scale_impl")
            ->args({"sx", "sy"});
        das::addExtern<DAS_BIND_FUN(dn_set_mouse_cursor_impl)>(
            *this, lib, "dn_set_mouse_cursor_impl", das::SideEffects::modifyExternal, "dn_set_mouse_cursor_impl")
            ->args({"cursor"});

        // InputActions functions
        das::addExtern<DAS_BIND_FUN(dn_create_input_action_impl)>(
            *this, lib, "dn_create_input_action_impl", das::SideEffects::modifyExternal, "dn_create_input_action_impl")
            ->args({"name"});
        das::addExtern<DAS_BIND_FUN(dn_is_input_action_triggered_impl)>(
            *this,
            lib,
            "dn_is_input_action_triggered_impl",
            das::SideEffects::accessGlobal,
            "dn_is_input_action_triggered_impl")
            ->args({"name"});
        das::addExtern<DAS_BIND_FUN(dn_remove_input_action_impl)>(
            *this, lib, "dn_remove_input_action_impl", das::SideEffects::modifyExternal, "dn_remove_input_action_impl")
            ->args({"name"});
        das::addExtern<DAS_BIND_FUN(dn_add_keyboard_binding_impl)>(
            *this,
            lib,
            "dn_add_keyboard_binding_impl",
            das::SideEffects::modifyExternal,
            "dn_add_keyboard_binding_impl")
            ->args({"action", "key", "event", "modifier"});
        das::addExtern<DAS_BIND_FUN(dn_add_mouse_binding_impl)>(
            *this, lib, "dn_add_mouse_binding_impl", das::SideEffects::modifyExternal, "dn_add_mouse_binding_impl")
            ->args({"action", "button", "event"});
        das::addExtern<DAS_BIND_FUN(dn_remove_keyboard_binding_impl)>(
            *this,
            lib,
            "dn_remove_keyboard_binding_impl",
            das::SideEffects::modifyExternal,
            "dn_remove_keyboard_binding_impl")
            ->args({"action", "key", "event", "modifier"});
        das::addExtern<DAS_BIND_FUN(dn_remove_mouse_binding_impl)>(
            *this,
            lib,
            "dn_remove_mouse_binding_impl",
            das::SideEffects::modifyExternal,
            "dn_remove_mouse_binding_impl")
            ->args({"action", "button", "event"});

        DN_CORE_INFO("Script Module [dn_input] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnInput);
REGISTER_DYN_MODULE(Module_DnInput, Module_DnInput);
