/*
 * FmuServer.hpp
 *
 *  Created on: 08.04.2016
 *      Author: hartung
 */

#include "../include/SimulationServer.hpp"
#include "../include/messages/InitSimulationMessage.hpp"
#include "../include/messages/RuntimeMessage.hpp"
#include "../include/messages/AddSimMessage.hpp"


namespace NetOff
{

    SimulationServer::SimulationServer()
            : _port(-1),
              _initialResponseTime(100),
              _currentState(CurrentState::NONE),
              _lastInitSpec(InitialClientMessageSpecifyer::CLIENT_INIT_ABORT),
              _lastSpec(ClientMessageSpecifyer::RESET),
              _lastReceivedTime(0.0),
              _lastSimId(-1),
              _lastAddedSim(nullptr),
              _handledLastRequest(true)
    {
    }

    SimulationServer::SimulationServer(const int& port)
            : _port(port),
              _initialResponseTime(100),
              _currentState(CurrentState::NONE),
              _lastInitSpec(InitialClientMessageSpecifyer::CLIENT_INIT_ABORT),
              _lastSpec(ClientMessageSpecifyer::RESET),
              _lastReceivedTime(0.0),
              _lastSimId(-1),
              _lastAddedSim(nullptr),
              _handledLastRequest(true)
    {
    }

    SimulationServer::~SimulationServer()
    {
        deinitialize();
    }

    bool SimulationServer::initializeConnection()
    {
        if (_port < 1)
        {
            std::cout << "SimulationServer: Port wasn't set.\n";
            return false;
        }
        else if (_currentState > CurrentState::NONE)
            return true;

        if (_netServer.initialize(_port))
        {
            _currentState = CurrentState::INITED;
            return true;
        }
        else
            return false;
    }

    void SimulationServer::deinitialize()
    {
        _netServer.deinitialize();
    }

    InitialClientMessageSpecifyer SimulationServer::getInitialClientRequest()
    {
        if (!_handledLastRequest || _currentState < CurrentState::INITED)
            throw std::runtime_error("SimulationServer: Last initial request wasn't handled or the server wasn't initiated.");
        _handledLastRequest = false;
        std::shared_ptr<char> data = recvInitialMessage();
        //start of message is always a InitialClientMessageSpecifyer
        _lastInitSpec = *reinterpret_cast<InitialClientMessageSpecifyer *>(data.get());
        switch (_lastInitSpec)
        {
            case InitialClientMessageSpecifyer::CLIENT_INIT_ABORT:

                break;
            case InitialClientMessageSpecifyer::ADD_SIM:
                prepareAddSim(data);
                break;
            case InitialClientMessageSpecifyer::INIT_SIM:
                prepareInitSim(data);
                break;
            case InitialClientMessageSpecifyer::START:
                prepareStart();
                break;
            default:
                break;
        }
        return _lastInitSpec;
    }

    std::tuple<std::string, int> SimulationServer::getAddedSimulation()
    {
        if (_handledLastRequest || _currentState < CurrentState::INITED || _lastInitSpec != InitialClientMessageSpecifyer::ADD_SIM)
        {
            throw std::runtime_error("SimulationServer: Couln't add FMU. Wrong function called.");
        }

        if (_lastAddedSim == nullptr)
            throw std::runtime_error("SimulationServer: No FMU added jet.");

        _handledLastRequest = true;
        return *_lastAddedSim;
    }

    int SimulationServer::getLastSimId() const
    {
        return _lastSimId;
    }

    VariableList SimulationServer::getInputVariables(const int& simId) const
    {
        if (_inputVarNames[simId].empty() || _currentState < CurrentState::INITED || simId >= _inputVarNames.size())
            throw std::runtime_error("ERROR: SimulationServer: Input container can't be returned. The simulation hasn't been initialized.");
        return _inputVarNames[simId];
    }

