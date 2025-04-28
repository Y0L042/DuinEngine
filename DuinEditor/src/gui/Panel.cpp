#include "Panel.h"
#include "PanelManager.h"
#include "GuiMeta.h"

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

Panel::Panel(duin::TOMLValue value)
{
    Deserialise(value);
    m_windowFlags = ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse;
}

duin::UUID Panel::GetUUID()
{
    return uuid;
}

duin::TOMLValue Panel::Serialise()
{
    duin::TOMLValue tomlValue{ toml::table{} };  // New: initialize as table :contentReference[oaicite:1]{index=1}
    auto & value = tomlValue.GetValue().as_table();  // Changed: work with the table directly
    
    value[guitag::PANEL_TYPE] = std::to_string(type).c_str();
    value[guitag::PANEL_UUID] = duin::UUID::ToStringHex(uuid);
    value[guitag::PANEL_NAME] = panelName;
    value[guitag::PANEL_WINDOW_NAME] = uniqueWindowName;
        
    return tomlValue;

}

void Panel::Deserialise(duin::TOMLValue tomlValue)
{
    toml::value& value = tomlValue.GetValue();

    if (!value.contains(guitag::PANEL_TYPE)) return;
    const char* typeStr = value[guitag::PANEL_TYPE].as_string().c_str();
    type = (Panel::PanelType)atoi(typeStr);

    if (!value.contains(guitag::PANEL_NAME)) return;
    panelName = value[guitag::PANEL_NAME].as_string();

    if (!value.contains(guitag::PANEL_UUID)) return;
    uuid = duin::UUID::FromStringHex(value[guitag::PANEL_UUID].as_string());

    uniqueWindowName = CreateUniquePanelName();
    DN_INFO("Deserialising panel TODO");
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

    if (!m_menuItems.empty() && ImGui::BeginMenuBar()) {
       SetupMenuBar();
       ImGui::EndMenuBar();
    }

    DrawContent();

    ImGui::End();
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
