#include "app.h"

extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{

    SimpleShapeApplication app(650, 480, PROJECT_NAME, true);
    app.run(1);

    return 0;
}