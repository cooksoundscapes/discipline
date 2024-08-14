#pragma once
#include <vector>

class audio_sink {
public: 
    virtual ~audio_sink() = default;

    virtual void start() {}
    virtual void stop() {}
    virtual const std::vector<float>& get_buffer(int ch) = 0;
    virtual unsigned int buffer_size() = 0;
};