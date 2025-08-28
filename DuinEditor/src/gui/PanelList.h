#pragma once

#include <string>

enum PanelType {
    INVALID = 0,
    DEFAULT,
    SCENETREE,
    VIEWPORT,
	FILEBROWSER
};

std::string PanelTypeToName(PanelType type);
PanelType NameToPanelType(const std::string& name);

