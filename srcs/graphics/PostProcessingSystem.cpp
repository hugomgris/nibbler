#include "../../incs/PostProcessingSystem.hpp"
#include <iostream>
#include <array>
#include <algorithm>
#include <cmath>

PostProcessingSystem::PostProcessingSystem() :
	screenWidth(0),
	screenHeight(0),
	time(0.0f) {}

PostProcessingSystem::~PostProcessingSystem() {
	if (renderTarget.id > 0) {
		UnloadRenderTexture(renderTarget);
	}
	if (bloomBuffer.id > 0) {
		UnloadRenderTexture(bloomBuffer);
	}
	if (crtShader.id > 0) {
		UnloadShader(crtShader);
	}
	if (crtBloomShader.id > 0) {
		UnloadShader(crtBloomShader);
	}
	if (bloomExtractShader.id > 0) {
		UnloadShader(bloomExtractShader);
	}
	if (blurShader.id > 0) {
		UnloadShader(blurShader);
	}
	if (pingPongBuffers[0].id > 0) {
		UnloadRenderTexture(pingPongBuffers[0]);
	}
	if (pingPongBuffers[1].id > 0) {
		UnloadRenderTexture(pingPongBuffers[1]);
	}
}

void PostProcessingSystem::init(int width, int height) {
	screenWidth = width;
	screenHeight = height;
	
	// Create render target for offscreen rendering
	renderTarget = LoadRenderTexture(width, height);
	
	// small bloom buffer for perfomance
	bloomBuffer = LoadRenderTexture(width / 2, height / 2);
	
	// Load CRT shader
	crtShader = LoadShader(0, "shaders/crt.fs");
	
	// Load bloom pipeline shaders
	bloomExtractShader = LoadShader(0, "shaders/bloom_extract.fs");
	blurShader = LoadShader(0, "shaders/blur.fs");
	crtBloomShader = LoadShader(0, "shaders/crt_bloom.fs");
	
	// Create ping-pong buffers for blur passes
	pingPongBuffers[0] = LoadRenderTexture(width / 2, height / 2);
	pingPongBuffers[1] = LoadRenderTexture(width / 2, height / 2);
	
	// Set resolution uniforms (constant)
	int resolutionLoc = GetShaderLocation(crtShader, "resolution");
	float resolution[2] = { (float)width, (float)height };
	SetShaderValue(crtShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
	
	resolutionLoc = GetShaderLocation(crtBloomShader, "resolution");
	SetShaderValue(crtBloomShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
	
	float blurResolution[2] = { (float)width / 2, (float)height / 2 };
	int blurResLoc = GetShaderLocation(blurShader, "resolution");
	SetShaderValue(blurShader, blurResLoc, blurResolution, SHADER_UNIFORM_VEC2);
	
	std::cout << "[PostProcessing] Initialized: " << width << "x" << height << std::endl;
	std::cout << "[PostProcessing] CRT shader loaded successfully" << std::endl;
	std::cout << "[PostProcessing] Bloom pipeline loaded successfully" << std::endl;
}

void PostProcessingSystem::setConfig(const PostProcessConfig& cfg) {
	config = cfg;
}

void PostProcessingSystem::beginCapture() {
	BeginTextureMode(renderTarget);
}

void PostProcessingSystem::endCapture() {
	EndTextureMode();
}

void PostProcessingSystem::applyAndPresent(float deltaTime) {
	time += deltaTime;
	
	if (!config.enabled) {
		// No postprocessing
		DrawTextureRec(
			renderTarget.texture,
			{0, 0, (float)screenWidth, -(float)screenHeight},  // Flip Y
			{0, 0},
			WHITE
		);
		return;
	}
	
	// Check if we should use shader-based CRT and/or Bloom
	bool useCRTShader = false;
	bool useBloom = false;
	
	for (const auto& effect : config.effects) {
		if (effect == PostProcessEffect::CRT) {
			useCRTShader = true;
		}
		if (effect == PostProcessEffect::Bloom) {
			useBloom = true;
		}
	}
	
	// If both CRT and Bloom are enabled, use combined shader
	if (useCRTShader && useBloom && crtBloomShader.id > 0) {
		// Generate bloom texture
		applyBloom();
		
		// Apply combined CRT + Bloom shader
		BeginShaderMode(crtBloomShader);
		
		// Update shader uniforms
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "time"), &time, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "scanlineIntensity"), &config.scanlineIntensity, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "curvatureAmount"), &config.curvatureAmount, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "vignetteStrength"), &config.vignetteStrength, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "chromaticAberration"), &config.chromaticAberration, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "grainAmount"), &config.grainAmount, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtBloomShader, GetShaderLocation(crtBloomShader, "bloomIntensity"), &config.bloomIntensity, SHADER_UNIFORM_FLOAT);
		
		// Bind bloom texture to texture unit 1
		int bloomTexLoc = GetShaderLocation(crtBloomShader, "bloomTexture");
		SetShaderValueTexture(crtBloomShader, bloomTexLoc, pingPongBuffers[1].texture);
		
		// Draw with shader
		DrawTextureRec(
			renderTarget.texture,
			{0, 0, (float)screenWidth, -(float)screenHeight},
			{0, 0},
			WHITE
		);
		
		EndShaderMode();
	}
	else if (useCRTShader && crtShader.id > 0) {
		// Use GPU shader for CRT effect only
		BeginShaderMode(crtShader);
		
		// Update shader uniforms
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "time"), &time, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "scanlineIntensity"), &config.scanlineIntensity, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "curvatureAmount"), &config.curvatureAmount, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "vignetteStrength"), &config.vignetteStrength, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "chromaticAberration"), &config.chromaticAberration, SHADER_UNIFORM_FLOAT);
		SetShaderValue(crtShader, GetShaderLocation(crtShader, "grainAmount"), &config.grainAmount, SHADER_UNIFORM_FLOAT);
		
		// Draw with shader
		DrawTextureRec(
			renderTarget.texture,
			{0, 0, (float)screenWidth, -(float)screenHeight},
			{0, 0},
			WHITE
		);
		
		EndShaderMode();
	} else {
		// Fallback to CPU-based effects
		DrawTextureRec(
			renderTarget.texture,
			{0, 0, (float)screenWidth, -(float)screenHeight},
			{0, 0},
			WHITE
		);
		
		// Apply CPU effects in order
		for (const auto& effect : config.effects) {
			if (effect != PostProcessEffect::CRT && effect != PostProcessEffect::Bloom) {
				applyEffect(effect);
			}
		}
	}
}

