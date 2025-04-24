#include "TabBrowser.h"


TabBrowser::TabBrowser()
{
    AddTab("New Tab");
}

void TabBrowser::AddTab(const std::string& title) 
{    
    static int counter = 1;
    Tab newTab;
    newTab.title = title + "##" + std::to_string(counter);
    newTab.id = counter++;
    tabs.push_back(newTab);
    selectedTab = tabs.size() - 1;
}
