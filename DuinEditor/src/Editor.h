#pragma once

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Utils/TOMLFile.h>


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

        static void SaveTOMLConfig(duin::TOMLValue value);
        static duin::TOMLValue LoadTOMLConfig();

private:
    static Editor* instance;
};

