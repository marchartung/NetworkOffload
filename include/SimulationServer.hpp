/*
 * NetworkOffloader.hpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
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

        VariableList getInputVariables(const int & simId) const;
        VariableList getOutputVariables(const int & simId) const;

        bool confirmSimulationAdd(const int & simId, const VariableList & varNames);
        bool confirmSimulationInit(const int & simId, const ValueContainer & initialOutputs);

        ValueContainer & getInputValueContainer(const int & simId);
        ValueContainer & getOutputValueContainer(const int & simId);

        void confirmStart();
        void confirmPause();
        void confirmUnpause();
        void confirmReset();

        const int & getPort() const
        {
            return _port;
        }

        void setPort(const int & port)
        {
            this->_port = port;
        }

        /////////////////////////////////////////
        ////////////RUNNING CONNECTION///////////
        /////////////////////////////////////////

        ClientMessageSpecifyer getClientRequest();


        const double & getLastReceivedTime(const int& simId) const;
        int getLastSimId() const;

        bool sendOutputValues(const int & simId, const double & time, const ValueContainer & vals);
        ValueContainer & recvInputValues(const int & simId);


     private:
        int _port;
        size_t _initialResponseTime;

        CurrentState _currentState;

        std::map<std::string,int> _pathToId;
        NetworkServer _netServer;

        InitialClientMessageSpecifyer _lastInitSpec;
        ClientMessageSpecifyer _lastSpec;
        std::vector<double> _lastReceivedTime;
        int _lastSimId;


        std::pair<const std::string,int> * _lastAddedSim;

        bool _handledLastRequest;

        std::vector<VariableList> _varNames;
        std::vector<VariableList> _inputVarNames;
        std::vector<VariableList> _outputVarNames;

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

        void prepareStart();

        template<typename MessageType>
        bool sendInitialRequest(MessageType & in)
        {
            return _netServer.variableSend(in.data(), in.dataSize());
        }
    };

}

#endif /* INCLUDE_SIMULATIONSERVER_HPP_ */
