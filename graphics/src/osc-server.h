#pragma once
#include <lo/lo.h>

class osc_server {
public:
    osc_server();
    ~osc_server();
private:
    lo_server_thread server;
};