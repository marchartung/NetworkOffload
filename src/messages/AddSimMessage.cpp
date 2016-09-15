/*
 * AddSimMessage.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "../../include/messages/AddSimMessage.hpp"
#include "../../include/messages/AbstractMessage.hpp"
#include "../../include/AdditionalTypes.hpp"
#include "../../include/network_impl/SimNetworkFunctions.hpp"

#include <iostream>

namespace NetOff
{
    //////////////////////////////////////////////
    /////////////AddSimRequestMessage/////////////
    //////////////////////////////////////////////

    AddSimRequestMessage::AddSimRequestMessage(const int & id, const std::string & path)
            : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::ADD_SIM),
              _data(nullptr),
              _dataSize(0)
    {
        _dataSize = sizeof(InitialClientMessageSpecifyer) + sizeof(int) + getStringDataSize(path);
        _data = std::shared_ptr<char>(new char[_dataSize]);
        char * p = _data.get();
        p = saveShiftIntegralInData(InitialClientMessageSpecifyer::ADD_SIM, p);
        p = saveShiftIntegralInData(id, p);
        saveStringInData(path, p);
    }

    AddSimRequestMessage::AddSimRequestMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialClientMessageSpecifyer>(*reinterpret_cast<InitialClientMessageSpecifyer *>(data.get())),
              _data(data),
              _dataSize(0)
    {
    }

    char * AddSimRequestMessage::data()
    {
        return _data.get();
    }

    const char * AddSimRequestMessage::data() const
    {
        return _data.get();
    }

    size_t AddSimRequestMessage::dataSize() const
    {
        return _dataSize;
    }

    int AddSimRequestMessage::getSimId()
    {
        return *reinterpret_cast<int*>(shift<InitialClientMessageSpecifyer>(_data.get()));
    }

    std::string AddSimRequestMessage::getPath()
    {
        return createStringFromData(shift<int>(shift<InitialClientMessageSpecifyer>(_data.get())));
    }

    //////////////////////////////////////////////
    /////////AddSimRequestSuccessMessage//////////
    //////////////////////////////////////////////

    AddSimSuccessMessage::AddSimSuccessMessage(const int & id, const VariableList & inputs, const VariableList & outputs)
            : AbstractMessage<InitialServerMessageSpecifyer>(InitialServerMessageSpecifyer::SUCCESS_ADD_SIM),
              _data(nullptr),
              _dataSize(0)
    {
        _dataSize = sizeof(InitialServerMessageSpecifyer) + sizeof(int) + inputs.dataSize() + outputs.dataSize();
        _data = std::shared_ptr<char>(new char[_dataSize]);
        char * p = _data.get();
        p = saveShiftIntegralInData(InitialServerMessageSpecifyer::SUCCESS_ADD_SIM, p);
        p = saveShiftIntegralInData(id, p);
        inputs.saveVariablesTo(p);
        p = shiftDataAccessable<VariableList>(inputs, p);
        outputs.saveVariablesTo(p);
    }

    AddSimSuccessMessage::AddSimSuccessMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialServerMessageSpecifyer>(getIntegralFromData<InitialServerMessageSpecifyer>(data.get())),
              _data(data),
              _dataSize(0)
    {
    }

    char * AddSimSuccessMessage::data()
    {
        return _data.get();
    }

    const char * AddSimSuccessMessage::data() const
    {
        return _data.get();
    }

    size_t AddSimSuccessMessage::dataSize() const
    {
        return _dataSize;
    }

    const int & AddSimSuccessMessage::getSimId() const
    {
        return *reinterpret_cast<int*>(shift<InitialServerMessageSpecifyer>(_data.get()));
    }

    VariableList AddSimSuccessMessage::getInputVariableList() const
    {
        return VariableList::getVariableListFromData(shift<const int>(shift<const InitialServerMessageSpecifyer>(_data.get())));
    }

    VariableList AddSimSuccessMessage::getOutputVariableList() const
    {
        return VariableList::getVariableListFromData(shift<const int>(shiftDataAccessable<VariableList>(getInputVariableList(), shift<const InitialServerMessageSpecifyer>(_data.get()))));
    }

    std::string AddSimSuccessMessage::getPath()
    {
        return createStringFromData(shift<int>(shift<InitialServerMessageSpecifyer>(_data.get())));
    }
}
