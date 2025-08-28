#include "PanelList.h"

std::string PanelTypeToName(PanelType type)
{
    std::string panelName = "";
    switch (type) {
    case INVALID:
        panelName = "INVALID";
        break;
    case DEFAULT:
        panelName = "DEFAULT";
        break;
    case SCENETREE:
        panelName = "SCENETREE";
        break;
    case VIEWPORT:
        panelName = "VIEWPORT";
        break;
    case FILEBROWSER:
        panelName = "FILEBROWSER";
        break;
    default:
        panelName = "__EMPTY__";
        break;
    }

    return panelName;
}

PanelType NameToPanelType(const std::string& name)
{
    PanelType type = DEFAULT;
    if (name == "INVALID") type = INVALID;
    if (name == "DEFAULT") type = DEFAULT;
    if (name == "SCENETREE") type = SCENETREE;
    if (name == "VIEWPORT") type = VIEWPORT;
    if (name == "FILEBROWSER") type = FILEBROWSER;

    return type;
}
