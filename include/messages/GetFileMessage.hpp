/*
 * GetFileMessage.hpp
 *
 *  Created on: 27.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_GETFILEMESSAGE_HPP_
#define INCLUDE_MESSAGES_GETFILEMESSAGE_HPP_

#include <fstream>
#include <algorithm>

#include "AbstractMessage.hpp"
#include "../AdditionalTypes.hpp"
#include "../network_impl/SimNetworkFunctions.hpp"

namespace NetOff
{
    class GetFileMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {
     public:
        GetFileMessage(const int & simId, const std::string & file)
                : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::GET_FILE),
                  _data(nullptr),
                  _dataSize(0),
                  _req(nullptr),
                  _simId(nullptr),
                  _path(nullptr)
        {
            std::cout << "size: " << _dataSize << "\n";
            _dataSize = sizeof(InitialClientMessageSpecifyer) + sizeof(int) + getStringDataSize(file);
            _data = std::shared_ptr<char>(new char[_dataSize]);
            _req = reinterpret_cast<InitialClientMessageSpecifyer*>(_data.get());
            char * p = saveShiftIntegralInData(InitialClientMessageSpecifyer::GET_FILE, _data.get());
            _simId = reinterpret_cast<int*>(p);
            p = saveShiftIntegralInData<int>(simId, p);
            _path = p;
            saveStringInData(file,_path);
        }

        GetFileMessage(std::shared_ptr<char> & data)
                : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::GET_FILE),
                  _data(data),
                  _dataSize(0),
                  _req(nullptr),
                  _simId(nullptr),
                  _path(nullptr)
        {
            _req = reinterpret_cast<InitialClientMessageSpecifyer*>(_data.get());
            char * p = shift<InitialClientMessageSpecifyer>(_data.get());
            _simId = reinterpret_cast<int*>(p);
            p = shift<int>(p);
            _path = p;
        }

        ~GetFileMessage()
        {

        }

        int getSimId() const
        {
            return *_simId;
        }

        std::string getFilePath() const
        {
            return createStringFromData(_path);
        }

        char * data() override
        {
            return _data.get();
        }

        const char * data() const override
        {
            return _data.get();
        }

        size_t dataSize() const override
        {
            return _dataSize;
        }

     private:
        std::shared_ptr<char> _data;
        size_t _dataSize;
        InitialClientMessageSpecifyer * _req;
        int * _simId;
        char * _path;
    };

    class GetFileSuccessMessage : public AbstractMessage<InitialServerMessageSpecifyer>
    {
     public:
        GetFileSuccessMessage(const int & simId, const std::string & file)
            : AbstractMessage<InitialServerMessageSpecifyer>(InitialServerMessageSpecifyer::SUCCESS_GET_FILE),
              _data(nullptr),
              _dataSize(0),
              _req(nullptr),
              _simId(nullptr),
              _fileStart(nullptr)
        {
            std::ifstream fileStream;
            std::streampos startPos, endPos;
            fileStream.open(file.c_str(), std::ios::binary);
            startPos = fileStream.tellg();
            fileStream.seekg(0,std::ios::end);
            endPos = fileStream.tellg();

            _dataSize = sizeof(InitialServerMessageSpecifyer) + sizeof(int) + endPos - startPos;
            _data = std::shared_ptr<char>(new char[_dataSize]);
            _req = reinterpret_cast<InitialServerMessageSpecifyer*>(_data.get());
            char * p = saveShiftIntegralInData(InitialServerMessageSpecifyer::SUCCESS_GET_FILE,_data.get());
            _simId = reinterpret_cast<int*>(p);
            p = saveShiftIntegralInData(simId,p);
            _fileStart = p;
            fileStream.seekg(0,std::ios::beg);
            size_t count = sizeof(InitialServerMessageSpecifyer) + sizeof(int);
            size_t blockLength = 512, curReadLength = 1;
            while(fileStream.good() && curReadLength > 0)
            {
                curReadLength = std::min(blockLength, _dataSize - count);
                fileStream.read(p, curReadLength);
                p += curReadLength;
                count += curReadLength;
            }
            fileStream.close();
            if(count != _dataSize)// || endPos != fileStream.tellg())
                throw std::runtime_error("GetFileSuccessMessage: Error during file read.");
        }

        GetFileSuccessMessage(const size_t numBytes, std::shared_ptr<char> & data, const std::string & file)
        : AbstractMessage<InitialServerMessageSpecifyer>(InitialServerMessageSpecifyer::SUCCESS_GET_FILE),
          _data(data),
          _dataSize(numBytes),
          _req(nullptr),
          _simId(nullptr),
          _fileStart(nullptr)
        {
            _req = reinterpret_cast<InitialServerMessageSpecifyer*>(_data.get());
            char * p = _data.get();
            p = shift<InitialServerMessageSpecifyer>(p);
            _simId = reinterpret_cast<int*>(p);
            p = shift<int>(p);

            size_t curReadLength = 1, count = sizeof(InitialServerMessageSpecifyer) + sizeof(int), blockLength = 512;
            std::ofstream fileStream;
            fileStream.open(file.c_str(), std::ios::binary);
            while(fileStream.good() && curReadLength > 0)
            {
                curReadLength = std::min(blockLength, _dataSize - count);
                fileStream.write(p, curReadLength);
                p += curReadLength;
                count += curReadLength;
            }
            fileStream.close();
        }

        char * data() override
        {
            return _data.get();
        }

        std::shared_ptr<char> & shared()
        {
            return _data;
        }

        const char * data() const override
        {
            return _data.get();
        }

        size_t dataSize() const override
        {
            return _dataSize;
        }

     private:
        std::shared_ptr<char> _data;
        size_t _dataSize;

        InitialServerMessageSpecifyer * _req;
        int * _simId;
        char * _fileStart;
    };

}

#endif /* INCLUDE_MESSAGES_GETFILEMESSAGE_HPP_ */
