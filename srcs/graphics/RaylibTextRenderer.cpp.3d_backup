#include "../../incs/RaylibTextRenderer.hpp"
#include "../../incs/RaylibGraphic.hpp"
#include <rlgl.h>

RaylibTextRenderer::RaylibTextRenderer(RaylibGraphic& parent) : graphic(parent) {
	loadFont();
}

RaylibTextRenderer::~RaylibTextRenderer() {
	UnloadFont(customFont);
}

void RaylibTextRenderer::loadFont() {
	// Load font with Unicode support
	// Create array with all codepoints we need
	int codepointCount = 256 - 32 + 8;  // ASCII printable (32-255) + arrows + dot
	int *codepoints = new int[codepointCount];
	
	// Load basic ASCII printable characters (32-255)
	for (int i = 0; i < (256 - 32); i++) {
		codepoints[i] = 32 + i;
	}
	
	// Add Unicode arrows and middle dot
	codepoints[256 - 32 + 0] = 0x2191;  // ↑
	codepoints[256 - 32 + 1] = 0x2193;  // ↓
	codepoints[256 - 32 + 2] = 0x2190;  // ←
	codepoints[256 - 32 + 3] = 0x2192;  // →
	codepoints[256 - 32 + 4] = 0x00B7;  // · (middle dot)
	codepoints[256 - 32 + 5] = 0x2022;  // • (bullet point, alternative)
	codepoints[256 - 32 + 6] = 0x00B7;  // · (duplicate for safety)
	codepoints[256 - 32 + 7] = 0x002E;  // . (period, fallback)
	
	customFont = LoadFontEx("fonts/JetBrainsMono-VariableFont_wght.ttf", 64, codepoints, codepointCount);
	delete[] codepoints;
}

void RaylibTextRenderer::drawModes(const GameState &state) {
	Vector3 textPosition = { 0.0f, 0.0f, 0.0f };
	float fontSize = 2.0f;
	float fontSpacing = 0.20f;
	float lineSpacing = 1.0f;

	switch (state.config.mode) {
		case GameMode::SINGLE:
			DrawText3D(customFont, "SINGLE               ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.snakeALightSide, 90.0f, 7.0f, 5.0f);
			DrawText3D(customFont, "       - MULTI - VsAI", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 5.0f);
			break;

		case GameMode::MULTI:
			DrawText3D(customFont, "         MULTI       ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.snakeBLightTop, 90.0f, 7.0f, 5.0f);
			DrawText3D(customFont, "SINGLE -       - VsAI", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 5.0f);
			break;

		case GameMode::AI:
			DrawText3D(customFont, "                 VsAI", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.snakeAILightTop, 90.0f, 7.0f, 5.0f);
			DrawText3D(customFont, "SINGLE - MULTI -     ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 5.0f);
			break;
	}	
}

void RaylibTextRenderer::drawInstructions(const GameState &state) {
	Vector3 textPosition = { 0.0f, 0.0f, 0.0f };
	float fontSize = 2.0f;
	float fontSpacing = 0.20f;
	float lineSpacing = 1.0f;

	drawModes(state);
	
	textPosition.x += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ ENTER ]              START", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 90.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "          ············      ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 7.0f);

	textPosition.x += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ SPACE ]               MODE", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 90.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "          ·············     ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 7.0f);
	
	textPosition.x += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ ↑ ↓ ← → ]             MOVE", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 90.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "            ············     ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 7.0f);
	
	textPosition.x += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ 1   2   3 ]         TRAVEL", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 90.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "    /   /     ·······       ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 7.0f);
	
	textPosition.x += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ Q   ESC ]             QUIT", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 90.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "    /       ···········     ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 90.0f, 7.0f, 7.0f);
}

