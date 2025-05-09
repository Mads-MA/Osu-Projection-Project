#ifndef FRAME_DATA_HPP
#define FRAME_DATA_HPP

#include <vector>

struct FrameData {
    std::vector<uint8_t> data;
    int width;
    int height;
    int channels;

    FrameData() {
        data = {};
        width = -1;
        height = -1;
        channels = -1;
    }
};


#endif