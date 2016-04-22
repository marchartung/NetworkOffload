/*
 * NetOffBenchmark.cpp
 *
 *  Created on: 22.04.2016
 *      Author: hartung
 */

#include <SimulationClient.hpp>
#include <SimulationServer.hpp>
#include <chrono>

struct BenchSim {
	size_t _numStates;
	size_t _numInputs;
	std::shared_ptr<double> _states;
	double _currentTime;

	NetOff::VariableList _vars;

	BenchSim(const size_t & numStates) :
			_numStates(numStates), _numInputs(0), _states(
					std::shared_ptr<double>(new double[_numStates])), _currentTime(
					0) {
		for (size_t i = 0; i < _numStates; ++i) {
			_vars.addReal(std::string("s") + std::to_string(i));
		}
	}

	void init(const double * in) {
		if (_numInputs == 0)
			throw std::runtime_error("BenchSim: No inputs set.");
		std::copy(in, in + _numStates, _states.get());
	}

	const double * solve(const double * in, const double & time) {
		std::copy(in, in + _numStates, _states.get());
		_currentTime = time;
		return _states.get();
	}
};

int BenchServer(const int & port, const size_t & numStates) {
	NetOff::SimulationServer noFS(port);
	// open server
	if (!noFS.initializeConnection())
		throw std::runtime_error("Couldn't open server.");

	std::vector<BenchSim> fmus;

	// now wait for initialization requests of a client:
	bool run = true;
	NetOff::InitialClientMessageSpecifyer spec;
	int requestedFmu, newFmuId;
	std::string newFmuName;
	std::vector<std::string> realVars;
	while (run) {
		spec = noFS.getInitialClientRequest();
		switch (spec) {
		case NetOff::InitialClientMessageSpecifyer::ADD_SIM: {
			// to react on this message the server has to call getAddedFmu()
			std::tie(newFmuName, newFmuId) = noFS.getAddedSimulation();
			// check if the fmu exists
			if (newFmuName != "/home/of/very/funny/Fmu.fmu") {
				noFS.deinitialize();
				return 1;
			}
			// add fmu to calculation
			fmus.push_back(BenchSim(numStates));
			noFS.confirmSimulationAdd(newFmuId, fmus.back()._vars);
			break;
		}
		case NetOff::InitialClientMessageSpecifyer::INIT_SIM: {
			requestedFmu = noFS.getLastFmuId();
			NetOff::VariableList inputsVars = noFS.getInputVariables(
					requestedFmu);
			NetOff::VariableList outputVars = noFS.getOutputVariables(
					requestedFmu);

			// initialize simulation:
			fmus[requestedFmu]._numInputs = inputsVars.getReals().size();
			NetOff::ValueContainer & inputVals = noFS.getInputValueContainer(
					requestedFmu);
			fmus[requestedFmu].init(inputVals.getRealValues());

			// send outputs
			NetOff::ValueContainer & outputVals = noFS.getOutputValueContainer(
					requestedFmu);
			outputVals.setRealValues(fmus[requestedFmu]._states.get());
			noFS.confirmSimulationInit(requestedFmu, outputVals);
			break;
		}
		case NetOff::InitialClientMessageSpecifyer::START: {
			// notify client about a proper start up:
			noFS.confirmStart();
			run = false;
			break;
		}
		case NetOff::InitialClientMessageSpecifyer::CLIENT_INIT_ABORT: {
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
	while (run) {
		NetOff::ClientMessageSpecifyer spec = noFS.getClientRequest();
		switch (spec) {
		case NetOff::ClientMessageSpecifyer::INPUTS: {
			requestedFmu = noFS.getLastFmuId();
			// when receiving inputs return the requested time step:
			NetOff::ValueContainer & inputs = noFS.recvInputValues(
					requestedFmu);

			// calc a new step: (for examplewith  the BenchSim)
			fmus[requestedFmu].solve(inputs.getRealValues(),
					noFS.getLastReceivedTime(requestedFmu));
			//set the states in the container:
			NetOff::ValueContainer & outputs = noFS.getOutputValueContainer(
					requestedFmu);
			outputs.setRealValues(fmus[requestedFmu]._states.get());

			//send the new values to the client:
			noFS.sendOutputValues(requestedFmu, fmus[requestedFmu]._currentTime,
					outputs);
			break;
		}
		case NetOff::ClientMessageSpecifyer::PAUSE: {
			// pause a bit (1sec):
			noFS.confirmPause();
			SDL_Delay(1000);
			break;
		}
		case NetOff::ClientMessageSpecifyer::UNPAUSE: {
			noFS.confirmUnpause();
			break;
		}
		case NetOff::ClientMessageSpecifyer::RESET: {
			// go back to initial phase: (here not supported, server is just shutting down)
			noFS.confirmReset();
			// shut down Server
			noFS.deinitialize();
			return 1;
			break;
		}
		case NetOff::ClientMessageSpecifyer::CLIENT_ABORT: {
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

int BenchClient(const std::string & hostname, int port,
		const size_t & numInputs, const size_t & numRounds,
		bool bench = false) {
	size_t numStates = 0;
	std::chrono::high_resolution_clock::time_point t1 =
			std::chrono::high_resolution_clock::now();
	NetOff::SimulationClient noFC(hostname, port);
	// test if server can be reached
	if (!noFC.initializeConnection())
		throw std::runtime_error("Couldn't reach important server.");

	// send server data about the fmu u want to calculate and get a handle ID of the fmu
	int funFmu = noFC.addSimulation("/home/of/very/funny/Fmu.fmu");

	// checkout the variables of the fmu
	NetOff::VariableList all = noFC.getVariableNames(funFmu);
	numStates = all.getReals().size();
	// set the input variables to send (in this case just the first variable of all fmu variables)
	NetOff::VariableList inputVars;
	std::shared_ptr<double> inputsClient(new double[numInputs]);
	for (size_t i = 0; i < numInputs; ++i) {
		inputVars.addReal(all.getReals()[i]);
		inputsClient.get()[i] = i;
	}

	// set the output variables, which should be received (in this case all fmu variables)
	NetOff::VariableList outputVars;
	std::shared_ptr<double> outputsClient(new double[all.getReals().size()]);
	outputVars = all;

	noFC.initializeSimulation(funFmu, inputVars, outputVars, inputsClient.get(),
			nullptr, nullptr);
	NetOff::ValueContainer & data = noFC.getOutputValueContainer(funFmu);

	// start the server
	if (!noFC.start())
		throw std::runtime_error("Couldn't start important server.");

	std::chrono::high_resolution_clock::time_point t2 =
			std::chrono::high_resolution_clock::now();
	double t = 0;

	for (size_t i = 0; i < numRounds; ++i) {

		NetOff::ValueContainer & inputsServer = noFC.getInputValueContainer(
				funFmu);
		inputsServer.setRealValues(inputsClient.get());
		// send inputs
		noFC.sendInputValues(funFmu, t, inputsServer);

		// get outputs
		NetOff::ValueContainer & outputsServer = noFC.recvOutputValues(funFmu,
				t);
		std::copy(outputsServer.getRealValues(),
				outputsServer.getRealValues() + all.getReals().size(),
				outputsClient.get());

		t += 0.1;
	}

	std::chrono::high_resolution_clock::time_point t3 =
			std::chrono::high_resolution_clock::now();
	noFC.deinitialize();

	std::chrono::high_resolution_clock::time_point t4 =
			std::chrono::high_resolution_clock::now();

	if (!bench) {
		std::cout << "Connection to host: " << hostname << " on port " << port
				<< ":\n";
		std::cout << "Initialization: 	"
				<< std::chrono::duration<double>(t2 - t1).count() << "\n";
		std::cout << "Simulation: 		"
				<< std::chrono::duration<double>(t3 - t2).count() << "\n";
		std::cout << "Deinitialization: 	"
				<< std::chrono::duration<double>(t4 - t3).count() << "\n";
	} else
		std::cout << numStates << "," << numInputs << ","
				<< std::chrono::duration<double>(t2 - t1).count() << ","
				<< std::chrono::duration<double>(t3 - t2).count() / numRounds
				<< "," << std::chrono::duration<double>(t4 - t3).count()
				<< "\n";
	return 0;
}

int main(int argc, char * argv[]) {
	if (argc < 2) {
		std::cout
				<< "Usage 1: ./SimpleSim client [numInputs]  [numRounds] [port] [servername]\n";
		std::cout << "Usage 2: ./SimpleSim server [numStates] [port]\n";
		std::cout
				<< "If no servername is set, the program will start a server, otherwise a client.\n";
		std::cout
				<< "Usage 3: ./SimpleSim [client | server] bench will start a predefined scaling benchmark.\n";
		return 0;
	}
	const size_t roundsPerTest = 10;
	const size_t numRounds = 1000;
	const size_t statesStart = 16;
	const double percentInputs = 0.1;
	if (std::string(argv[1]) == std::string("client")) {
		if (std::string(argv[2]) == std::string("bench")) {
			size_t numRounds = 1000;
			std::cout << "scale states with _" << numRounds << " rounds:\n";
			size_t statesStart = 16;
			std::cout
					<< "numStates, numInputs, initTime, simTime/rounds, deinitTime\n";
			for (size_t i = 1; i <= roundsPerTest; ++i) {
				statesStart *= 2;
				BenchClient(argv[3], 3009, 10, 1000, true);
			}
			std::cout << "\n";

			std::cout << "scale states with _" << numRounds
					<< " rounds and 1000 states:\n";
			std::cout
					<< "numStates, numInputs, initTime, simTime/rounds, deinitTime\n";
			for (size_t i = 1; i <= roundsPerTest; ++i) {
				BenchClient(argv[3], 3009,
						std::min((unsigned) (1000 * i * percentInputs), 1000u),
						1000, true);
			}
			return 0;
		} else {
			size_t numInputs = std::stoi(argv[2]);
			size_t numRounds = std::stoi(argv[3]);
			int port = std::stoi(argv[4]);
			return BenchClient(argv[5], port, numInputs, numRounds);
		}
	} else if (std::string(argv[1]) == std::string("server")) {
		if (std::string(argv[2]) == std::string("bench")) {
			size_t stateIt = statesStart;
			for (size_t i = 1; i <= roundsPerTest; ++i) {
				stateIt *= 2u;
				BenchServer(3009,stateIt);
			}

			for (size_t i = 1; i <= roundsPerTest; ++i) {
				BenchServer(3009,1000);
			}
			return 0;
		} else {
			size_t numStates = std::stoi(argv[2]);
			int port = std::stoi(argv[3]);
			return BenchServer(port, numStates);
		}
	}

}