void RaylibTextRenderer::DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint) {
	// Character index position in sprite font
	int index = GetGlyphIndex(font, codepoint);
	float scale = fontSize / (float)font.baseSize;
	
	// Character destination rectangle on screen
	position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding) * scale;
	position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding) * scale;
	
	// Character source rectangle from font texture atlas
	Rectangle srcRec = { 
		font.recs[index].x - (float)font.glyphPadding, 
		font.recs[index].y - (float)font.glyphPadding,
		font.recs[index].width + 2.0f * font.glyphPadding, 
		font.recs[index].height + 2.0f * font.glyphPadding 
	};
	
	float width = (float)(font.recs[index].width + 2.0f * font.glyphPadding) * scale;
	float height = (float)(font.recs[index].height + 2.0f * font.glyphPadding) * scale;
	
	if (font.texture.id > 0) {
		const float x = 0.0f;
		const float y = 0.0f;
		const float z = 0.0f;
		
		// Normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
		const float tx = srcRec.x / font.texture.width;
		const float ty = srcRec.y / font.texture.height;
		const float tw = (srcRec.x + srcRec.width) / font.texture.width;
		const float th = (srcRec.y + srcRec.height) / font.texture.height;
		
		rlCheckRenderBatchLimit(4 + 4 * backface);
		rlSetTexture(font.texture.id);
		
		rlPushMatrix();
		rlTranslatef(position.x, position.y, position.z);
		
		rlBegin(RL_QUADS);
		rlColor4ub(tint.r, tint.g, tint.b, tint.a);
		
		// Front Face
		rlNormal3f(0.0f, 1.0f, 0.0f);
		rlTexCoord2f(tx, ty); rlVertex3f(x, y, z);
		rlTexCoord2f(tx, th); rlVertex3f(x, y, z + height);
		rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);
		rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);
		
		if (backface) {
			// Back Face
			rlNormal3f(0.0f, -1.0f, 0.0f);
			rlTexCoord2f(tx, ty); rlVertex3f(x, y, z);
			rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);
			rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);
			rlTexCoord2f(tx, th); rlVertex3f(x, y, z + height);
		}
		rlEnd();
		rlPopMatrix();
		
		rlSetTexture(0);
	}
}

void RaylibTextRenderer::DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint, float YRotation, float YOffset, float XOffset) {
    int length = TextLength(text);
    
    float textOffsetY = graphic.cubeSize * YOffset;
    float textOffsetX = -graphic.cubeSize * XOffset;
    
    float scale = fontSize / (float)font.baseSize;
    
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(YRotation, 0.0f, 1.0f, 0.0f);
    
    for (int i = 0; i < length;) {
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);
        
        if (codepoint == 0x3f) codepointByteCount = 1;
        
        if (codepoint == '\n') {
            textOffsetY += fontSize + lineSpacing;
            textOffsetX = 0.0f;
        } else {
            if ((codepoint != ' ') && (codepoint != '\t')) {
                DrawTextCodepoint3D(font, codepoint, 
                    (Vector3){ textOffsetX, 0.0f, textOffsetY },
                    fontSize, backface, tint);
            }
            
            if (font.glyphs[index].advanceX == 0) {
                textOffsetX += (float)font.recs[index].width * scale + fontSpacing;
            } else {
                textOffsetX += (float)font.glyphs[index].advanceX * scale + fontSpacing;
            }
        }
        
        i += codepointByteCount;
    }
    
    rlPopMatrix();
}

void RaylibTextRenderer::drawWinner(const GameState& state) {
	enum result {
		A_win,
		B_win,
		Draw
	};
	
	result res;
	std::string winnerText;
	Color textColor;
	
	Vector3 textPosition = { 45.0f, 34.0f, 0.0f };  // Position above the score text
	float fontSize = 3.0f;
	float fontSpacing = 0.20f;
	float lineSpacing = 1.0f;
	float conversionFactor = 0.020f;
	
	if (state.snake_A.isDead() && state.snake_B->isDead()) {
		res = Draw;
	} else if (state.snake_A.isDead()) {
		res = B_win;
	} else if (state.snake_B->isDead()) {
		res = A_win;
	} else {
		if (state.score > state.scoreB) {
			res = A_win;
		} else if (state.scoreB > state.score) {
			res = B_win;
		} else {
			res = Draw;
		}
	}
	
	if (state.score == 0 && state.score == state.scoreB && state.snake_A.isDead() && state.snake_B->isDead()) {
		res = Draw;
	}
	
	switch (res) {
		case A_win:
			winnerText = "PLAYER 1 WINS";
			textColor = graphic.snakeALightSide;
			break;
		
		case B_win:
			if (state.config.mode == GameMode::AI) {
				winnerText = "AI WINS";
				textColor = graphic.snakeAILightTop;
			} else {
				winnerText = "PLAYER 2 WINS";
				textColor = graphic.snakeBLightTop;
			}
			break;
		
		case Draw:
			winnerText = "MATCH ENDED IN DRAW";
			textColor = graphic.customWhite;
			break;
	}
	
	Vector2 textSize = MeasureTextEx(customFont, winnerText.c_str(), fontSize * customFont.baseSize, fontSpacing);
	float textWidth = textSize.x * conversionFactor;
	
	textPosition.x -= textWidth / 2.0f;
	
	DrawText3D(customFont, winnerText.c_str(), textPosition, fontSize, fontSpacing, lineSpacing, false, 
	           textColor, 0.0f, 7.0f, 7.0f);
}