    VariableList SimulationServer::getOutputVariables(const int& simId) const
    {
        if (_outputVarNames[simId].empty() || _currentState < CurrentState::INITED || simId >= _outputVarNames.size())
            throw std::runtime_error("ERROR: SimulationServer: Output container can't be returned. The simulation hasn't been initialized.");
        return _outputVarNames[simId];
    }

    bool SimulationServer::confirmSimulationAdd(const int& simId, const VariableList& varNames)
    {
        if (!_handledLastRequest || _currentState < CurrentState::INITED || _lastInitSpec != InitialClientMessageSpecifyer::ADD_SIM)
        {
            throw std::runtime_error("SimulationServer: Cannot send variable names.");
        }
        _varNames[simId] = varNames;
        AddSimSuccessMessage answer(simId, varNames);
        sendInitialRequest(answer);
        _handledLastRequest = true;
        return true;
    }

    bool SimulationServer::confirmSimulationInit(const int& simId, const ValueContainer& initialOutputs)
    {
        if (_handledLastRequest || _currentState < CurrentState::INITED || _lastInitSpec != InitialClientMessageSpecifyer::INIT_SIM
                || initialOutputs.getSimId() != simId)
            throw std::runtime_error("SimulationServer: Cannot confirm simulation intialization.");
        _outputMessages[simId].setSpecifyer(ServerMessageSpecifyer::SUCCESS_SIM_INIT);
        _outputMessages[simId].setTime(0.0);
        _handledLastRequest = true;

        return sendMessage(simId);
    }

    void SimulationServer::confirmStart()
    {
        if (_handledLastRequest || _currentState < CurrentState::INITED || _lastInitSpec != InitialClientMessageSpecifyer::START)
        {
            throw std::runtime_error("SimulationServer: Cannot confirm start.");
        }
        StartSuccessMessage answer;
        sendInitialRequest(answer);
        _handledLastRequest = true;
        _currentState = CurrentState::STARTED;
    }

    void SimulationServer::confirmPause()
    {
        if (_handledLastRequest || _currentState < CurrentState::STARTED || _lastSpec != ClientMessageSpecifyer::PAUSE)
            std::runtime_error("SimulationServer: Cannot confirm pause.");

        _outputMessages[0].setSpecifyer(ServerMessageSpecifyer::SUCCESS_PAUSE);
        sendMessage(0);
        _handledLastRequest = true;
    }

    void SimulationServer::confirmUnpause()
    {
        if (_handledLastRequest || _currentState < CurrentState::STARTED || _lastSpec != ClientMessageSpecifyer::UNPAUSE)
            std::runtime_error("SimulationServer: Cannot confirm unpause.");
        _outputMessages[0].setSpecifyer(ServerMessageSpecifyer::SUCCESS_UNPAUSE);
        sendMessage(0);
        _handledLastRequest = true;
    }

    void SimulationServer::confirmReset()
    {
        if (_handledLastRequest || _currentState < CurrentState::STARTED || _lastSpec != ClientMessageSpecifyer::RESET)
            std::runtime_error("SimulationServer: Cannot confirm reset.");
        _outputMessages[0].setSpecifyer(ServerMessageSpecifyer::SUCCESS_RESET);
        sendMessage(0);
        _handledLastRequest = true;
    }

    void SimulationServer::prepareAddSim(std::shared_ptr<char>& data)
    {
        AddSimRequestMessage message(data);
        if (!message.testRequest(InitialClientMessageSpecifyer::ADD_SIM))
        {
            throw std::runtime_error("SimulationServer: some how tried to add fmu, but the message was misinterpret.");
        }
        int simId = message.getSimId();
        std::string fmuPath = message.getPath();
        auto it = _pathToId.insert(std::make_pair(fmuPath, simId));
        if (!it.second)
            throw std::runtime_error("SimulationServer: Fmu already added.");
        _lastAddedSim = &(*it.first);
        _varNames.resize(simId + 1);
        _inputVarNames.resize(simId + 1);
        _outputVarNames.resize(simId + 1);
        _lastReceivedTime.resize(simId + 1, -1.0);
        _inputMessages.resize(simId + 1);
        _outputMessages.resize(simId + 1);
        _isInitialized.resize(simId+1,false);
    }

