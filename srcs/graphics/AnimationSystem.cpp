#include "../../incs/AnimationSystem.hpp"

AnimationSystem::AnimationSystem() :
    tunnelEffectEnabled(false),
    shakeTimer(0.0f),
    shakeOffset({0.0f, 0.0f}),
    screenWidth(1920),
    screenHeight(1080) {
    lastTunnelSpawn = std::chrono::high_resolution_clock::now();
}

void AnimationSystem::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;
}

void AnimationSystem::enableTunnelEffect(bool enabled, const TunnelConfig &config) {
    tunnelEffectEnabled = enabled;
    currentTunnelConfig = config;
    if (!enabled) {
        clearTunnelEffect();
    }
}

void AnimationSystem::clearTunnelEffect() {
    tunnelLines.clear();
    lastTunnelSpawn = std::chrono::high_resolution_clock::now();
}

void AnimationSystem::updateTunnelEffect(float deltaTime) {
    if (!tunnelEffectEnabled) return;

    // Age existing lines and update their progress
    for (auto& line : tunnelLines) {
        line.age += deltaTime;
        line.progress = easeInQuad(line.age * currentTunnelConfig.animationSpeed);
    }

    // Remove lines that have completed their animation
    tunnelLines.erase(
        std::remove_if(tunnelLines.begin(), tunnelLines.end(),
            [](const TunnelLine& line) { return line.progress >= 1.0f; }),
        tunnelLines.end()
    );

    // Spawn new lines at regular intervals
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - lastTunnelSpawn;
    
    if (elapsed.count() >= currentTunnelConfig.spawnInterval) {
        if (static_cast<int>(tunnelLines.size()) < currentTunnelConfig.maxLines) {
            tunnelLines.push_back(TunnelLine());
        }
        lastTunnelSpawn = now;
    }
}

void AnimationSystem::renderTunnelEffect() const {
    if (!tunnelEffectEnabled || tunnelLines.empty()) return;

    int borderThickness = currentTunnelConfig.borderThickness;
    int contentInset = currentTunnelConfig.contentInset;
    
    int startLeft = borderThickness + contentInset;
    int startTop = borderThickness + contentInset;
    int startRight = screenWidth - borderThickness - contentInset;
    int startBottom = screenHeight - borderThickness - contentInset;

    int maxTravelX = contentInset;
    int maxTravelY = contentInset;

    for (const auto& line : tunnelLines) {
        float easedProgress = easeInQuad(line.progress);
        int travelX = static_cast<int>(easedProgress * maxTravelX);
        int travelY = static_cast<int>(easedProgress * maxTravelY);

        int left = startLeft - travelX;
        int top = startTop - travelY;
        int right = startRight + travelX;
        int bottom = startBottom + travelY;

        // Apply fade-in effect based on progress
        unsigned char alpha = static_cast<unsigned char>(line.progress * 255);
        Color fadedColor = currentTunnelConfig.lineColor;
        fadedColor.a = alpha;

        // Top line
        DrawLine(left, top, right, top, fadedColor);
        // Bottom line
        DrawLine(left, bottom, right, bottom, fadedColor);
        // Left line
        DrawLine(left, top, left, bottom, fadedColor);
        // Right line
        DrawLine(right, top, right, bottom, fadedColor);
    }
}

void AnimationSystem::triggerScreenShake(const ScreenShakeConfig &config) {
    shakeConfig = config;
    shakeTimer = config.duration;
}

void AnimationSystem::updateScreenShake(float deltaTime) {
    if (shakeTimer <= 0.0f) {
        shakeOffset = {0.0f, 0.0f};
        return;
    }

    shakeTimer -= deltaTime;
    
    // Simple random shake within intensity bounds
    float intensity = shakeConfig.intensity * (shakeTimer / shakeConfig.duration);
    shakeOffset.x = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * intensity;
    shakeOffset.y = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * intensity;
}