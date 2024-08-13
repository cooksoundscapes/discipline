#include "main.h"
#include <bitset>
#include <array>

std::bitset<8> read_i2c_data(int file){
    auto raw = i2c_smbus_read_byte_data(file, 0xff);
    if (raw < 0) raw = 0;
    return std::bitset<8>(raw);
}

/**
 * Encoders hard logic
 */
using gray_code_t = std::array<std::bitset<4>, 4>;
using group_state = std::array<int, 4>;

struct encoder_mapping {
    gray_code_t cw;
    gray_code_t ccw;
} gray_code{
    {0b1110, 0b1000, 0b0001, 0b0111},
    {0b0100, 0b0010, 0b1011, 0b1101}
};

group_state decode_encoder_data(const std::bitset<8>& state) {
    auto intValue = state.to_ulong();
	int v1 = (intValue & 0x03); // Mask with 0b00000011 to get the lowest 2 bits.
	int v2 = ((intValue >> 2) & 0x03); // Shift and mask for 2.
	int v3 = ((intValue >> 4) & 0x03); // Shift and mask for 3.
	int v4 = ((intValue >> 6) & 0x03);
	return {v1, v2, v3, v4};
}

int get_rotation_direction(int prev, int curr) {
    std::bitset<4> stack((prev << 2) | curr);
	if (
        stack == gray_code.cw[0] ||
        stack == gray_code.cw[1] ||
        stack == gray_code.cw[2] ||
        stack == gray_code.cw[3]
    ) {
		return 1; // Clockwise
	} else if (
        stack == gray_code.ccw[0] ||
        stack == gray_code.ccw[1] ||
        stack == gray_code.ccw[2] ||
        stack == gray_code.ccw[3]
    ) {
		return -1; // Counterclockwise
	} else {
		return 0; // No rotation
	}
}

group_state read_encoder_data(int fd_id) {
    static group_state prev_read = {0, 0, 0, 0};
    group_state current_read = decode_encoder_data(read_i2c_data(fd_id));

    group_state state;
    for (int i{0}; i < 4; i++) {
        state[i] = get_rotation_direction(prev_read[i], current_read[i]);
    }
    prev_read = current_read;
    return state;
}

/**
 * Button Array logic
 */
std::array<int, 8> read_button_data(int fd_id) {
    static std::bitset<8> prev_state = std::bitset<8>(0);
    auto curr_state = read_i2c_data(fd_id);
    auto diff = prev_state ^ curr_state;
    std::array<int, 8> state{0};

    for (int i{0}; i < diff.size(); i++) {
        if (diff.test(i)) {
            state[i] = curr_state[i] == 1 ? 1 : -1;
        }
    }
    return state;
}
