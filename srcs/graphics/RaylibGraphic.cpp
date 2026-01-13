#include "../../incs/IGraphic.hpp"
#include "../../incs/colors.h"
#include <raylib.h>
#include <iostream>

class RaylibGraphic : public IGraphic {
private:
    int cellSize;
    int screenWidth;
    int screenHeight;
    
public:
    RaylibGraphic() : cellSize(20), screenWidth(0), screenHeight(0) {}
    
    void init(int width, int height) override {
        screenWidth = width * cellSize;
        screenHeight = height * cellSize;
        
        InitWindow(screenWidth, screenHeight, "Nibbler - Raylib");
        SetTargetFPS(60);
        
        std::cout << BYEL << "[Raylib] Initialized: " << width << "x" << height << RESET << std::endl;
    }
    
    void render(const GameState& state) override {
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw snake (green)
        for (size_t i = 0; i < state.snake.length; ++i) {
            DrawRectangle(
                state.snake.segments[i].x * cellSize,
                state.snake.segments[i].y * cellSize,
                cellSize,
                cellSize,
                GREEN
            );
        }
        
        // Draw food (red)
        DrawRectangle(
            state.food.position.x * cellSize,
            state.food.position.y * cellSize,
            cellSize,
            cellSize,
            RED
        );
        
        EndDrawing();
    }
    
    Input pollInput() override {
        // Raylib uses IsKeyPressed() - checks if key was just pressed
        if (IsKeyPressed(KEY_UP))    return Input::Up;
        if (IsKeyPressed(KEY_DOWN))  return Input::Down;
        if (IsKeyPressed(KEY_LEFT))  return Input::Left;
        if (IsKeyPressed(KEY_RIGHT)) return Input::Right;
        if (IsKeyPressed(KEY_Q))     return Input::Quit;
        if (IsKeyPressed(KEY_ONE))   return Input::SwitchLib1;
        if (IsKeyPressed(KEY_TWO))   return Input::SwitchLib2;
        if (IsKeyPressed(KEY_THREE)) return Input::SwitchLib3;
        
        if (WindowShouldClose())     return Input::Quit;
        
        return Input::None;
    }
    
    ~RaylibGraphic() {
        CloseWindow();
        std::cout << BYEL << "[Raylib] Destroyed" << RESET << std::endl;
    }
};

extern "C" IGraphic* createGraphic() {
    return new RaylibGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
    delete g;
}
