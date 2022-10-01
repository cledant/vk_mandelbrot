#include "Engine.hpp"

#include "fmt/core.h"

int
main()
{
    try {
        Engine engine;

        engine.init();
        engine.run();
    } catch (std::exception const &e) {
        fmt::print("Exiting app: {}", e.what());
        return (1);
    }
    return (0);
}