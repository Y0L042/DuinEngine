#pragma once

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/IO/IOModule.h>

namespace duin {
    class Event;
    class DataValue;
}

class Editor : public duin::Application
{
    public:
        static Editor& Get();

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

        static void SaveProjectEditorConfig(duin::DataValue value);
        static duin::DataValue LoadProjectEditorConfig();

private:
    static Editor* instance;
};

