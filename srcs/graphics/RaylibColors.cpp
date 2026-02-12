#include "../../incs/RaylibColors.hpp"

// Colors
	const Color customWhite = { 255, 248, 227, 255};				// Warm off-white (cream)
	const Color customGray = { 125, 125, 125, 255};				// Mid gray
	const Color customBlack = { 23, 23, 23, 255};					// Deep navy black
	
	// Logo colors - Dark segments (darker overall, balanced contrast)
	const Color logoDarkTop = { 180, 175, 160, 255 };				// Medium-dark cream/beige (brightest of dark set)
	const Color logoDarkFront = { 70, 67, 58, 255 };				// Dark brown-gray (not too close to black)
	const Color logoDarkSide = { 125, 120, 108, 255 };			// Medium warm gray (between top and front)

	// Logo colors - Light segments (lighter overall, more contrast)
	const Color logoLightTop = customWhite;						// Brightest (255, 248, 227)
	const Color logoLightFront = { 170, 165, 148, 255 };			// Medium beige (darker for more contrast)
	const Color logoLightSide = { 215, 208, 188, 255 };			// Light beige (between top and front)

	// Ground colors - Light squares
	const Color groundLightTop = customWhite;
	const Color groundLightFront = { 26, 64, 96, 255 };			// Blue
	const Color groundLightSide = { 254, 74, 81, 255 };			// Red
	const Color groundHidden = customBlack;
	
	// Ground colors - Dark squares
	const Color groundDarkTop = { 200, 195, 178, 255 };			// Darker cream/beige
	const Color groundDarkFront = { 18, 45, 68, 255 };			// Darker blue
	const Color groundDarkSide = { 180, 52, 58, 255 };			// Darker red
	
	// Wall colors
	const Color wallColor = { 147, 112, 219, 255 };				// Medium purple
	const Color wallColorFade = { 147, 112, 219, 120 };			// Faded purple (transparent)
	
	// Snake A colors - Blue shades (matches ground blue)
	// Light segments
	const Color snakeALightTop = { 135, 206, 250, 255 };			// Light sky blue (brightest)
	const Color snakeALightFront = { 26, 64, 96, 255 };			// Ground blue (darker)
	const Color snakeALightSide = { 70, 130, 180, 255 };			// Steel blue (lighter than front)
	
	// Dark segments
	const Color snakeADarkTop = { 70, 130, 180, 255 };			// Steel blue
	const Color snakeADarkFront = { 18, 45, 68, 255 };			// Ground dark blue (darker)
	const Color snakeADarkSide = { 26, 64, 96, 255 };				// Ground blue (lighter than front)
	const Color snakeAHidden = customBlack;
	
	// Snake B colors - Golden/Orange shades (more saturated for better contrast)
	// Light segments
	const Color snakeBLightTop = { 255, 215, 0, 255 };			// Pure golden yellow (brightest)
	const Color snakeBLightFront = { 218, 112, 21, 255 };			// Peru orange (darker, more orange)
	const Color snakeBLightSide = { 255, 165, 0, 255 };			// Pure orange (lighter than front)
	
	// Dark segments
	const Color snakeBDarkTop = { 255, 165, 0, 255 };				// Pure orange
	const Color snakeBDarkFront = { 184, 85, 15, 255 };			// Dark orange (darker)
	const Color snakeBDarkSide = { 218, 112, 21, 255 };			// Peru orange (lighter than front)
	const Color snakeBHidden = customBlack;
	
	// Snake AI colors - Green shades (darker for better contrast)
	// Light segments
	const Color snakeAILightTop = { 50, 205, 50, 255 };			// Lime green (bright but darker than before)
	const Color snakeAILightFront = { 34, 139, 34, 255 };			// Forest green (darker)
	const Color snakeAILightSide = { 46, 179, 113, 255 };			// Medium sea green (lighter than front)
	
	// Dark segments
	const Color snakeAIDarkTop = { 46, 179, 113, 255 };			// Medium sea green
	const Color snakeAIDarkFront = { 25, 100, 60, 255 };			// Dark forest green (much darker)
	const Color snakeAIDarkSide = { 34, 139, 34, 255 };			// Forest green (lighter than front)
	const Color snakeAIHidden = customBlack;
	
	// Food colors - Red shades (based on ground red)
	const Color foodTop = { 255, 120, 120, 255 };					// Light coral red (brightest)
	const Color foodFront = { 180, 52, 58, 255 };					// Ground dark red (darker)
	const Color foodSide = { 254, 74, 81, 255 };					// Ground red (lighter than front)
	const Color foodHidden = customBlack; 