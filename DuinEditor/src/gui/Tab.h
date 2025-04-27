#pragma once

#include "PanelManager.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Utils/TOMLFile.h>

#include <string>


class Tab
{
    public:
        std::string title;
        bool open = true;
        bool renaming = false;
        bool flag_requestDelete = false;

        Tab() = default;
        Tab(duin::TOMLValue value);

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

        duin::TOMLValue Serialise();
        void Deserialise(duin::TOMLValue value);

    private:
        duin::UUID uuid;
        PanelManager panelManager;
};
