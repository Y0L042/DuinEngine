#pragma once

#include "Panel.h"

#include <string>

class ViewportPanel : public Panel
{
    public:
        ViewportPanel(const std::string& name);

        void DrawContent() override;
    private:
};
