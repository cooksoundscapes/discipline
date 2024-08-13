#include "jack-sink-impl.h"
#include <iostream>
#include <string>

audio_sink_impl::~audio_sink_impl() {
    stop();
}

void audio_sink_impl::start() {
    buffers.resize(n_channel);
    client = jack_client_open("discipline", JackNullOption, NULL);

    if (client == NULL) {
        std::cerr << "Failed to start JACK Client, program will not receive audio;\n";
        for (auto& buff : buffers) {
            buff.resize(1);
        }
        return;
    }

    buff_size = jack_get_buffer_size(client);
    for (auto& buff: buffers) {
        buff.resize(buff_size);
    }

    for (int i{0}; i < n_channel; i++) {
        std::string p_name = "input_" + std::to_string(i+1);
        jack_port_t* port = jack_port_register(
            client,
            p_name.c_str(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput,
            0
        );
        if (port == NULL) {
            std::cerr << "Failed to register port [" << i+1 << "];\n";
        } else {
            ports.push_back(port);
        }
    }

    if (jack_set_process_callback(client, audio_processing, this)) {
        std::cerr << "Error while registering JACK audio callback;\n";
        return;
    }
    if (jack_activate(client)) {
        std::cerr << "Error while activating JACK client;\n";
        return;
    }
    std::cout << "JACK Client started successfully;\n";
}

void audio_sink_impl::stop() {
   if (!client) return;
    if(jack_deactivate(client)) {
        std::cerr << "Error while closing JACK client;\n";
    }
    ports.clear();
    for (auto buff : buffers) {
        buff.clear();
    }
    buffers.clear();
}

int audio_sink_impl::audio_processing(jack_nframes_t n_frame, void* user_data) {
    auto sink = static_cast<audio_sink_impl*>(user_data);

    for (int i{0}; i < sink->ports.size(); i++) {
        auto jack_buffer = static_cast<float*>(jack_port_get_buffer(sink->ports[i], n_frame));

        if (!jack_buffer) {
            std::cerr << "Failed to get JACK buffer from port " << jack_port_name(sink->ports[i]) << std::endl;
            sink->stop();
            return 1;
        }
        for(size_t s{0}; s < sink->buff_size; s++) {
            sink->buffers[i][s] = jack_buffer[s];
        }
    }
    return 0;
}