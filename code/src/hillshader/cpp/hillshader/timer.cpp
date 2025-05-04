#include "hillshader/timer.hpp"

#include <chrono>

namespace hillshader::timer
{

    time_t now_ms()
    {
        using chron = std::chrono;
        return chron::duration_cast<chron::milliseconds>(chron::system_clock::now().time_since_epoch()).count();
    }

}