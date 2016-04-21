/*
 * InitSimulationMessage.cpp
 *
 *  Created on: 18.04.2016
 *      Author: hartung
 */

#include "../../include/messages/InitSimulationMessage.hpp"
#include "../../include/network_impl/SimNetworkFunctions.hpp"

namespace NetOff
{
    InitSimulationMessage::InitSimulationMessage(const int& simId, const VariableList& inputs, const VariableList& outputs, const double* rVals,
                                                 const int* iVals, const char* bVals)
            : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::INIT_SIM),
              _dataSize(0),
              _req(nullptr),
              _id(nullptr),
              _inputPtr(nullptr),
              _outputPtr(nullptr),
              _reals(nullptr),
              _ints(nullptr),
              _bools(nullptr)
    {
        _dataSize =
                sizeof(InitialClientMessageSpecifyer) + sizeof(int) + 3 * sizeof(size_t) + inputs.dataSize() + outputs.dataSize() + ((rVals != nullptr) ?
                        inputs.getReals().size() :
                0 )+ ((iVals != nullptr) ? inputs.getInts().size() : 0) + ((bVals != nullptr) ? inputs.getBools().size() : 0);
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

        size_t numVals;
        numVals = (rVals == nullptr) ? 0 : inputs.getReals().size();
        p = saveShiftIntegralInData(numVals, p);
        _reals = reinterpret_cast<double *>(p);
        p = saveShiftPointerInData<double>(rVals, numVals, p);

        numVals = (iVals == nullptr) ? 0 : inputs.getInts().size();
        p = saveShiftIntegralInData(numVals, p);
        _ints = reinterpret_cast<int *>(p);
        p = saveShiftPointerInData<int>(iVals, numVals, p);

        numVals = (bVals == nullptr) ? 0 : inputs.getBools().size();
        p = saveShiftIntegralInData(numVals, p);
        _bools = reinterpret_cast<char *>(p);
        p = saveShiftPointerInData(bVals, numVals, p);
    }

    InitSimulationMessage::InitSimulationMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialClientMessageSpecifyer>(data),
              _data(data),
              _dataSize(0),
              _req(nullptr),
              _id(nullptr),
              _inputPtr(nullptr),
              _outputPtr(nullptr),
              _reals(nullptr),
              _ints(nullptr),
              _bools(nullptr)
    {
        char * p = _data.get();
        _req = reinterpret_cast<InitialClientMessageSpecifyer*>(p);
        p = shift<InitialClientMessageSpecifyer>(p);

        _id = reinterpret_cast<int *>(p);
        p = shift<int>(p);

        _inputPtr = p;
        //saveVariablesTo(p);
        p = shiftDataAccessable(VariableList::getVariableListFromData(p),p);
        _outputPtr = p;
        p = shiftDataAccessable(VariableList::getVariableListFromData(p),p);

        size_t numVals;
        numVals = getIntegralFromData<size_t>(p);
        p = shift<size_t>(p);
        if(numVals != 0)
        {
            _reals = reinterpret_cast<double *>(p);
            p = shift<double>(numVals,p);
        }
        numVals = getIntegralFromData<size_t>(p);
        p = shift<size_t>(p);
        if(numVals != 0)
        {
            _ints = reinterpret_cast<int *>(p);
            p = shift<int>(numVals,p);
        }
        numVals = getIntegralFromData<size_t>(p);
        p = shift<size_t>(p);
        if(numVals != 0)
        {
            _bools = reinterpret_cast<char *>(p);
            p = shift<char>(numVals,p);
        }

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

    const double * InitSimulationMessage::getReals() const
    {
        return _reals;
    }

    const int * InitSimulationMessage::getInts() const
    {
        return _ints;
    }

    const char * InitSimulationMessage::getBools() const
    {
        return _bools;
    }

}
