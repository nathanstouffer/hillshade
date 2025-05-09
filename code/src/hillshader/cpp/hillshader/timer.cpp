#include "hillshader/timer.hpp"

#include <chrono>

namespace hillshader::timer
{

    time_t now_ms()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

}