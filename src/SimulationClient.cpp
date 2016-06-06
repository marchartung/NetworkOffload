/*
 * SimulationClient.cpp
 *
 *  Created on: 08.04.2016
 *      Author: Marc Hartung
 */

#include "../include/SimulationClient.hpp"

#include "../include/messages/messages.hpp"
#include "../include/network_impl/SimNetworkFunctions.hpp"
#include <stdexcept>

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
        _possibleInputVarNames.resize(simId + 1);
        _possibleOutputVarNames.resize(simId + 1);

        _selectedInputVarNames.resize(simId + 1);
        _selectedOutputVarNames.resize(simId + 1);

        _inputMessages.resize(simId + 1);
        _outputMessages.resize(simId + 1);
        _isInitialized.resize(simId + 1, false);

        AddSimRequestMessage req(simId, serverPathToSim);
        sendInitialRequest(req);
        AddSimSuccessMessage varMessage(
                recvInitialServerSuccess<AddSimSuccessMessage>(InitialServerMessageSpecifyer::SUCCESS_ADD_SIM,
                                                               "SimulationClient: Couldn't initialize simulation."));
        _possibleInputVarNames[simId] = varMessage.getInputVariableList();
        _possibleOutputVarNames[simId] = varMessage.getOutputVariableList();
        return simId;
    }

    VariableList SimulationClient::getPossibleInputVariableNames(const int & simId)
    {
        if (_currentState < CurrentState::INITED || simId >= (long long int)_possibleInputVarNames.size())
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to get variables names before calling initialize(host,port).");
        return _possibleInputVarNames[simId];
    }

    VariableList SimulationClient::getPossibleOuputVariableNames(const int & simId)
    {
        if (_currentState < CurrentState::INITED || simId >= (long long int)_possibleOutputVarNames.size())
            throw std::runtime_error("ERROR: SimulationClient: It's not possible to get variables names before calling initialize(host,port).");
        return _possibleOutputVarNames[simId];
    }

    ValueContainer& SimulationClient::initializeSimulation(const int& simId, const VariableList& inputs, const VariableList& outputs, const double* inputsReal,
                                                           const int* inputsInt, const char* inputsBool)
    {
        _selectedInputVarNames[simId] = inputs;
        if (!_selectedInputVarNames[simId].isSubsetOf(_possibleInputVarNames[simId]))
            throw std::runtime_error("SimulationClient: Input variable names passed to initializeSimulation, which aren't supported by server.");
        _selectedOutputVarNames[simId] = outputs;
        if (!_selectedOutputVarNames[simId].isSubsetOf(_possibleOutputVarNames[simId]))
            throw std::runtime_error("SimulationClient: Output variable names passed to initializeSimulation, which aren't supported by server.");

        _inputMessages[simId] = ValueContainerMessage<ClientMessageSpecifyer>(simId, inputs, ClientMessageSpecifyer::INPUTS);
        _outputMessages[simId] = ValueContainerMessage<ServerMessageSpecifyer>(simId, outputs, ServerMessageSpecifyer::OUTPUTS);

        InitSimulationMessage initMessage(simId, inputs, outputs);
        this->sendInitialRequest(initMessage);
        if (inputsReal != nullptr)
            _inputMessages[simId].getContainer().setRealValues(inputsReal);
        if (inputsInt != nullptr)
            _inputMessages[simId].getContainer().setIntValues(inputsInt);
        if (inputsBool != nullptr)
            _inputMessages[simId].getContainer().setBoolValues(inputsBool);
        _inputMessages[simId].setSpecifyer(ClientMessageSpecifyer::INPUTS);
        send(simId);
        _isInitialized[simId] = recv(simId, 0.0, ServerMessageSpecifyer::SUCCESS_SIM_INIT);
        _isInitialized[simId] = true;
        return _outputMessages[simId].getContainer();
    }

    bool SimulationClient::getSimulationFile(const int & simId, const std::string & sourcePath, const std::string & targetPath)
    {
        try {
            GetFileMessage message(simId, sourcePath);
            sendInitialRequest(message);
            std::shared_ptr<size_t> numBytes(new size_t[1]);
            // Receive file content.
            std::shared_ptr<char> data = _netClient.variableRecv(numBytes.get());
            // And store it in given target path.
            GetFileSuccessMessage sucMessage(*numBytes, data, targetPath);
        }
        catch (std::runtime_error& ex)
        {
            std::cout << "Something went wrong in " << __FILE__ << " line " << __LINE__ << std::endl;
            return false;
        }
        return true;
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
        _netClient.send(reinterpret_cast<const char *>(&_inputMessages[simId].getId()), sizeof(int));  //send id
        return _netClient.send(_inputMessages[simId].data(), _inputMessages[simId].dataSize());
    }

    const std::string& SimulationClient::getHostAddress() const
    {
        return _hostAddress;
    }

    void SimulationClient::setHostAddress(const std::string& hostAddress)
    {
        _hostAddress = hostAddress;
    }

    int SimulationClient::getPort() const
    {
        return _port;
    }

    void SimulationClient::setPort(int port)
    {
        _port = port;
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

        recv(simId, time, ServerMessageSpecifyer::OUTPUTS);
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
}