void RaylibTextRenderer::drawRetry(const GameState& state) {
	Vector3 textPosition = { 4.0f, -2.0f, 0.0f };
	float fontSize = 2.0f;
	float fontSpacing = 0.20f;
	float lineSpacing = 1.0f;
	float conversionFactor = 0.020f;
	
	if (state.config.mode != GameMode::SINGLE && state.snake_B) {
		// MULTIPLAYER MODE
		
		// p1 score
		std::string scoreNumA = std::to_string(state.score);
		std::string appleWordA = (state.score == 1) ? "APPLE" : "APPLES";
		
		const char* player1Text = "PLAYER 1 ";
		const char* ateAText = "ATE ";
		const char* scoreAText = scoreNumA.c_str();
		
		Vector2 player1Size = MeasureTextEx(customFont, player1Text, fontSize * customFont.baseSize, fontSpacing);
		Vector2 ateASize = MeasureTextEx(customFont, ateAText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 scoreASize = MeasureTextEx(customFont, scoreAText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 appleASize = MeasureTextEx(customFont, appleWordA.c_str(), fontSize * customFont.baseSize, fontSpacing);
		
		float player1Width = player1Size.x * conversionFactor;
		float ateAWidth = ateASize.x * conversionFactor;
		float scoreAWidth = scoreASize.x * conversionFactor;
		float appleAWidth = appleASize.x * conversionFactor;
		
		float totalWidthA = player1Width + ateAWidth + scoreAWidth + appleAWidth;
		
		Vector3 currentPosA = textPosition;
		currentPosA.x -= totalWidthA / 2.0f - 8.0f;  //centering
		
		// p1
		DrawText3D(customFont, player1Text, currentPosA, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.snakeALightSide, 0.0f, 7.0f, 7.0f);
		currentPosA.x += player1Width;
		
		// ate
		DrawText3D(customFont, ateAText, currentPosA, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);
		currentPosA.x += ateAWidth;
		
		// score
		float scoreAInterset = (static_cast<float>((scoreNumA.length())) * 0.075f) + 0.2f;
		DrawText3D(customFont, scoreAText, currentPosA, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.foodSide, 0.0f, 7.0f, 7.0f + scoreAInterset);
		currentPosA.x += scoreAWidth + 0.3f;
		
		// apples
		DrawText3D(customFont, appleWordA.c_str(), currentPosA, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);
		
		// p2 score
		std::string scoreNumB = std::to_string(state.scoreB);
		std::string appleWordB = (state.scoreB == 1) ? "APPLE" : "APPLES";
		
		const char* player2Text = (state.config.mode == GameMode::AI) ? "AI " : "PLAYER 2 ";
		const char* ateBText = "ATE ";
		const char* scoreBText = scoreNumB.c_str();
		
		Vector2 player2Size = MeasureTextEx(customFont, player2Text, fontSize * customFont.baseSize, fontSpacing);
		Vector2 ateBSize = MeasureTextEx(customFont, ateBText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 scoreBSize = MeasureTextEx(customFont, scoreBText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 appleBSize = MeasureTextEx(customFont, appleWordB.c_str(), fontSize * customFont.baseSize, fontSpacing);
		
		float player2Width = player2Size.x * conversionFactor;
		float ateBWidth = ateBSize.x * conversionFactor;
		float scoreBWidth = scoreBSize.x * conversionFactor;
		float appleBWidth = appleBSize.x * conversionFactor;
		
		float totalWidthB = player2Width + ateBWidth + scoreBWidth + appleBWidth;
		
		Vector3 currentPosB = textPosition;
		currentPosB.x -= totalWidthB / 2.0f - 8.0f;			//centering
		currentPosB.z += fontSize + lineSpacing + 1.0f;
		
		// p2
		Color player2Color = (state.config.mode == GameMode::AI) ? graphic.snakeAILightTop : graphic.snakeBLightTop;
		DrawText3D(customFont, player2Text, currentPosB, fontSize, fontSpacing, lineSpacing, false, 
		           player2Color, 0.0f, 7.0f, 7.0f);
		currentPosB.x += player2Width;
		
		// ate
		DrawText3D(customFont, ateBText, currentPosB, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);
		currentPosB.x += ateBWidth;
		
		// score
		float scoreBInterset = (static_cast<float>((scoreNumB.length())) * 0.075f) + 0.2f;
		DrawText3D(customFont, scoreBText, currentPosB, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.foodSide, 0.0f, 7.0f, 7.0f + scoreBInterset);
		currentPosB.x += scoreBWidth + 0.3f;
		
		// apples
		DrawText3D(customFont, appleWordB.c_str(), currentPosB, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);
		
		// Retry instructions
		textPosition.x -= 7.0f;
		textPosition.z += (fontSize + lineSpacing + 1.0f) * 3.0f;  // Skip 3 lines
		
	} else {
		// SINGLE PLAYER MODE
		std::string scoreNum = std::to_string(state.score);
		std::string appleWord = (state.score == 1) ? "APPLE" : "APPLES";
		
		const char* youText = "YOU ";
		const char* ateText = "ATE ";
		const char* scoreText = scoreNum.c_str();
		
		Vector2 youSize = MeasureTextEx(customFont, youText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 ateSize = MeasureTextEx(customFont, ateText, fontSize * customFont.baseSize, fontSpacing);
		Vector2 scoreSize = MeasureTextEx(customFont, scoreText, fontSize * customFont.baseSize, fontSpacing);
		
		float youWidth = youSize.x * conversionFactor;
		float ateWidth = ateSize.x * conversionFactor;
		float scoreWidth = scoreSize.x * conversionFactor;
		
		Vector3 currentPos = textPosition;
		
		// you
		DrawText3D(customFont, youText, currentPos, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.snakeALightSide, 0.0f, 7.0f, 7.0f);
		currentPos.x += youWidth;
		
		// ate
		DrawText3D(customFont, ateText, currentPos, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);
		currentPos.x += ateWidth;
		
		// score
		float scoreInterset = (static_cast<float>((scoreNum.length())) * 0.075f) + 0.2f;
		DrawText3D(customFont, scoreText, currentPos, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.foodSide, 0.0f, 7.0f, 7.0f + scoreInterset);
		currentPos.x += scoreWidth + 0.3f;
		
		// apples
		DrawText3D(customFont, appleWord.c_str(), currentPos, fontSize, fontSpacing, lineSpacing, false, 
		           graphic.customWhite, 0.0f, 7.0f, 7.0f);

		textPosition.x -= 7.0f;
		textPosition.z += fontSize + lineSpacing + 1.0f;
	}
	
	// Common retry/quit instructions
	DrawText3D(customFont, "[ ENTER ]              RETRY", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 0.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "          ············      ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 0.0f, 7.0f, 7.0f);

	textPosition.z += fontSize + lineSpacing + 1.0f;
	DrawText3D(customFont, "[ Q   ESC ]             QUIT", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customWhite, 0.0f, 7.0f, 7.0f);
	DrawText3D(customFont, "    /       ···········     ", textPosition, fontSize, fontSpacing, lineSpacing, false, graphic.customGray, 0.0f, 7.0f, 7.0f);
}

Font& RaylibTextRenderer::getFont() { return customFont; }