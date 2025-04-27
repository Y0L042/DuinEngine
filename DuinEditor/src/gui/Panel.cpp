#include "Panel.h"
#include "PanelManager.h"

Panel::Panel(const std::string& name, PanelManager *panelManager)
    : uuid(duin::UUID()), panelManager(panelManager), m_windowName(name) 
{
    m_windowFlags = ImGuiWindowFlags_MenuBar | 
                    ImGuiWindowFlags_NoCollapse;

    AddMenuItem("Panel", "Remove Panel", [this]() mutable { this->panelManager->RemovePanel(uuid); });
}

duin::UUID Panel::GetUUID()
{
    return uuid;
}

void Panel::Draw() 
{
    ImGui::Begin(m_windowName.c_str(), nullptr, m_windowFlags);

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
