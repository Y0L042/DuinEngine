#include "Panel.h"
#include "PanelManager.h"
#include "GuiMeta.h"
#include "Tab.h"

#include <Duin/Core/Debug/DebugModule.h>

void DrawPanelMenu(PanelManager *panelManager)
{
    static int counter = 0;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Add Panel")) {
            if (ImGui::MenuItem("Add Default Panel")) {
                const std::string panelName = "Default_Panel_" + std::to_string(++counter);
                panelManager->CreatePanel(Panel::PanelType::DEFAULT, panelName, panelManager);
            }
            if (ImGui::MenuItem("Add Viewport Panel")) {
                const std::string panelName = "Viewport_Panel_" + std::to_string(++counter);
                panelManager->CreatePanel(Panel::PanelType::VIEWPORT, panelName, panelManager);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}



Panel::Panel(const std::string& name, PanelManager *panelManager)
    : uuid(duin::UUID()), panelManager(panelManager), panelName(name) 
{
    uniqueWindowName = CreateUniquePanelName();
    m_windowFlags = ImGuiWindowFlags_MenuBar | 
                    ImGuiWindowFlags_NoCollapse;
    AddMenuItem("Panel", "Remove Panel", [this]() mutable { this->panelManager->RemovePanel(uuid); });
}

Panel::Panel(const std::string& name, duin::UUID uuid, PanelManager* panelManager)
    : uuid(uuid), panelManager(panelManager), panelName(name)
{
    uniqueWindowName = CreateUniquePanelName();
    m_windowFlags = ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse;

    AddMenuItem("Panel", "Remove Panel", [this]() mutable { this->panelManager->RemovePanel(this->uuid); });
}

Panel::Panel(duin::DataValue value)
{
    Deserialise(value);
    m_windowFlags = ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse;

    AddMenuItem("Panel", "Remove Panel", [this]() mutable { this->panelManager->RemovePanel(this->uuid); });
}

void Panel::SetBlackboard(std::shared_ptr<TabBlackboard> b)
{
    DNASSERT((b != nullptr), "Blackboard cannot be NULL!");
    blackboard = b;
    if (blackboard->onFocusChange) {
        DN_INFO("Panel connected to blackboard signal");
        blackboard->onFocusChange->Connect([this](bool isFocussed) { isTabOpen = isFocussed; });
    }
    else {
        DN_WARN("Panel could not connect to blackboard signal");
    }
}

duin::UUID Panel::GetUUID()
{
    return uuid;
}

duin::DataValue Panel::Serialise()
{
    duin::DataValue data;
    const char c = '0' + static_cast<int>(type);
    data.AddMember(guitag::PANEL_TYPE, c);
    data.AddMember(guitag::PANEL_UUID, duin::UUID::ToStringHex(uuid));
    data.AddMember(guitag::PANEL_NAME, panelName);
    data.AddMember(guitag::PANEL_WINDOW_NAME, uniqueWindowName);

    return data;
}

void Panel::Deserialise(duin::DataValue data)
{
    if (!data.HasMember(guitag::PANEL_TYPE)) return;
    std::string typeStr = data[guitag::PANEL_TYPE].GetString();
    type = (Panel::PanelType)std::stoi(typeStr.c_str());

    if (!data.HasMember(guitag::PANEL_NAME)) return;
    panelName = data[guitag::PANEL_NAME].GetString();

    if (!data.HasMember(guitag::PANEL_UUID)) return;
    uuid = duin::UUID::FromStringHex(data[guitag::PANEL_UUID].GetString());

    uniqueWindowName = CreateUniquePanelName();
}

std::string Panel::CreateUniquePanelName()
{
    std::string name = panelName + "_" + duin::UUID::ToStringHex(uuid);

    return name;
}

std::pair<std::string, std::string> Panel::SplitPanelName(const std::string& panelName)
{
    std::string name = panelName.substr(0, panelName.find("_"));
    std::string uuidHex = panelName.substr(0, panelName.find("_"));

    return std::pair<std::string, std::string>(name, uuidHex);
}

void Panel::Draw() 
{
    ImGui::Begin(uniqueWindowName.c_str(), nullptr, m_windowFlags);
    prevIsHovered = isHovered;
    prevIsFocussed = isFocussed;
    prevIsTabOpen = isTabOpen;
    isHovered = ImGui::IsWindowHovered();
    isFocussed = ImGui::IsWindowFocused();
    if (!m_menuItems.empty() && ImGui::BeginMenuBar()) {
       SetupMenuBar();
       barHeight = ImGui::GetFrameHeight();
       ImGui::EndMenuBar();
    }

    DrawContent();

    ImGui::End();
}

void Panel::RunOnPanelEvent()
{
    PanelEvent e;
    if (ownerTab && (isTabOpen != prevIsTabOpen)) {
        e.type |= PanelEvent::Type::TAB_FOCUSSED;
        e.isTabFocussed = isTabOpen;
    }
    if (prevIsHovered != isHovered) {
        e.type |= PanelEvent::Type::IS_HOVERED;
        e.isHovered = isHovered;
    }
    if (prevIsFocussed != isFocussed) {
        e.type |= PanelEvent::Type::IS_FOCUSSED;
        e.isFocussed = isFocussed;
    }
    if (e.type != PanelEvent::Type::NONE) {
        OnPanelEvent(e);
    }
}

void Panel::SetupMenuBar() 
{
    for (auto& [menuName, items] : m_menuItems) {
        if (ImGui::BeginMenu(menuName.c_str())) {

            size_t separatorIndex = 0;
            size_t nextSeparator = m_menuSeparators.count(menuName) && !m_menuSeparators[menuName].empty() 
                ? m_menuSeparators[menuName][0] 
                : SIZE_MAX;

            for (size_t i = 0; i < items.size(); ++i) {
                if (i == nextSeparator) {
                    ImGui::Separator();
                    separatorIndex++;
                    nextSeparator = separatorIndex < m_menuSeparators[menuName].size() 
                        ? m_menuSeparators[menuName][separatorIndex] 
                        : SIZE_MAX;
                }
                if (ImGui::MenuItem(items[i].label.c_str())) {
                    items[i].callback();
                }
            }
            ImGui::EndMenu();
        }
    }
}

void Panel::AddMenuItem(const std::string& menuName, const std::string& itemName, std::function<void()> callback) 
{
    m_menuItems[menuName].push_back({itemName, callback});
}

void Panel::AddSeparator(const std::string& menuName) 
{
    if (!m_menuItems.count(menuName)) return;
    m_menuSeparators[menuName].push_back(m_menuItems[menuName].size());
}

void Panel::DrawMenu()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Choose Panel Type")) {
            if (ImGui::MenuItem("")) {
                // TODO
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
