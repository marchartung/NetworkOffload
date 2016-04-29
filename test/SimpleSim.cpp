/*
 * SimpleSim.cpp
 *
 *  Created on: 19.04.2016
 *      Author: Marc Hartung
 */

#include <string>
#include <iostream>

#include "SimpleSimClient.hpp"
#include "SimpleSimServer.hpp"

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./SimpleSim [port] ([servername])\n";
        std::cout << "If no servername is set, the program will start a server, otherwise a client.\n";
        return 0;
    }
    int port = std::stoi(argv[1]);
    if (argc > 2)
        return clientFunc(argv[2], port);
    else
        return serverFunc(port);
}
