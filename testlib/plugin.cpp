#include "plugin.h"
#include <string>

class PluginImpl : public IPlugin {
public:
    explicit PluginImpl(IHostCallbacks* host) : host_(host) {}

    void start() override {
        host_->log("[plugin] start() llamado");
        int r = host_->add(2, 40);
        host_->log((std::string("[plugin] host->add(2,40) = ") + std::to_string(r)).c_str());
    }

    void do_work(int x) override {
        host_->log("[plugin] do_work() llamado");
        int r = host_->add(x, 1);
        host_->log((std::string("[plugin] add(x,1) = ") + std::to_string(r)).c_str());
    }

private:
    IHostCallbacks* host_;
};

extern "C" IPlugin* create_plugin(IHostCallbacks* host) {
    return new PluginImpl(host);
}

extern "C" void destroy_plugin(IPlugin* p) {
    delete p;
}
