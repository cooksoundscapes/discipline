#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <pigpiod_if2.h>
#include <lo/lo.h>
#include "main.h"

#define I2C_DEV "/dev/i2c-1"
#define I2C_NAV_BUTTONS_ADDR 0x20
#define I2C_LED_ARRAY_ADDR 0x21
#define I2C_SEQ_BUTTONS_ADDR 0x23
#define I2C_ENCODERS_ADDR 0x27

#define GPIO_NAV_BUTTONS_INT_PIN 17
#define GPIO_SEQ_BUTTONS_INT_PIN 27
#define GPIO_ENCODERS_INT_PIN 22

/**
 * I2C smbus setup
 */
int use_i2c_address(unsigned int addr) {
    int i2c_fd = open(I2C_DEV, O_RDWR);
    if (i2c_fd > 0) {
        std::cerr << "Error while opening " << I2C_DEV << ";\n";
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, addr)) {
        std::cout << "Error opening I2C device" << addr << '\n';
    }
    return i2c_fd;
}

/**
 * GPIO Daemon setup
 */
int gpio_d;
std::vector<int> gpio_cb_ids{};
extern std::array<int, 4> read_encoder_data(int);
extern std::array<int, 8> read_button_data(int);

void gpio_callback(
    int daemon,
    unsigned int pin,
    unsigned int level,
    unsigned int tick,
    void* user_data
) {
    auto i2c_fd_id = static_cast<int*>(user_data);
    switch(pin) {
        case GPIO_NAV_BUTTONS_INT_PIN:
        case GPIO_SEQ_BUTTONS_INT_PIN:
            auto state = read_button_ata(i2c_fd_id);
            break;
        case GPIO_ENCODERS_INT_PIN:
            auto state = read_encoder_data(i2c_fd_id);
            break;
        default:
            break;
    }
    // liblo logic? also, map pin to device is here
}

void add_gpio_interrupt(int pin, int&i2c_fd_id) {
    set_mode(gpio_d, pin, PI_INPUT);
    set_pull_up_down(gpio_d, pin, PI_PUD_UP);
    gpio_cb_ids.push_back(callback_ex(daemon, pin, EITHER_EDGE, gpio_callback, i2c_fd_id));
}

/**
 * Main function
 */
int main() {
    int nav_buttons = use_i2c_address(I2C_NAV_BUTTONS_ADDR);
    int led_array = use_i2c_address(I2C_LED_ARRAY_ADDR);
    int seq_buttons = use_i2c_address(I2C_SEQ_BUTTONS_ADDR);
    int encoders = use_i2c_address(I2C_ENCODERS_ADDR);
    gpio_d = pigpio_start(0, 0);
    if (gpio_d < 0) {
        std::cerr << "Error while initializing GPIO Daemon\n";
        return 1;
    }
    add_gpio_interrupt(GPIO_NAV_BUTTONS_INT_PIN, nav_buttons);
    add_gpio_interrupt(GPIO_SEQ_BUTTONS_INT_PIN, seq_buttons);
    add_gpio_interrupt(GPIO_ENCODERS_INT_PIN, encoders);

    // live until called out
    while(true) {}

    /**
     * Cleanup
     */
    close(nav_buttons);
    close(led_array);
    close(seq_buttons);
    close(encoders);
    for (int cb_id : gpio_cb_ids) {
        callback_cancel(cb_id);
    }
    pigpio_stop(gpio_d);
}