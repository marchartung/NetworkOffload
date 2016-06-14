/*
 * SimpleSimServer.hpp
 *
 *  Created on: 19.04.2016
 *      Author: Marc Hartung
 */

#ifndef TEST_SIMPLESIMSERVER_HPP_
#define TEST_SIMPLESIMSERVER_HPP_

#include "../include/SimulationServer.hpp"

#include <fstream>

struct PseudoSim
{
    double states[4] = { 0, 0, 0, 0 };
    double currentTime = 0;

    NetOff::VariableList outVars = NetOff::VariableList( { "x", "y" }, { }, { });
    NetOff::VariableList inVars = NetOff::VariableList( { "dx", "dy" }, { }, { });

    void init(const double * in)
    {
        for (size_t i = 0; i < 2; ++i)
            states[i + 2] = in[i];
    }

    std::vector<double> solve(const double * in, const double & time)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            states[i] += in[i] * (time - currentTime);
            states[i + 2] = in[i];
        }

        currentTime = time;
        return
        {   states[0],states[1],states[2],states[3]};
    }
};

int serverFunc(int port)
{
    NetOff::SimulationServer noFS(port);
    // open server
    if (!noFS.initializeConnection())
        throw std::runtime_error("Couldn't open server.");

    std::vector<PseudoSim> sims;

    // now wait for initialization requests of a client:
    bool run = true;
    NetOff::InitialClientMessageSpecifyer spec;
    int requestedSim, newSimId;
    std::string newSimName;
    std::vector<std::string> realVars;
    while (run)
    {
        spec = noFS.getInitialClientRequest();
        switch (spec)
        {
            case NetOff::InitialClientMessageSpecifyer::ADD_SIM:  // client called addSimulation(string str)
            {
                // newSim name is the str passed to the client function addSimulation
                std::tie(newSimName, newSimId) = noFS.getAddedSimulation();  // optional call
                // do for example some checks, here if the sim exists
                if (newSimName != "/home/of/very/funny/Sim.sim")
                {
                    noFS.deinitialize();
                    return 1;
                }
                // add sim to calculation
                sims.push_back(PseudoSim());
                // confirm the clients request, as arguments you have to pass the id received by the client and the available vars of the simulation [newSimName]
                noFS.confirmSimulationAdd(newSimId, sims.back().inVars, sims.back().outVars);  // NON OPTIONAL CALL!!!!!!!!!!!!
                break;
            }
            case NetOff::InitialClientMessageSpecifyer::INIT_SIM:  // client called initializeSimulation(id,inputs,outputs,initialReals,initialInts,initialBools)
            {
                // with getLastSimId it's possible to access the simId the received request is pointing to
                requestedSim = noFS.getLastSimId();  // optional call
                // the input variables wished by the client can be now accessed with getInputVariables(id)
                // in this case now means, after the server received an INIT_SIM, it's save to access all containers regarding the given id
                NetOff::VariableList inputsVars = noFS.getSelectedInputVariables(requestedSim);  // optional call

                //check if the variable names are known (for example):
                for (size_t i = 0; i < inputsVars.getReals().size(); ++i)
                    if (!(inputsVars.getReals()[i] == "dx" || inputsVars.getReals()[i] == "dy"))
                    {
                        // unkown variables so shut down:
                        noFS.deinitialize();
                        return 1;
                    }
                // check outputs wished by the client (for example)
                NetOff::VariableList outputVars = noFS.getSelectedOutputVariables(requestedSim);  // optional call

                //check if the variable names are known:
                for (size_t i = 0; i < outputVars.getReals().size(); ++i)
                    if (!(outputVars.getReals()[i] == "x" || outputVars.getReals()[i] == "y"))
                    {
                        // unkown variables so shut down:
                        noFS.deinitialize();
                        return 1;
                    }

                NetOff::ValueContainer & inputVals = noFS.getInputValueContainer(requestedSim);  // optional call
                sims[requestedSim].init(inputVals.getRealValues());

                // everything is ok with the variable call confirmSimulationInit
                NetOff::ValueContainer & outputVals = noFS.getOutputValueContainer(requestedSim);  // optional call
                outputVals.setRealValues(sims[requestedSim].states);  // optional call
                noFS.confirmSimulationInit(requestedSim, outputVals);  // NON OPTIONAL CALL!!!!!!!!!!!!
                break;
            }
            case NetOff::InitialClientMessageSpecifyer::START:
            {
                // notify client about a proper start up:
                noFS.confirmStart();  // NON OPTIONAL CALL!!!!!!!!!!!!
                run = false;
                break;
            }
            case NetOff::InitialClientMessageSpecifyer::GET_FILE:
            {
                // send file to server:
                std::string supposedFile = noFS.getSimulationFileName();
                // check if file is there:
                std::ifstream file;
                file.open(supposedFile.c_str(), std::ios::in);
                if (!file.good())
                {
                    //file not there, shut down
                    file.close();
                    noFS.deinitialize();
                    return 1;
                }
                file.close();

                noFS.confirmSimulationFile(noFS.getLastSimId(), supposedFile);  // NON OPTIONAL CALL!!!!!!!!!!!!
                break;
            }
            case NetOff::InitialClientMessageSpecifyer::CLIENT_INIT_ABORT:
            {
                // shut down Server
                noFS.deinitialize();  // NON OPTIONAL CALL!!!!!!!!!!!!
                return 0;
            }
            default:
                throw std::runtime_error("Received unknown request.");
        }
    }

// let the simulation run begin:

    run = true;
    while (run)
    {
        NetOff::ClientMessageSpecifyer spec = noFS.getClientRequest();
        switch (spec)
        {
            case NetOff::ClientMessageSpecifyer::INPUTS:
            {
                requestedSim = noFS.getLastSimId();  // optional call
                // when receiving inputs return the requested time step:
                NetOff::ValueContainer & inputs = noFS.recvInputValues(requestedSim);            // optional call
                //std::cout << sims[requestedSim].currentTime << "  Inputs: " << inputs << "\n";
                // calc a new step: (for examplewith  the PseudoSim)
                sims[requestedSim].solve(inputs.getRealValues(), noFS.getLastReceivedTime(requestedSim));
                //set the states in the container:
                NetOff::ValueContainer & outputs = noFS.getOutputValueContainer(requestedSim);            // optional call
                outputs.setRealValues(sims[requestedSim].states);
                //std::cout << sims[requestedSim].currentTime << " Outputs: " << outputs << "\n";
                //send the new values to the client:
                noFS.sendOutputValues(requestedSim, sims[requestedSim].currentTime, outputs);            // NON OPTIONAL CALL!!!!!!!!!!!!
                break;
            }
            case NetOff::ClientMessageSpecifyer::PAUSE:
            {
                // pause a bit (1sec):
                noFS.confirmPause();            // NON OPTIONAL CALL!!!!!!!!!!!!
                SDL_Delay(1000);
                break;
            }
            case NetOff::ClientMessageSpecifyer::UNPAUSE:
            {
                noFS.confirmUnpause();  // NON OPTIONAL CALL!!!!!!!!!!!!
                break;
            }
            case NetOff::ClientMessageSpecifyer::RESET:
            {
                // go back to initial phase: (here not supported, server is just shutting down)
                noFS.confirmReset();  // NON OPTIONAL CALL!!!!!!!!!!!!
                // shut down Server because this is not supported in this example
                noFS.deinitialize();            // optional call
                return 1;
                break;
            }
            case NetOff::ClientMessageSpecifyer::CLIENT_ABORT:
            {
                // shut down Server
                noFS.deinitialize();            // NON OPTIONAL CALL!!!!!!!!!!!!
                run = false;
                break;
            }
            default:
                throw std::runtime_error("Received unknown request.");
        }
    }

    return 0;
}

#endif /* TEST_SIMPLESIMSERVER_HPP_ */