    void SimulationServer::prepareInitSim(std::shared_ptr<char>& data)
    {
        InitSimulationMessage initMessage(data);

        _lastSimId = initMessage.getSimId();
        _inputVarNames[_lastSimId] = initMessage.getInputs();
        _outputVarNames[_lastSimId] = initMessage.getOutputs();

        _inputMessages[_lastSimId] = ValueContainerMessage<ClientMessageSpecifyer>(_lastSimId, _inputVarNames[_lastSimId], ClientMessageSpecifyer::INPUTS);
        _outputMessages[_lastSimId] = ValueContainerMessage<ServerMessageSpecifyer>(_lastSimId, _outputVarNames[_lastSimId], ServerMessageSpecifyer::OUTPUTS);

        if(_lastSimId != this->recvMessage())
            throw std::runtime_error("SimulationServer: Internal error occured. Received initial values invalid.");

        _isInitialized[_lastSimId] = true;

    }

    bool SimulationServer::sendMessage(const int& simId)
    {
        return _netServer.send(_outputMessages[simId].data(),_outputMessages[simId].dataSize());
    }

    bool SimulationServer::sendOutputValues(const int& simId, const double& time, const ValueContainer& vals)
    {
        if (_handledLastRequest || _currentState < CurrentState::STARTED || ClientMessageSpecifyer::INPUTS != _lastSpec || _lastSimId != simId)
            std::runtime_error("SimulationServer: Couldn't send output values");
        _handledLastRequest = true;
        return sendMessage(simId);
    }

    ValueContainer& SimulationServer::recvInputValues(const int& simId)
    {
        if (_handledLastRequest || _currentState < CurrentState::STARTED || ClientMessageSpecifyer::INPUTS != _lastSpec || _lastSimId != simId)
            std::runtime_error("SimulationServer: Couldn't recv output values. Error occured.");
        _handledLastRequest = true;
        return getInputValueContainer(simId);
    }

    void SimulationServer::prepareStart()
    {

    }

    ClientMessageSpecifyer SimulationServer::getClientRequest()
    {
        if (!_handledLastRequest || _currentState < CurrentState::STARTED)
            throw std::runtime_error("SimulationServer: Last request wasn't handled or the server wasn't started.");
        _handledLastRequest = false;
        int simId = recvMessage();  // sets id and data in _outputContainer[simId]
        _lastSpec = _inputMessages[simId].getSpecifyer();
        _lastReceivedTime[simId] = _inputMessages[simId].getTime();
        return _lastSpec;
    }

    ValueContainer& SimulationServer::getInputValueContainer(const int& simId)
    {
        if (!_isInitialized[_lastSimId] || _currentState < CurrentState::INITED)
        {
            throw std::runtime_error("SimulationServer: Cannot create input container. Server not started.");
        }
        return _inputMessages[simId].getContainer();
    }

    ValueContainer& SimulationServer::getOutputValueContainer(const int& simId)
    {
        if (!_isInitialized[_lastSimId] || _currentState < CurrentState::INITED)
        {
            throw std::runtime_error("SimulationServer: Cannot create input container. Server not started.");
        }
        return _outputMessages[simId].getContainer();
    }

    const double & SimulationServer::getLastReceivedTime(const int& simId) const
    {
        return _lastReceivedTime[simId];
    }

    std::shared_ptr<char> SimulationServer::recvInitialMessage()
    {
        return _netServer.variableRecv();
    }

    int SimulationServer::recvSimulationId()
    {
        _netServer.recv(reinterpret_cast<char *>(&_lastSimId), sizeof(int));
        return _lastSimId;
    }

    int SimulationServer::recvMessage()
    {
        recvSimulationId();
        _netServer.recv(_inputMessages[_lastSimId].data(), _inputMessages[_lastSimId].dataSize());
        return _lastSimId;
    }
}
