/*
 * SimulationClient.cpp
 *
 *  Created on: 08.04.2016
 *      Author: hartung
 */

#include "../include/SimulationClient.hpp"

#include "../include/messages.hpp"
#include "../include/network_impl/SimNetworkFunctions.hpp"

namespace NetOff
{

    SimulationClient::SimulationClient()
            : _hostAddress(""),
              _port(-1),
              _initialResponseTime(1000),
              _currentState(CurrentState::NONE)
    {
    }

    SimulationClient::SimulationClient(const std::string& hostAddress, const int & port, const size_t & initialResponseTime)
            : _hostAddress(hostAddress),
              _port(port),
              _initialResponseTime(initialResponseTime),
              _currentState(CurrentState::NONE)
    {
    }

    SimulationClient::~SimulationClient()
    {
        _netClient.deinitialize();
    }

    bool SimulationClient::initializeConnection()
    {
        if (_port < 1 || _hostAddress.size() < 1)
        {
            std::cout << "SimulationClient: Port or host address wasn't set.\n";
            return false;
        }
        else if (_currentState > CurrentState::NONE)
            return true;

        if (_netClient.initialize(_hostAddress, _port))
        {
            _currentState = CurrentState::INITED;
            return true;
        }
        else
            return false;
    }

    void SimulationClient::deinitialize()
    {
        _currentState = CurrentState::NONE;

        _inputMessages[0].setSpecifyer(ClientMessageSpecifyer::CLIENT_ABORT);
        send(0);
        _netClient.deinitialize();
    }

    int SimulationClient::addSimulation(const std::string & serverPathToSim)
    {
        if (_currentState == CurrentState::NONE)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to add simulation before calling initialize(host,port).");
        if (_pathToId.find(serverPathToSim) != _pathToId.end())
            return _pathToId[serverPathToSim];

        const int & simId = _pathToId.size();

        _pathToId[serverPathToSim] = simId;
        _varNames.resize(simId + 1);

        _inputVarNames.resize(simId + 1);
        _outputVarNames.resize(simId + 1);

        _inputMessages.resize(simId + 1);
        _outputMessages.resize(simId + 1);
        _isInitialized.resize(simId + 1, false);

        AddSimRequestMessage req(simId, serverPathToSim);
        sendInitialRequest(req);
        AddSimSuccessMessage varMessage(
                recvInitialServerSuccess<AddSimSuccessMessage>(InitialServerMessageSpecifyer::SUCCESS_ADD_SIM,
                                                                      "SimulationClient: Couldn't initialize simulation."));
        _varNames[simId] = varMessage.getVariableList();
        return simId;
    }

    VariableList SimulationClient::getVariableNames(const int & simId)
    {
        if (_currentState < CurrentState::INITED || simId >= _varNames.size())
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to get variables names before calling initialize(host,port).");
        return _varNames[simId];
    }

    bool SimulationClient::start()
    {
        bool abortSim = false;

        for (const bool & i : _isInitialized)
            if (!i)
            {
                abortSim = true;
                break;
            }
        if (abortSim || _isInitialized.empty())
        {
            AbortRequestMessage abortM;
            sendInitialRequest(abortM);
            throw std::runtime_error("ERROR: SimulationClient: No simulation added or at least one simulation is not initialized.");
        }
        StartRequestMessage startRequest;
        sendInitialRequest(startRequest);
        recvInitialServerSuccess<StartSuccessMessage>(InitialServerMessageSpecifyer::SUCCESS_START, "SimulationClient: Can't start server.");

        _currentState = CurrentState::STARTED;

        return true;
    }

