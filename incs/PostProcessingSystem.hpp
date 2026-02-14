#pragma once
#include <raylib.h>
#include <vector>
#include <memory>

enum class PostProcessEffect {
	None,
	CRT,
	Scanlines,
	Bloom,
	Vignette,
	ChromaticAberration,
	Grain
};

struct PostProcessConfig {
	bool enabled = true;
	std::vector<PostProcessEffect> effects;

	// CRT params
	float scanlineIntensity = 0.15f;
	float curvatureAmount = 0.01f;
	float vignetteStrength = 0.3f;
	float chromaticAberration = 0.005f;
	float bloomIntensity = 0.3f;
	float grainAmount = 0.02f;
};

class PostProcessingSystem {
	private:    
		RenderTexture2D renderTarget;
		RenderTexture2D bloomBuffer;

	Shader crtShader;				// Main CRT base shader
	Shader crtBloomShader;			// Combined CRT + Bloom
	Shader bloomExtractShader;		// Extract bright areas
	Shader blurShader;				// Gaussian blur

	RenderTexture2D pingPongBuffers[2];  // For blur passes

	PostProcessConfig config;		int screenWidth;
		int screenHeight;
		float time;

		// Helpers
		void applyBloom();
		void applyCRT();
		void applyEffect(PostProcessEffect effect);

	public:
		PostProcessingSystem();
		~PostProcessingSystem();

		void init(int width, int height);
		void setConfig(const PostProcessConfig& config);

		// rendering flow
		void beginCapture();
		void endCapture();
		void applyAndPresent(float deltaTime);

	// presets
		static PostProcessConfig presetCRT();
		static PostProcessConfig presetCRTBloom();  // CRT + Bloom combined
		static PostProcessConfig presetClean();
		static PostProcessConfig presetMenu();		// runtime control
		void toggleEffect(PostProcessEffect effect);
		void setEffectIntensity(PostProcessEffect effect, float intensity);

		RenderTexture2D& getRenderTarget() { return renderTarget; }
};