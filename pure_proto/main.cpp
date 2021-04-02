#include <iostream>
#include <fstream>
#include "desc.h"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "need specify pb file and out path to convert" << std::endl;
        std::cout << "example: pure_proto game.pb ./out_dir" << std::endl;
        return 0;
    }

    std::cout << "pb file " << argv[1] << std::endl;
    std::cout << "out path " << argv[2] << std::endl;

    Tool tool;
    tool.Do(argv[1], argv[2]);

    return 0;
}