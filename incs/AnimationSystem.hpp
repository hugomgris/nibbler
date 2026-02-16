#pragma once

#include <raylib.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

struct TunnelLine {
	float progress;        // 0 = at center, 1 = at edge
	float age;
	
	TunnelLine() : progress(0.0f), age(0.0f) {}
};

struct TunnelConfig {
	int     borderThickness;
	int     contentInset;
	float   spawnInterval;
	float   animationSpeed;
	int     maxLines;
	Color   lineColor;

	// menu default
	static TunnelConfig menu() {
		return {20, 80, 0.3f, 0.5f, 8, Color{70, 130, 180, 255}};
	}

	// 2d realm
	static TunnelConfig realm2D() {
		return {15, 40, 0.15f, 0.7f, 12, Color{144, 238, 144, 255}};
	}
};

// screen shake effect, experimenting
struct ScreenShakeConfig {
	float intensity;
	float duration;
	float frequency;
};

// system
class AnimationSystem {
	private:
		std::vector<TunnelLine>                         tunnelLines;
		std::chrono::high_resolution_clock::time_point  lastTunnelSpawn;
		TunnelConfig                                    currentTunnelConfig;
		ScreenShakeConfig                               shakeConfig;
		bool                                            tunnelEffectEnabled;
		float											tunnelLineThickness;

		// currently unused
		float   shakeTimer;
		Vector2 shakeOffset;

		// screen dimensions
		int screenWidth;
		int screenHeight;

		// Easing helpers
		float easeInQuad(float t) const {
			return t * t;
		}

		float easeOutQuad(float t) const {
			return t * (2 - t);
		}

		float easeInOutCubic(float t) const {
			return t < 0.5f ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
		}

	public:
		AnimationSystem();
		~AnimationSystem() = default;

		void init(int width, int heigth);

	// tunnel effect managers
	void enableTunnelEffect(bool enabled, const TunnelConfig &config = TunnelConfig::menu());
	void updateTunnelEffect(float deltaTime);
	void renderTunnelEffect() const;
	void renderTunnelEffectCustom(int left, int top, int right, int bottom) const;  // Custom border area
	void clearTunnelEffect();		// tunnel effect getters
		const std::vector<TunnelLine>& getTunnelLines() const { return tunnelLines; }
		const TunnelConfig& getTunnelConfig() const { return currentTunnelConfig; }
		bool isTunnelEffectEnabled() const { return tunnelEffectEnabled; }

		// scren shake managers (unsued for now)
		void triggerScreenShake(const ScreenShakeConfig &config);
		void updateScreenShake(float deltaTime);
		Vector2 getScreenShakeOffset() const { return shakeOffset; }

		// fade effect managers (future, placeholdedr for now)
		// void startFade(float duration, Color fromColor, Color toColor);
		// void updateFade(float dt);
		// void renderFade() const;
};