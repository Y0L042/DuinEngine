#pragma once

namespace duin {
    class SceneManager
    {
        public:
            SceneManager() = default;
            virtual ~SceneManager() = default;

            void LoadEntity();
            void SaveEntity();
            void UpdateEntity();

        private:
    };
}
