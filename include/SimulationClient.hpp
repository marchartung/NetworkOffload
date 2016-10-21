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

        /*! \brief Default constructs a SimulationClient object with default member values. */
        SimulationClient();

        /*! \brief Constructs a SimulationClient object from the given server address and port.
         *
         * \param hostAddress   Address of the server to connect this SimulationClient.
         * \param port          The port to use for the connection.
         */
        SimulationClient(const std::string & hostAddress, const int & port);

        /*! \brief Destructs the SimulationClient by killing all sockets. */
        ~SimulationClient();

        /////////////////////////////////////////
        //////////////DE-/INITIALIZATION/////////
        /////////////////////////////////////////

        /** \brief Set ups the connection between server and host.
         *
         * @return bool Returns true, if the connection was established, false otherwise.
         */
        bool initializeConnection();

        /** \brief Notifies the server and disconnects the client. */
        void deinitialize();

        const std::string& getHostAddress() const;
        void setHostAddress(const std::string& hostAddress);

        int getPort() const;
        void setPort(int port);

        /** \brief Adds a FMU to the remote simulation.
         *
         * The given path is send to the server. The server tries to initialize the FMU and can now response to further
         * requests like \ref getVariableNames().
         *
         * \param serverPathToSim Path and FMU file name.
         * \exception std::runtime_error SimulationClient was not already initialized.
         * \return ID of the FMU to add.
         */
        int addSimulation(const std::string & serverPathToSim);

        /** \brief Copies simulation file from remote server to localhost.
         *
         * @remark: This method can be used in a more general way to copy any desired file from server to localhost;-)
         *
         * @param simId         ID of the desired simulation.
         * @param sourcePath    File (with full path) on server.
         * @param targetPath    File (with full path) on localhost.
         * @return True
         *
         * \todo: Always returns true. Implement error handling.
         */
        bool getSimulationFile(const int & simId, const std::string & sourcePath, const std::string & targetPath);

        VariableList getPossibleInputVariableNames(const int & simId);

        VariableList getPossibleOuputVariableNames(const int & simId);

        /** \brief Initializes the variables the server should provide for the given simulation.
         *
         * The function takes the simulation ID, the identifier for the inputs variables and the identifier for the
         * output variables. Optional it is possible to send initial values for the input values. All value indices
         * are similar to the indices of the variables in the VariableLists.
         *
         * @param simId         The simulation ID returned by \ref addSimulation()
         * @param inputs        VariableList containing all variables the client sends to the server. The order
         *                      determines the order in the input ValueContainer.
         * @param outputs       VariableList containing all variables the client receives from the server. The order
         *                      determines the order in the output ValueContainer.
         * @param inputsReal    A double pointer to the initial values of the real variables. This argument is optional.
         * @param inputsInt     A integer pointer to the initial values of the integer variables. This argument is optional.
         * @param inputsBool    A char pointer to the initial values of the bool variables. There's no native correct way
         *                      to present bools in memory, so they are presented as chars. This argument is optional.
         * @return Returns the initial values of the simulation. Most times needed for setting up the client functionality.
         */
        ValueContainer & initializeSimulation(const int & simId, const VariableList & inputs,
                                              const VariableList & outputs, const double * inputsReal = nullptr,
                                              const int * inputsInt = nullptr, const char * inputsBool = nullptr);

        /////////////////////////////////////////
        ////////////RUNNING CONNECTION///////////
        /////////////////////////////////////////

        /*! \brief Starts the simulation.
         *
         * This function checks that all simulations are initialized. If so, a StartRequestMessage is send to the
         * server.
         * \exception std::runtime_error At least one simulation was not initialized.
         * \return True, if the simulation was started.
         */
        bool start();

        /*! \brief Pause the simulation.
         *
         * The new current status is CurrentStatus::PAUSED.
         *
         * \exception std::runtime_error Simulation was not yet started.
         * \return True, if simulation is paused.
         */
        bool pause();

        /*! \brief Unpause the simulation.
         *
         * The new current status is CurrentStatus::STARTED.
         *
         * \exception std::runtime_error Simulation was not yet started.
         * \return True, if the simulation was unpaused.
         */
        bool unpause();

        /*! \brief Reset the simulation.
         *
         * The new current status is CurrentStatus::INITED.
         *
         * \exception std::runtime_error Simulation was not yet started.
         * \return True, if the simulation was reseted.
         */
        bool reset();

        ValueContainer & getInputValueContainer(const int & simId);
        ValueContainer & getOutputValueContainer(const int & simId);

        /*! \brief Receive the output values for the given simulation and time from the server.
         *
         * Because we have a asynchronous communication, the time value is used to unambiguously identify the output
         * values on client side and also when sending the corresponding input values via \ref sendInputValues().
         *
         * \param simId ID of the simulation.
         * \param time  Value of time step.
         * \exception std::runtime_error Simulation was not yet started.
         * \return ValueContainer containing the output values.
         */
        ValueContainer & recvOutputValues(const int & simId, const double & time);

        /*! \brief Sends input values of the given simulation and time to the server.
         *
         * Because we have a asynchronous communication, the time value is used to unambiguously identify the input
         * values on server side and also when receiving the corresponding output values via \ref recvOutputValues().
         *
         * \param simId ID of the simulation.
         * \param time  Value of time step.
         * \param vals  The input values.
         * \exception std::runtime_error Simulation was not yet started.
         * \exception std::runtime_error Given simulation ID and simulation ID of vals to not match.
         * \return True, if the values were successfully sent.
         */
        bool sendInputValues(const int & simId, const double & time, const ValueContainer & vals);

        /*! \brief Returns true, if the current status is CurrentStatus::STARTED. */
        bool isStarted() const;

     private:

        /////////////////////////////////////////
        //////////////DE-/INITIALIZATION/////////
        /////////////////////////////////////////

        std::string _hostAddress;
        int _port;
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

        /*! \brief Send the input values of the simulation specified by its ID.
         *
         * \param simId ID of the simulation
         * @return True, if values were send successfully.
         */
        bool send(const int simId);

        /*! \brief Receive the output values of the simulation specified by its ID.
         *
         * Because we have a asynchronous communication, the two values of expectedTime and specification are needed to
         * unambiguously identify the output values.
         *
         * \param simId         ID of the simulation
         * \param expectedTime  Time the values are needed for.
         * \param spec          Specifier to identify message and thus output values.
         * @return True, if values were send successfully.
         */
        bool recv(const int simId, const double & expectedTime, const ServerMessageSpecifyer & spec =
                          ServerMessageSpecifyer::OUTPUTS);

        template<typename MessageType>
        bool sendInitialRequest(MessageType & in)
        {
            return _netClient.variableSend(in.data(), in.dataSize());
        }

        template<typename MessageType>
        MessageType recvInitialServerSuccess(const InitialServerMessageSpecifyer & in, const std::string & erroMessage,
                                             size_t * numBytes = nullptr)
        {
            auto answer = _netClient.variableRecv(numBytes);
            MessageType sr(answer);
            if (!sr.testRequest(in))
                throw std::runtime_error(erroMessage);
            return sr;
        }

        template<typename MessageType>
        bool recvServerRequest(MessageType & out, const ServerMessageSpecifyer & should,
                               const std::string & errorMessage)
        {
            bool res = _netClient.recv(out.data(), out.dataSize());
            if (!out.testRequest(should))
                throw std::runtime_error(errorMessage);
            return res;
        }

    };

}  // End namespace NetOff

#endif /* INCLUDE_SIMULATIONCLIENT_HPP_ */