void PostProcessingSystem::applyEffect(PostProcessEffect effect) {
	switch (effect) {
		case PostProcessEffect::None:
			break;
			
		case PostProcessEffect::Scanlines: {
			// Simple CPU-based scanlines
			for (int y = 0; y < screenHeight; y += 2) {
				DrawRectangle(0, y, screenWidth, 1, 
					Color{0, 0, 0, (unsigned char)(config.scanlineIntensity * 255)});
			}
			break;
		}
			
		case PostProcessEffect::Vignette: {
			// Simple radial gradient vignette
			int centerX = screenWidth / 2;
			int centerY = screenHeight / 2;
			float maxDist = sqrtf(centerX * centerX + centerY * centerY);
			
			for (int y = 0; y < screenHeight; y += 4) {
				for (int x = 0; x < screenWidth; x += 4) {
					float dx = x - centerX;
					float dy = y - centerY;
					float dist = sqrtf(dx * dx + dy * dy);
					float intensity = (dist / maxDist) * config.vignetteStrength;
					
					if (intensity > 0.01f) {
						DrawRectangle(x, y, 4, 4, 
							Color{0, 0, 0, (unsigned char)(intensity * 255)});
					}
				}
			}
			break;
		}
			
		case PostProcessEffect::Grain: {
			// Random noise overlay
			for (int i = 0; i < 500; i++) {
				int x = GetRandomValue(0, screenWidth);
				int y = GetRandomValue(0, screenHeight);
				unsigned char alpha = (unsigned char)(config.grainAmount * 255);
				DrawPixel(x, y, Color{255, 255, 255, alpha});
			}
			break;
		}
			
		case PostProcessEffect::CRT:
			// Apply multiple CRT effects
			applyEffect(PostProcessEffect::Scanlines);
			applyEffect(PostProcessEffect::Vignette);
			applyEffect(PostProcessEffect::Grain);
			break;
			
		case PostProcessEffect::Bloom:
			applyBloom();
			break;
			
		case PostProcessEffect::ChromaticAberration:
			// TODO: Requires shader implementation
			break;
	}
}

