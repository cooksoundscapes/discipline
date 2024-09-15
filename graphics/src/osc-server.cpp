#include "main.h"
#include "osc-server.h"

int navigate_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    auto next_scrn = &argv[0]->s;
    if (pages.find(next_scrn) != pages.end()) {
        current_page = next_scrn;
    } else {
        std::cerr << "Screen " << next_scrn << " is not registered;\n";
    }
    return 0;
}

int param_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string key = &argv[0]->s;
    float value = argv[1]->f;
    pages[current_page]->set(key, value);
    return 0;
}

int text_param_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string key = &argv[0]->s;
    std::string value = &argv[1]->s;
    pages[current_page]->set(key, value);
    return 0;
}

int text_list_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    if (t[0] != 's') return 0;
    std::string key = &argv[0]->s;
    std::vector<std::string> values{};
    for (int i = 1; i < argc-1; i++) {
        if (t[i] == 's') {
            values.push_back(&argv[i]->s);
        }
    }
    pages[current_page]->set(key, values);
    return 0;
}

int buffer_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    if (t[0] != 's') return 0;
    std::string key = &argv[0]->s;
    std::vector<float> values{};

    for (int i = 1; i < argc-1; i++) {
        if (t[i] == 'f') {
            values.push_back(argv[i]->f);
        }
    }
    pages[current_page]->set(key, values);
    return 0;
}

osc_server::osc_server() {
    server = lo_server_thread_new(
        OSC_PORT,
        [](int num, const char *msg, const char *path){
            std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
        }
    );
    if (!server) {
        std::cerr << "Failed to create OSC server.\n";
        return;
    }
    lo_server_thread_add_method(server, "/navigate", "s", navigate_handler, nullptr);
    lo_server_thread_add_method(server, "/param", "sf", param_handler, nullptr);
    lo_server_thread_add_method(server, "/param", "ss", text_param_handler, nullptr);
    lo_server_thread_add_method(server, "/buffer", nullptr, buffer_handler, nullptr);
    lo_server_thread_add_method(server, "/text-list", nullptr, text_list_handler, nullptr);
    lo_server_thread_start(server);
    std::cout << "Listening to OSC messages at port " << OSC_PORT << ";\n";
}

osc_server::~osc_server() {
    lo_server_thread_free(server);
}