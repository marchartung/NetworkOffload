/*
 * SimpleSimServer.hpp
 *
 *  Created on: 19.04.2016
 *      Author: hartung
 */

#ifndef TEST_SIMPLESIMSERVER_HPP_
#define TEST_SIMPLESIMSERVER_HPP_

#include <SimulationServer.hpp>

struct PseudoFmu
{
    double states[4] = { 0, 0, 0, 0 };
    double currentTime = 0;

    NetOff::VariableList vars = NetOff::VariableList( { "x", "y", "dx", "dy" }, { }, { });

    void init(const double * in)
    {
        for (size_t i = 0; i < 2; ++i)
            states[i+2] = in[i];
    }

    std::vector<double> solve(const double * in, const double & time)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            states[i] += in[i] * (time - currentTime);
            states[i+2] = in[i];
        }

        currentTime = time;
        return {states[0],states[1],states[2],states[3]};
    }
};

int serverFunc(int port)
{
    NetOff::SimulationServer noFS(port);
       // open server
       if (!noFS.initializeConnection())
           throw std::runtime_error("Couldn't open server.");

       std::vector<PseudoFmu> fmus;

       // now wait for initialization requests of a client:
       bool run = true;
       NetOff::InitialClientMessageSpecifyer spec;
       int requestedFmu, newFmuId;
       std::string newFmuName;
       std::vector<std::string> realVars;
       while (run)
       {
           spec = noFS.getInitialClientRequest();
           switch (spec)
           {
               case NetOff::InitialClientMessageSpecifyer::ADD_SIM:
               {
                   // to react on this message the server has to call getAddedFmu()
                   std::tie(newFmuName, newFmuId) = noFS.getAddedSimulation();
                   // check if the fmu exists
                   if (newFmuName != "/home/of/very/funny/Fmu.fmu")
                   {
                       noFS.deinitialize();
                       return 1;
                   }
                   // add fmu to calculation
                   fmus.push_back(PseudoFmu());
                   noFS.confirmSimulationAdd(newFmuId,fmus.back().vars);
                   break;
               }
               case NetOff::InitialClientMessageSpecifyer::INIT_SIM:
               {
                   requestedFmu = noFS.getLastFmuId();
                   NetOff::VariableList inputsVars = noFS.getInputVariables(requestedFmu);

                   //check if the variable names are known:
                   for (size_t i = 0; i < inputsVars.getReals().size(); ++i)
                       if (!(inputsVars.getReals()[i] == "x" || inputsVars.getReals()[i] == "y" || inputsVars.getReals()[i] == "dx" || inputsVars.getReals()[i] == "dy"))
                       {
                           // unkown variables so shut down:
                           noFS.deinitialize();
                           return 1;
                       }

                   NetOff::VariableList outputVars = noFS.getOutputVariables(requestedFmu);

                   //check if the variable names are known:
                   for (size_t i = 0; i < outputVars.getReals().size(); ++i)
                       if (!(outputVars.getReals()[i] == "x" || outputVars.getReals()[i] == "y" || outputVars.getReals()[i] == "dx" || outputVars.getReals()[i] == "dy"))
                       {
                           // unkown variables so shut down:
                           noFS.deinitialize();
                           return 1;
                       }

                   NetOff::ValueContainer & inputVals = noFS.getInputValueContainer(requestedFmu);
                   fmus[requestedFmu].init(inputVals.getRealValues());

                   // everything is ok with the variable call confirmSimulationInit
                   NetOff::ValueContainer & outputVals = noFS.getOutputValueContainer(requestedFmu);
                   outputVals.setRealValues(fmus[requestedFmu].states);
                   noFS.confirmSimulationInit(requestedFmu, outputVals);
                   break;
               }
               case NetOff::InitialClientMessageSpecifyer::START:
               {
                   // notify client about a proper start up:
                   noFS.confirmStart();
                   run = false;
                   break;
               }
               case NetOff::InitialClientMessageSpecifyer::CLIENT_INIT_ABORT:
               {
                   // shut down Server
                   noFS.deinitialize();
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
                   requestedFmu = noFS.getLastFmuId();
                   // when receiving inputs return the requested time step:
                   NetOff::ValueContainer & inputs = noFS.recvInputValues(requestedFmu);
                   std::cout << fmus[requestedFmu].currentTime << "  Inputs: " << inputs << "\n";
                   // calc a new step: (for examplewith  the PseudoFmu)
                   fmus[requestedFmu].solve(inputs.getRealValues(), noFS.getLastReceivedTime(requestedFmu));
                   //set the states in the container:
                   NetOff::ValueContainer & outputs = noFS.getOutputValueContainer(requestedFmu);
                   outputs.setRealValues(fmus[requestedFmu].states);
                   std::cout << fmus[requestedFmu].currentTime << " Outputs: " << outputs << "\n";
                   //send the new values to the client:
                   noFS.sendOutputValues(requestedFmu,fmus[requestedFmu].currentTime, outputs);
                   break;
               }
               case NetOff::ClientMessageSpecifyer::PAUSE:
               {
                   // pause a bit (1sec):
                   noFS.confirmPause();
                   SDL_Delay(1000);
                   break;
               }
               case NetOff::ClientMessageSpecifyer::UNPAUSE:
               {
                   noFS.confirmUnpause();
                   break;
               }
               case NetOff::ClientMessageSpecifyer::RESET:
               {
                   // go back to initial phase: (here not supported, server is just shutting down)
                   noFS.confirmReset();
                   // shut down Server
                   noFS.deinitialize();
                   return 1;
                   break;
               }
               case NetOff::ClientMessageSpecifyer::CLIENT_ABORT:
               {
                   // shut down Server
                   noFS.deinitialize();
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