void PostProcessingSystem::applyBloom() {
	// Step 1: Extract bright areas (downsample to half-res)
	BeginTextureMode(bloomBuffer);
	ClearBackground(BLACK);
	BeginShaderMode(bloomExtractShader);
	float threshold = 0.7f;
	SetShaderValue(bloomExtractShader, GetShaderLocation(bloomExtractShader, "threshold"), &threshold, SHADER_UNIFORM_FLOAT);
	
	// Draw full-res texture scaled to half-res buffer
	DrawTexturePro(
		renderTarget.texture,
		{0, 0, (float)screenWidth, -(float)screenHeight},  // Source: full texture, flipped Y
		{0, 0, (float)(screenWidth/2), (float)(screenHeight/2)},  // Dest: half size
		{0, 0},
		0.0f,
		WHITE
	);
	EndShaderMode();
	EndTextureMode();
	
	// Step 2: Blur horizontally
	BeginTextureMode(pingPongBuffers[0]);
	ClearBackground(BLACK);
	BeginShaderMode(blurShader);
	bool horizontal = true;
	SetShaderValue(blurShader, GetShaderLocation(blurShader, "horizontal"), &horizontal, SHADER_UNIFORM_INT);
	DrawTextureRec(bloomBuffer.texture, {0, 0, (float)(screenWidth/2), -(float)(screenHeight/2)}, {0, 0}, WHITE);
	EndShaderMode();
	EndTextureMode();
	
	// Step 3: Blur vertically
	BeginTextureMode(pingPongBuffers[1]);
	ClearBackground(BLACK);
	BeginShaderMode(blurShader);
	horizontal = false;
	SetShaderValue(blurShader, GetShaderLocation(blurShader, "horizontal"), &horizontal, SHADER_UNIFORM_INT);
	DrawTextureRec(pingPongBuffers[0].texture, {0, 0, (float)(screenWidth/2), -(float)(screenHeight/2)}, {0, 0}, WHITE);
	EndShaderMode();
	EndTextureMode();
}

void PostProcessingSystem::applyCRT() {
	// Combined CRT effect
	applyEffect(PostProcessEffect::Scanlines);
	applyEffect(PostProcessEffect::Vignette);
	applyEffect(PostProcessEffect::Grain);
}

PostProcessConfig PostProcessingSystem::presetCRT() {
	PostProcessConfig cfg;
	cfg.enabled = true;
	cfg.effects = { PostProcessEffect::CRT };
	cfg.scanlineIntensity = 0.15f;
	cfg.curvatureAmount = 0.04f;
	cfg.vignetteStrength = 0.6f;
	cfg.chromaticAberration = 0.0015f;
	cfg.grainAmount = 0.05f;
	return cfg;
}

PostProcessConfig PostProcessingSystem::presetClean() {
	PostProcessConfig cfg;
	cfg.enabled = false;
	cfg.effects = {};
	return cfg;
}

PostProcessConfig PostProcessingSystem::presetMenu() {
	PostProcessConfig cfg;
	cfg.enabled = true;
	cfg.effects = { PostProcessEffect::Vignette };
	cfg.vignetteStrength = 0.2f;
	return cfg;
}

// CRT + Bloom preset
PostProcessConfig PostProcessingSystem::presetCRTBloom() {
	PostProcessConfig cfg;
	cfg.enabled = true;
	cfg.effects = { PostProcessEffect::CRT, PostProcessEffect::Bloom };
	cfg.scanlineIntensity = 0.3f;
	cfg.curvatureAmount = 0.04f;
	cfg.vignetteStrength = 0.6f;
	cfg.chromaticAberration = 0.0015f;
	cfg.grainAmount = 0.08f;
	cfg.bloomIntensity = 0.20f;
	return cfg;
}

void PostProcessingSystem::toggleEffect(PostProcessEffect effect) {
	auto it = std::find(config.effects.begin(), config.effects.end(), effect);
	if (it != config.effects.end()) {
		config.effects.erase(it);
	} else {
		config.effects.push_back(effect);
	}
}

void PostProcessingSystem::setEffectIntensity(PostProcessEffect effect, float intensity) {
	switch (effect) {
		case PostProcessEffect::Scanlines:
			config.scanlineIntensity = intensity;
			break;
		case PostProcessEffect::Vignette:
			config.vignetteStrength = intensity;
			break;
		case PostProcessEffect::Grain:
			config.grainAmount = intensity;
			break;
		case PostProcessEffect::Bloom:
			config.bloomIntensity = intensity;
			break;
		case PostProcessEffect::ChromaticAberration:
			config.chromaticAberration = intensity;
			break;
		default:
			break;
	}
}
