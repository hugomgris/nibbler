#pragma once
#include "IGraphic.hpp"
#include <dlfcn.h>
#include <iostream>

class LibraryManager {
	private:
		void *handle;
		IGraphic *graphic;
		using CreateFn = IGraphic *(*)();
		using DestroyFn = void (*)(IGraphic*);
	
	public:
		LibraryManager();

		LibraryManager(const LibraryManager&) = delete;
		LibraryManager &operator=(const LibraryManager&) = delete;

		~LibraryManager();

		bool load(const char *libPath);
		void unload();

		IGraphic *get();
};