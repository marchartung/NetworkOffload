/*
 * TypeEnum.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_ADDITIONALTYPES_HPP_
#define INCLUDE_ADDITIONALTYPES_HPP_

#include <vector>
#include <tuple>
#include <memory>

namespace NetOff
{
    enum InitialClientMessageSpecifyer
    {
        START = 0,
        ADD_SIM = 1,
        INIT_SIM = 2,
        CLIENT_INIT_ABORT = 3,
        GET_FILE = 4
    };

    enum ClientMessageSpecifyer
    {
        PAUSE = 8,
        UNPAUSE = 9,
        RESET = 10,
        INPUTS = 11,
        CLIENT_ABORT = 12
    };

    enum InitialServerMessageSpecifyer
    {
        SUCCESS_ADD_SIM = 13,
        SUCCESS_START = 14,
        SUCCESS_GET_FILE = 15,
        SERVER_INIT_ABORT = 16
    };

    enum ServerMessageSpecifyer
    {
        SUCCESS_SIM_INIT = 17,
        SUCCESS_PAUSE = 18,
        SUCCESS_UNPAUSE = 19,
        SUCCESS_RESET = 20,
        OUTPUTS = 21,
        SERVER_ABORT = 22
    };

    enum CurrentState
    {
        NONE = 0,
        INITED = 1,
        STARTED = 2,
        PAUSED = 3
    };

    class SharedDataAccessable
    {
     public:
        virtual ~SharedDataAccessable() { }
        virtual std::shared_ptr<char> data() = 0;
        virtual std::shared_ptr<const char> data() const = 0;
        virtual size_t dataSize() const = 0;
    };

    class RawDataAccessable
    {
     public:
        virtual ~RawDataAccessable() { }
        virtual char * data() = 0;
        virtual const char * data() const = 0;
        virtual size_t dataSize() const = 0;
    };

}

#endif /* INCLUDE_ADDITIONALTYPES_HPP_ */
