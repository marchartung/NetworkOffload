/*
 * InitSimulationMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_INITSIMULATIONMESSAGE_HPP_
#define INCLUDE_MESSAGES_INITSIMULATIONMESSAGE_HPP_

#include "AbstractMessage.hpp"
#include "../VariableList.hpp"

namespace NetOff
{

    class InitSimulationMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {
     public:
        InitSimulationMessage(const int & simId, const VariableList & inputs, const VariableList & outputs);

        InitSimulationMessage(std::shared_ptr<char> & data);

        InitSimulationMessage(const InitSimulationMessage & rhs) = delete;

        InitSimulationMessage& operator=(const InitSimulationMessage & rhs) = delete;

        ~InitSimulationMessage() = default;

        char * data() override;

        const char * data() const override;

        size_t dataSize() const override;

        VariableList getInputs() const;

        VariableList getOutputs() const;

        const int & getSimId() const;

        const double * getReals() const;

        const int * getInts() const;

        const char * getBools() const;

     private:
        std::shared_ptr<char> _data;
        size_t _dataSize;

        InitialClientMessageSpecifyer * _req;
        int * _id;
        char * _inputPtr;
        char * _outputPtr;
    };
}

#endif /* INCLUDE_MESSAGES_INITSIMULATIONMESSAGE_HPP_ */
