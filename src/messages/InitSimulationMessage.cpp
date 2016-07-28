/*
 * InitSimulationMessage.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "../../include/messages/InitSimulationMessage.hpp"
#include "../../include/network_impl/SimNetworkFunctions.hpp"

namespace NetOff
{
    InitSimulationMessage::InitSimulationMessage(const int& simId, const VariableList& inputs, const VariableList& outputs)
            : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::INIT_SIM),
              _data(nullptr),
              _dataSize(0),
              _req(nullptr),
              _id(nullptr),
              _inputPtr(nullptr),
              _outputPtr(nullptr)
    {
        _dataSize = sizeof(InitialClientMessageSpecifyer) + sizeof(int) + 3 * sizeof(size_t) + inputs.dataSize() + outputs.dataSize();
        _data = std::shared_ptr<char>(new char[_dataSize]);
        char * p = _data.get();

        // Data structure: [ Specifyer | id | inputs | outputs | numReals | reals | numInts | ints | numBools | bools ]
        _req = reinterpret_cast<InitialClientMessageSpecifyer *>(p);
        p = saveShiftIntegralInData(InitialClientMessageSpecifyer::INIT_SIM, p);

        _id = reinterpret_cast<int *>(p);
        p = saveShiftIntegralInData(simId, p);
        _inputPtr = p;

        p = saveShiftDirectDataAccessableInData(inputs, p);
        _outputPtr = p;
        p = saveShiftDirectDataAccessableInData(outputs, p);
    }

    InitSimulationMessage::InitSimulationMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialClientMessageSpecifyer>(data),
              _data(data),
              _dataSize(0),
              _req(nullptr),
              _id(nullptr),
              _inputPtr(nullptr),
              _outputPtr(nullptr)
    {
        char * p = _data.get();
        _req = reinterpret_cast<InitialClientMessageSpecifyer*>(p);
        p = shift<InitialClientMessageSpecifyer>(p);

        _id = reinterpret_cast<int *>(p);
        p = shift<int>(p);

        _inputPtr = p;
        //saveVariablesTo(p);
        p = shiftDataAccessable(VariableList::getVariableListFromData(p), p);
        _outputPtr = p;
        p = shiftDataAccessable(VariableList::getVariableListFromData(p), p);

    }

    char* InitSimulationMessage::data()
    {
        return _data.get();
    }

    const char* InitSimulationMessage::data() const
    {
        return _data.get();
    }

    size_t InitSimulationMessage::dataSize() const
    {
        return _dataSize;
    }

    VariableList InitSimulationMessage::getInputs() const
    {
        return VariableList::getVariableListFromData(_inputPtr);
    }

    VariableList InitSimulationMessage::getOutputs() const
    {
        return VariableList::getVariableListFromData(_outputPtr);
    }

    const int& InitSimulationMessage::getSimId() const
    {
        return *_id;
    }

}
