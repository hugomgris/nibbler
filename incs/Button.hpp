#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <iostream>
#include <functional>

struct Button {
    Rectangle bounds;
    std::string Text;

    Color outlineColor;
    Color backgroundColor;
    Color textColor;
    Color hoverColor;
    Color textHoverColor;
    Color outlineHoverColor;

    float outlineWidth = 5.0f;

    std::function<void()> onClick;

    bool isHovered(Vector2 mousePos) const {
        return CheckCollisionPointRec(mousePos, bounds);
    }

    void render(bool hovered) const {
        Color currentColor = hovered ? hoverColor : backgroundColor;
        Color currentTextColor = hovered ? textHoverColor : textColor;
        Color currentOutlineColor = hovered ? outlineHoverColor : outlineColor;
        DrawRectangleRec(bounds, currentColor);
        DrawRectangleLinesEx(bounds, outlineWidth, currentOutlineColor);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), Text.c_str(), 20, 1.0f);
        Vector2 textPos = {
            bounds.x + (bounds.width - textSize.x) / 2,
            bounds.y + (bounds.height - textSize.y) / 2
        };
        DrawTextEx(GetFontDefault(), Text.c_str(), textPos, 20, 1.0f, currentTextColor);
    }
};