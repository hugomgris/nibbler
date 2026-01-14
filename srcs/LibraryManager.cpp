#include "../incs/LibraryManager.hpp"

LibraryManager::LibraryManager() : handle(nullptr), graphic(nullptr) {}

LibraryManager::~LibraryManager() { unload(); }

bool LibraryManager::load(const char * libPath) {
	handle = dlopen(libPath, RTLD_NOW);
			if (!handle) {
				std::cerr << "dlopen error: " << dlerror() << std::endl;
				return false;
			}
			
			CreateFn create = (CreateFn)dlsym(handle, "createGraphic");
			DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");
			
			if (!create || !destroy) {
				std::cerr << "Symbol error: " << dlerror() << std::endl;
				dlclose(handle);
				handle = nullptr;
				return false;
			}
			
			graphic = create();
			std::cout << "Loaded: " << libPath << std::endl;
			return true;
}

void LibraryManager::unload() {
	if (graphic) {
		using DestroyFn = void (*)(IGraphic*);
		DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");
		if (destroy) {
			destroy(graphic);
		}
		graphic = nullptr;
	}

	if (handle) {
		dlclose(handle);
		handle = nullptr;
	}
}

IGraphic *LibraryManager::get() { return graphic; }