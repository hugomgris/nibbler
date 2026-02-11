#pragma once

#include "Button.hpp"
#include <vector>

enum class MenuState {
    Start,
    Paused,
    GameOver,
    Options
};

class MenuSystem {
    private:
        MenuState currentState;
        std::vector<Button> buttons;

    public:
        MenuSystem();
        ~MenuSystem() = default;
    
        void setState(MenuState newState);
        void update(const Vector2& mousePos, bool mouseClicked);
        void render();
        Button* getHoveredButton(const Vector2& mousePos) const;
};