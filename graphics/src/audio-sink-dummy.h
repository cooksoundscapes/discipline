#include "audio-sink.h"

class audio_sink_impl : public audio_sink
{
public:
    audio_sink_impl(int n_channel) {}
    ~audio_sink_impl() {}

    // expects range 1~n_channel
    const std::vector<float>& get_buffer(int ch) override {
        static auto dummy = std::vector<float>(0);
        return dummy;
    }

    unsigned int buffer_size() override {
        return 0;
    }
};