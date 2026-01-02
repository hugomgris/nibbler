#pragma once
#include <cstddef>

class IHostCallbacks {
public:
    virtual ~IHostCallbacks() = default;
    virtual void log(const char* msg) = 0;
    virtual int  add(int a, int b) = 0;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual void start() = 0;
    virtual void do_work(int x) = 0;
};

extern "C" {
    IPlugin* create_plugin(IHostCallbacks* host);
    void     destroy_plugin(IPlugin* p);
}
