/*
 * NetworkOffloader.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_SIMULATIONSERVER_HPP_
#define INCLUDE_SIMULATIONSERVER_HPP_

#include <string>
#include <map>
#include <vector>

#include "AdditionalTypes.hpp"
#include "ValueContainer.hpp"
#include "VariableList.hpp"
#include "network_impl/NetworkServer.hpp"
#include "messages/ValueContainerMessage.hpp"

namespace NetOff
{
    class SimulationServer
    {
     public:

        SimulationServer();

        SimulationServer(const int & port);

        SimulationServer(const SimulationServer& rhs) = delete;
        SimulationServer& operator=(const SimulationServer& rhs) = delete;

        ~SimulationServer();


        /////////////////////////////////////////
        //////////////DE-/INITIALIZATION/////////////
        /////////////////////////////////////////

        /**
         * \brief Set ups the connection between server and host and initializes every FMU given
         *
         * @return bool Returns true, if the connection could be set up, false otherwise.
         */
        bool initializeConnection();

        /**
         * \brief Shuts down the server
         */
        void deinitialize();


        InitialClientMessageSpecifyer getInitialClientRequest();

        std::tuple<std::string,int> getAddedSimulation();

        std::string getSimulationFileName() const;

        VariableList getSelectedInputVariables(const int & simId) const;
        VariableList getSelectedOutputVariables(const int & simId) const;

        bool confirmSimulationAdd(const int & simId, const VariableList & varNamePossibleInputs, const VariableList & varNamePossibleOutputs);
        bool confirmSimulationInit(const int & simId, const ValueContainer & initialOutputs);

        bool confirmSimulationFile(const int & simId, const std::string & fileSrc);

        ValueContainer & getInputValueContainer(const int & simId);
        ValueContainer & getOutputValueContainer(const int & simId);

        void confirmStart();
        void confirmPause();
        void confirmUnpause();
        void confirmReset();

        const int & getPort() const;
        void setPort(const int & port);

        /////////////////////////////////////////
        ////////////RUNNING CONNECTION///////////
        /////////////////////////////////////////

        ClientMessageSpecifyer getClientRequest();

        const double & getLastReceivedTime(const int& simId) const;
        int getLastSimId() const;

        bool sendOutputValues(const int & simId, const double & time);
        ValueContainer & recvInputValues(const int & simId);
        bool isActive() const;

     private:
        int _port;

        CurrentState _currentState;

        std::map<std::string,int> _pathToId;
        NetworkServer _netServer;

        InitialClientMessageSpecifyer _lastInitSpec;
        ClientMessageSpecifyer _lastSpec;
        std::vector<double> _lastReceivedTime;
        int _lastSimId;

        std::string _lastSimulationFile;

        std::pair<const std::string,int> * _lastAddedSim;

        bool _handledLastRequest;

        std::vector<VariableList> _allInputVarNames;
        std::vector<VariableList> _allOutputVarNames;
        std::vector<VariableList> _selectedInputVarNames;
        std::vector<VariableList> _selectedOutputVarNames;

        std::vector<bool> _isInitialized;

        std::vector<ValueContainerMessage<ClientMessageSpecifyer>> _inputMessages;
        std::vector<ValueContainerMessage<ServerMessageSpecifyer>> _outputMessages;

        //size_t _tmpDataSize;
        //std::shared_ptr<char> _tmpRecvData;

        std::shared_ptr<char> recvInitialMessage();

        int recvSimulationId();

        int recvMessage();

        bool sendMessage(const int & simId);

        void prepareAddSim(std::shared_ptr<char> & data);

        void prepareInitSim(std::shared_ptr<char> & data);

        void prepareSimulationFile(std::shared_ptr<char> & data);

        void prepareStart();

        template<typename MessageType>
        bool sendInitialRequest(MessageType & in)
        {
            return _netServer.variableSend(in.data(), in.dataSize());
        }
    };

}

#endif /* INCLUDE_SIMULATIONSERVER_HPP_ */
