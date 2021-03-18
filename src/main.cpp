#include <glm/glm.hpp>
#include <iostream>

#include "SHLighting.h"

int main(int argc, char** argv) {
    try {
        SHLighting& sh = SHLighting::GetInstance();
        sh.Init();
        sh.Run();
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    int x;
    std::cin >> x;
}
