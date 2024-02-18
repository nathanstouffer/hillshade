#include <iostream>

// TODO eventually define these via cmake
#ifndef PLATFORM_WIN32
#   define PLATFORM_WIN32 1
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#include <Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>
#include <stf/stf.hpp>

int main()
{
    std::cout << "hello world" << std::endl;
    std::cout << "vec3: " << stff::vec3(5) << std::endl;
    return 0;
}