/*
 * NetworkOffloader.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_SIMULATIONCLIENT_HPP_
#define INCLUDE_SIMULATIONCLIENT_HPP_

#include <string>
#include <map>
#include <vector>

#include "AdditionalTypes.hpp"
#include "ValueContainer.hpp"
#include "VariableList.hpp"
#include "network_impl/NetworkClient.hpp"
#include "messages/ValueContainerMessage.hpp"

namespace NetOff
{
    class SimulationClient
    {
     public:

        SimulationClient();

        SimulationClient(const std::string & hostAddress, const int & port, const size_t & initialResponseTime = 1000);

        ~SimulationClient();

        /////////////////////////////////////////
        //////////////DE-/INITIALIZATION/////////
        /////////////////////////////////////////

        /**
         * \brief Set ups the connection between server and host and initializes every FMU given
         *
         * @return bool Returns true, if the connection could be set up, false otherwise.
         */
        bool initializeConnection();

        /**
         * \brief Notifies the server and disconnects the client.
         */
        void deinitialize();

        const std::string& getHostAddress() const;

        void setHostAddress(const std::string& hostAddress);

        int getPort() const;

        void setPort(int port);

        /**
         * \brief Adds fmu to the remote simulation
         * The given path is send to the server. The server should initialize this FMU and can now response to further requests like getVariableNames().
         * @return int ID of the added FMU
         */
        int addSimulation(const std::string & serverPathToSim);

        /** \brief Copies simulation file from remote server to localhost.
         *
         * @remark: This method can be used in a more general way to copy any desired file from
         *          server to localhost;-)
         *
         * @param simId ID of the desired simulation.
         * @param sourcePath File (with full path) on server.
         * @param targetPath File (with full path) on localhost.
         * @return True
         *
         * \todo: Always returns true. Implement error handling.
         */
        bool getSimulationFile(const int & simId, const std::string & sourcePath, const std::string & targetPath);

        VariableList getPossibleInputVariableNames(const int & simId);

        VariableList getPossibleOuputVariableNames(const int & simId);
        /**
         * \brief Initializes the variables the server should provide for the given .
         *  The function takes the simulation ID, the identifier for the inputs variables and the identifier for the output variables.
         *  Optional it's possible to send initial values for the input values. All value indices are similar to the indices of the variables in the VariableLists
         *  @arg simId The simulation ID returned by addSimulation
         *  @arg inputs VariableList containing all variables the client sends to the server. The order determines the order in the input ValueContainer.
         *  @arg outputs VariableList containing all variables the client receives from the server. The order determines the order in the output ValueContainer.
         *  @arg inputsReal A double pointer to the initial values of the real variables. This argument is optional.
         *  @arg inputsInt A integer pointer to the initial values of the integer variables. This argument is optional.
         *  @arg inputsBool A char pointer to the initial values of the bool variables. There's no native correct way to present bools in memory, so they are presented as chars. This argument is optional.
         *  @return Returns the initial values of the simulation. Most times needed for setting up the client functionality
         */
        ValueContainer & initializeSimulation(const int & simId, const VariableList & inputs, const VariableList & outputs, const double * inputsReal = nullptr,
                                              const int * inputsInt = nullptr, const char * inputsBool = nullptr);

        /////////////////////////////////////////
        ////////////RUNNING CONNECTION///////////
        /////////////////////////////////////////

        bool start();

        bool pause();

        bool unpause();

        bool reset();

        ValueContainer & getInputValueContainer(const int & simId);
        ValueContainer & getOutputValueContainer(const int & simId);

        ValueContainer & recvOutputValues(const int & simId, const double & time);

        bool sendInputValues(const int & simId, const double & time, const ValueContainer & vals);

        bool isStarted() const;

     private:

        /////////////////////////////////////////
        //////////////DE-/INITIALIZATION/////////
        /////////////////////////////////////////

        std::string _hostAddress;
        int _port;
        size_t _initialResponseTime;
        std::map<std::string, int> _pathToId;

        CurrentState _currentState;

        NetworkClient _netClient;

        std::vector<VariableList> _possibleInputVarNames;
        std::vector<VariableList> _possibleOutputVarNames;
        std::vector<VariableList> _selectedInputVarNames;
        std::vector<VariableList> _selectedOutputVarNames;

        std::vector<bool> _isInitialized;

        /////////////////////////////////////////
        ////////////RUNNING CONNECTION///////////
        /////////////////////////////////////////

        std::vector<ValueContainerMessage<ClientMessageSpecifyer> > _inputMessages;
        std::vector<ValueContainerMessage<ServerMessageSpecifyer> > _outputMessages;

        bool send(const int simId);

        bool recv(const int simId, const double & expectedTime, const ServerMessageSpecifyer & spec = ServerMessageSpecifyer::OUTPUTS);

        template<typename MessageType>
        bool sendInitialRequest(MessageType & in)
        {
            return _netClient.variableSend(in.data(), in.dataSize());
        }

        template<typename MessageType>
        MessageType recvInitialServerSuccess(const InitialServerMessageSpecifyer & in, const std::string & erroMessage, size_t * numBytes = nullptr)
        {
            auto answer = _netClient.variableRecv(numBytes);
            MessageType sr(answer);
            if (!sr.testRequest(in))
                throw std::runtime_error(erroMessage);
            return sr;
        }

        template<typename MessageType>
        bool recvServerRequest(MessageType & out, const ServerMessageSpecifyer & should, const std::string & errorMessage)
        {
            bool res = _netClient.recv(out.data(), out.dataSize());
            if (!out.testRequest(should))
            {
                throw std::runtime_error(errorMessage);
            }
            return res;
        }

    };

}

#endif /* INCLUDE_SIMULATIONCLIENT_HPP_ */
