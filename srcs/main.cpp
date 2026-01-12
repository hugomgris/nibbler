#include "../incs/IGraphic.hpp"
#include "GameState.hpp"
#include <dlfcn.h> // for dynamic linking
#include <iostream>

int main() {
	void *handle = dlopen("./testlib.so", RTLD_NOW);
	if (!handle)
	{
		std::cerr << dlerror() << std::endl;
		return 1;
	}

	using CreateFn = IGraphic *(*)();
	using DestroyFn = void (*)(IGraphic*);

	dlerror();

	CreateFn create = (CreateFn)dlsym(handle, "createGraphic");
	const char* err = dlerror();
	if (err) {
		std::cerr << err << std::endl;
		return 1;
	}

	DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");
	err = dlerror();
	if (err) {
		std::cerr << err << std::endl;
		return 1;
	}

	IGraphic *gfx = create();
	gfx->init(20, 20);

	Vec2 segments[4] = {{10,10}, {9,10}, {8,10}, {7,10}};
	SnakeView snake { segments, 4 };
	FoodView food{ {5,5} };

	GameState state {
		20, 20, snake, food, false
	};

	gfx->render(state);

	destroy(gfx);
	dlclose(handle);

	return 0;
}