    bool SimulationClient::pause()
    {
        if (_currentState < CurrentState::STARTED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to pause before calling start().");

        _inputMessages[0].setSpecifyer(ClientMessageSpecifyer::PAUSE);
        send(0);

        bool res = recv(0, _inputMessages[0].getTime(), ServerMessageSpecifyer::SUCCESS_PAUSE);
        if (res)
            _currentState = CurrentState::PAUSED;
        return res;
    }

    bool SimulationClient::unpause()
    {
        if (_currentState < CurrentState::STARTED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to unpause before calling start().");

        _inputMessages[0].setSpecifyer(ClientMessageSpecifyer::UNPAUSE);
        send(0);

        bool res = recv(0, _inputMessages[0].getTime(), ServerMessageSpecifyer::SUCCESS_UNPAUSE);
        if (res)
            _currentState = CurrentState::STARTED;
        return res;
    }

    bool SimulationClient::reset()
    {
        if (_currentState < CurrentState::STARTED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to reset before calling start().");

        _inputMessages[0].setSpecifyer(ClientMessageSpecifyer::RESET);
        send(0);

        bool res = recv(0, _inputMessages[0].getTime(), ServerMessageSpecifyer::SUCCESS_RESET);
        if (res)
            _currentState = CurrentState::INITED;
        return res;
    }

    bool SimulationClient::send(const int simId)
    {
        _netClient.send(reinterpret_cast<const char *>(&_inputMessages[simId].getId()),sizeof(int)); //send id
        return _netClient.send(_inputMessages[simId].data(), _inputMessages[simId].dataSize());
    }

    bool SimulationClient::recv(const int simId, const double & expectedTime, const ServerMessageSpecifyer & spec)
    {
        _netClient.recv(_outputMessages[simId].data(), _outputMessages[simId].dataSize());
        return _outputMessages[simId].getTime() == expectedTime && _outputMessages[simId].getSpecifyer() == spec;
    }

    ValueContainer & SimulationClient::recvOutputValues(const int& simId, const double& time)
    {
        if (_currentState < CurrentState::STARTED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to receive outputs before calling start().");

        recv(simId, time,ServerMessageSpecifyer::OUTPUTS);
        return _outputMessages[simId].getContainer();
    }

    bool SimulationClient::sendInputValues(const int & simId, const double & time, const ValueContainer & vals)
    {
        if (_currentState < CurrentState::STARTED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to send inputs before calling start().");
        if (vals.getSimId() != simId || vals.data() != getInputValueContainer(simId).data())
        {
            throw std::runtime_error("SimulationClient: The ValueContainer is collected via getValueContainerInput()");
        }
        _inputMessages[simId].setSpecifyer(ClientMessageSpecifyer::INPUTS);
        _inputMessages[simId].setTime(time);
        return send(simId);
    }

    ValueContainer & SimulationClient::getInputValueContainer(const int & simId)
    {
        if (!_isInitialized[simId] || _currentState < CurrentState::INITED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to get a input container before calling initializeSimulation.");
        return _inputMessages[simId].getContainer();
    }

    ValueContainer & SimulationClient::getOutputValueContainer(const int & simId)
    {
        if (!_isInitialized[simId] || _currentState < CurrentState::INITED)
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to get a out container before calling initializeSimulation().");
        return _outputMessages[simId].getContainer();
    }

    ValueContainer& SimulationClient::initializeSimulation(const int& simId, const VariableList& outputs, const VariableList& inputs, const double* inputsReal,
                                                           const int* inputsInt, const char* inputsBool)
    {
        _inputVarNames[simId] = inputs;
        _outputVarNames[simId] = outputs;

        _inputMessages[simId] = ValueContainerMessage<ClientMessageSpecifyer>(simId, inputs, ClientMessageSpecifyer::INPUTS);
        _outputMessages[simId] = ValueContainerMessage<ServerMessageSpecifyer>(simId, outputs, ServerMessageSpecifyer::OUTPUTS);

        InitSimulationMessage initMessage(simId, inputs , outputs, inputsReal, inputsInt, inputsBool);
        this->sendInitialRequest(initMessage);
        _isInitialized[simId] = recv(simId, 0.0,ServerMessageSpecifyer::SUCCESS_SIM_INIT);
        return _outputMessages[simId].getContainer();
    }
}
