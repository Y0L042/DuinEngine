#pragma once

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>

class Editor : public duin::Application
{
    public:
        Editor() {}
        ~Editor() {}

        void Initialize() override;
        void Ready() override;
        void OnEvent(duin::Event event) override;
        void Update(double rDelta) override;
        void PhysicsUpdate(double pDelta) override;
        void Draw() override;
        void DrawUI() override;
        void Debug() override;
};

