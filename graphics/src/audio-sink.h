#pragma once
#include <vector>

class audio_sink {
public: 
    virtual ~audio_sink() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual const std::vector<float>& get_buffer(int ch) = 0;
    virtual unsigned int buffer_size() = 0;
};