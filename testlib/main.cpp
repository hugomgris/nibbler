#include "plugin.h"
#include <dlfcn.h>
#include <iostream>
#include <string>

class HostCallbacks : public IHostCallbacks {
public:
    void log(const char* msg) override {
        std::cout << "[host] " << msg << "\n";
    }

    int add(int a, int b) override {
        return a + b;
    }
};

int main() {
    void* handle = dlopen("./libplugin.so", RTLD_NOW);
    if (!handle) {
        std::cerr << "dlopen error: " << dlerror() << "\n";
        return 1;
    }

    using create_fn_t  = IPlugin* (*)(IHostCallbacks*);
    using destroy_fn_t = void (*)(IPlugin*);

    dlerror();

    auto create_plugin  = (create_fn_t)dlsym(handle, "create_plugin");
    const char* e1 = dlerror();
    if (e1) {
        std::cerr << "dlsym(create_plugin) error: " << e1 << "\n";
        dlclose(handle);
        return 1;
    }

    auto destroy_plugin = (destroy_fn_t)dlsym(handle, "destroy_plugin");
    const char* e2 = dlerror();
    if (e2) {
        std::cerr << "dlsym(destroy_plugin) error: " << e2 << "\n";
        dlclose(handle);
        return 1;
    }

    HostCallbacks host;
    IPlugin* plugin = create_plugin(&host);

    plugin->start();
    plugin->do_work(10);

    destroy_plugin(plugin);
    dlclose(handle);
    return 0;
}

