#include "../../incs/AnimationSystem.hpp"

AnimationSystem::AnimationSystem() :
	tunnelLineThickness(2.0),
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

		Vector2 topLeftCorner{static_cast<float>(left), static_cast<float>(top)};
		Vector2 topRightCorner{static_cast<float>(right), static_cast<float>(top)};
		Vector2 bottomLeftCorner{static_cast<float>(left), static_cast<float>(bottom)};
		Vector2 bottomRightCorner{static_cast<float>(right), static_cast<float>(bottom)};

		// Top line
		//DrawLine(left, top, right, top, fadedColor);
		DrawLineEx(topLeftCorner, topRightCorner, tunnelLineThickness, fadedColor);
		// Bottom line
		//DrawLine(left, bottom, right, bottom, fadedColor);
		DrawLineEx(bottomLeftCorner, bottomRightCorner, tunnelLineThickness, fadedColor);
		// Left line
		DrawLineEx(topLeftCorner, bottomLeftCorner, tunnelLineThickness, fadedColor);
		// Right line
		DrawLineEx(topRightCorner, bottomRightCorner, tunnelLineThickness, fadedColor);
	}
}

void AnimationSystem::renderTunnelEffectCustom(int borderLeft, int borderTop, int borderRight, int borderBottom) const {
	if (!tunnelEffectEnabled || tunnelLines.empty()) return;

	int contentInset = currentTunnelConfig.contentInset;
	
	int startLeft = borderLeft + contentInset;
	int startTop = borderTop + contentInset;
	int startRight = borderRight - contentInset;
	int startBottom = borderBottom - contentInset;

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

		Vector2 topLeftCorner{static_cast<float>(left), static_cast<float>(top)};
		Vector2 topRightCorner{static_cast<float>(right), static_cast<float>(top)};
		Vector2 bottomLeftCorner{static_cast<float>(left), static_cast<float>(bottom)};
		Vector2 bottomRightCorner{static_cast<float>(right), static_cast<float>(bottom)};

		// Top line
		//DrawLine(left, top, right, top, fadedColor);
		DrawLineEx(topLeftCorner, topRightCorner, tunnelLineThickness, fadedColor);
		// Bottom line
		//DrawLine(left, bottom, right, bottom, fadedColor);
		DrawLineEx(bottomLeftCorner, bottomRightCorner, tunnelLineThickness, fadedColor);
		// Left line
		DrawLineEx(topLeftCorner, bottomLeftCorner, tunnelLineThickness, fadedColor);
		// Right line
		DrawLineEx(topRightCorner, bottomRightCorner, tunnelLineThickness, fadedColor);
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
	shakeOffset.x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * intensity;
	shakeOffset.y = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * intensity;
}