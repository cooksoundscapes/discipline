#include "audio-sink.h"
#include <jack/jack.h>

class audio_sink_impl : public audio_sink
{
public:
    audio_sink_impl(int n_channel) : n_channel(n_channel) {}
    ~audio_sink_impl();

    void start() override;
    void stop() override;

    // expects range 1~n_channel
    const std::vector<float>& get_buffer(int ch) override {
        return buffers[ch-1];
    }

    unsigned int buffer_size() override {
        return buff_size;
    }

private:
    jack_nframes_t buff_size;
    jack_client_t* client;
    std::vector<jack_port_t*> ports;
    std::vector<std::vector<float>> buffers;

    int n_channel;

    static int audio_processing(jack_nframes_t, void*);
};