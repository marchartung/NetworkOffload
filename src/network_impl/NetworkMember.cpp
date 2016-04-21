/*
 * NetworkClient.cpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
 */

#include "../../include/network_impl/NetworkMember.hpp"

#include <iostream>

namespace NetOff
{

    NetworkMember::NetworkMember()
            : _socket(nullptr),
              _maxBuffer(1024),
              _sleepTime(1000),
              _numMaxSleeps(100)
    {

    }

    NetworkMember::~NetworkMember()
    {
        killSocket(_socket);
    }

    bool NetworkMember::send(const char* buffer, const int & num)
    {
        int numBits = -1;
        int i = 0;
        //std::cout << "sending " << num << " Bytes\n";
        //std::cout << "[";
        //for (size_t j = 0; j < num; ++j)
          //  std::cout << "'" << (int)buffer[j] << "' ";
        //std::cout << "]\n";
        while (i < num)
        {
            int partNum = std::min(num, i + _maxBuffer) - i;
            numBits = SDLNet_TCP_Send(_socket, static_cast<const void *>(&buffer[i]), partNum);

          //  std::cout << "sent " << numBits << "\n";
            if (numBits > 0)
                i += numBits;
            else if (i == numBits && i < num)
            {
                std::cout << "Connection send fail.\n";
                return false;
            }
        }
        //std::cout << "complete: " << i << "/" << num << " Bytes.\n\n";
        return true;
    }

    bool NetworkMember::recv(char* buffer, const int & num)
    {
        int numBits = -1;
        int i = 0;
        //std::cout << "recv " << num << " Bytes\n";
        while (i < num)
        {
            unsigned partNum = std::min(num, i + _maxBuffer) - i;
            numBits = SDLNet_TCP_Recv(_socket, &buffer[i], partNum);
            //std::cout << "received " << numBits << ":\n";
            if (numBits > 0)
            {
              //  std::cout << "[";
                //for (size_t j = i; j < i + numBits; ++j)
                  //  std::cout << "'" << (int)buffer[j] << "' ";
                //std::cout << "]\n";
            }
            if (numBits > 0)
                i += numBits;
            else if (i == 0 && i < num)
            {
                std::cout << "Connection recv fail.\n";
                return false;
            }

        }
        //std::cout << "complete: " << i << "/" << num << " Bytes.\n\n";
        return true;
    }

    bool NetworkMember::variableSend(const char* buffer, const int & num)
    {
        int numCopy = num;
        std::cout << "variable send: " << num << " Bytes\n";
        if (!send(reinterpret_cast<char *>(&numCopy), sizeof(numCopy)))
            return false;
        return send(buffer, num);
    }

    std::shared_ptr<char> NetworkMember::variableRecv()
    {
        int num = 0;
        recv(reinterpret_cast<char*>(&num), sizeof(num));
        std::shared_ptr<char> res(new char[num]);
        recv(res.get(), num);
        return res;
    }

    void NetworkMember::killSocket(TCPsocket& socket)
    {
        {
            if (socket != nullptr)
            {
                SDLNet_TCP_Close(socket);
                socket = nullptr;
            }
        }
    }

} /* namespace NetOff */

