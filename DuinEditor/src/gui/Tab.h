#pragma once

#include "PanelManager.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>

#include <string>


class Tab
{
    public:
        std::string title;
        bool open = true;
        bool renaming = false;
        bool flag_requestDelete = false;

        Tab() = default;
        Tab(duin::DataValue value);

        std::string GetPanelManagerID();

        bool operator==(Tab& o)
        {
            return !this->title.compare(o.title);
        }

        bool operator!=(Tab& o)
        {
            return this->title.compare(o.title);
        }

        duin::UUID GetUUID();

        duin::DataValue Serialise(); // Serialise tab to toml value
        void Deserialise(duin::DataValue value); // Deserialise tab from toml value

        void DrawWorkspace();

    private:
        duin::UUID uuid;
        PanelManager panelManager;

        void DrawMenu();
};
