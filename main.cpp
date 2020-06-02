#include <iostream>
#include <random>

#include <CL/cl.hpp>
#include <SFML/Graphics.hpp>

#include "cl.hpp"

#define coordToIndex(x, y) x + (y * golWidth)

int golWidth = 1280;
int golHeight = 720;

void PrintHelp(std::string argvZero) {
    std::cout<<"Usage: "<<std::endl
             <<"\t"<<argvZero<<std::endl
             <<"\t"<<argvZero<<" width height"<<std::endl;
}

int main(int argc, char **argv) {
    if (argc == 3) { // ./a.out width height
        try {
            golWidth = std::stoi(argv[1]);
            golHeight = std::stoi(argv[2]);
        } catch (...) {
            PrintHelp(argv[0]);
            return 1;
        }
    } else if (argc == 1) {
        //default
    } else {
        PrintHelp(argv[0]);
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(golWidth, golHeight, sf::Style::None), "GoCL");
    window.setFramerateLimit(60);
    sf::Image image;
    image.create(golWidth, golHeight);
    sf::Texture texture;
    sf::Sprite sprite;

    CLManager clmg;
    clmg.CLSetup();
    assert(clmg.GetIsCLSetUp());

    cl::Context context = clmg.GetCLContext();
    cl::Device device = clmg.GetCLDevice();

    std::unique_ptr<int[]> golMatrix = std::make_unique<int[]>(golHeight*golWidth);
    std::fill(&golMatrix[0], &golMatrix[golWidth*golHeight], 0x0);
    
    /*golMatrix[coordToIndex(10,10)] = 1;
    golMatrix[coordToIndex(11,11)] = 1;
    golMatrix[coordToIndex( 9,12)] = 1;
    golMatrix[coordToIndex(10,12)] = 1;
    golMatrix[coordToIndex(11,12)] = 1;*/

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);
    for (int i = 0; i < golWidth * golHeight; i++) {
        golMatrix[i] = dist(gen);
    }

    cl::Buffer bufferA(context, CL_MEM_READ_WRITE, golHeight*golWidth*sizeof(int));
    cl::Buffer bufferB(context, CL_MEM_READ_WRITE, golHeight*golWidth*sizeof(int));

    cl::Kernel golKernel = clmg.LoadCLKernelFromPath("kernel.cl");
    golKernel.setArg(0, bufferA);
    golKernel.setArg(1, bufferB);
    golKernel.setArg(2, (int)golWidth);
    golKernel.setArg(3, (int)golHeight);

    cl::CommandQueue queue(context, device);

    while (window.isOpen()) {
        queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(int)*golWidth*golHeight, &(golMatrix[0]));
        queue.enqueueNDRangeKernel(golKernel, cl::NullRange, cl::NDRange(std::min(window.getSize().y, clmg.GetMaxGroups())));
        queue.enqueueReadBuffer(bufferB, CL_TRUE, 0, sizeof(int)*golWidth*golHeight, &(golMatrix[0]));
        queue.finish();

        for (int y = 0; y < golHeight; y++) {
            for (int x = 0; x < golWidth; x++) {
                int actualColor = ((golMatrix[coordToIndex(x, y)]) ? 255 : 0);
                if (actualColor)
                    image.setPixel(x, y, sf::Color(actualColor, 0, 0));
                else
                    image.setPixel(x, y, sf::Color(0, 0, 0));
            }
        }

        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }
    }

    return 0;
}