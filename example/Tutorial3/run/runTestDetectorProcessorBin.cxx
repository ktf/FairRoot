/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * runTestDetectorProcessorBin.cxx
 *
 * @since 2012-10-26
 * @author A. Rybalchenko, N. Winckler
 */

#include <iostream>
#include <csignal>

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQProcessor.h"

#ifdef NANOMSG
#include "nanomsg/FairMQTransportFactoryNN.h"
#else
#include "zeromq/FairMQTransportFactoryZMQ.h"
#endif

#include "FairTestDetectorMQRecoTask.h"

#include "FairTestDetectorHit.h"
#include "FairTestDetectorDigi.h"
#include "FairTestDetectorPayload.h"

using namespace std;

typedef FairTestDetectorDigi TDigi;           // class to serialize/deserialize
typedef FairTestDetectorHit THit;             // class to serialize/deserialize
typedef TestDetectorPayload::Digi TPayloadIn; // binary payload
typedef TestDetectorPayload::Hit TPayloadOut; // binary payload
typedef FairTestDetectorMQRecoTask<TDigi, THit, TPayloadIn, TPayloadOut> TProcessorTask;

FairMQProcessor processor;

static void s_signal_handler(int signal)
{
    LOG(INFO) << "Caught signal " << signal;

    processor.ChangeState(FairMQProcessor::END);

    LOG(INFO) << "Shutdown complete.";
    exit(1);
}

static void s_catch_signals(void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

typedef struct DeviceOptions
{
    DeviceOptions() :
        id(), ioThreads(0), processorTask(),
        inputSocketType(), inputBufSize(0), inputMethod(), inputAddress(),
        outputSocketType(), outputBufSize(0), outputMethod(), outputAddress() {}

    string id;
    int ioThreads;
    string processorTask;
    string inputSocketType;
    int inputBufSize;
    string inputMethod;
    string inputAddress;
    string outputSocketType;
    int outputBufSize;
    string outputMethod;
    string outputAddress;
} DeviceOptions_t;

inline bool parse_cmd_line(int _argc, char* _argv[], DeviceOptions* _options)
{
    if (_options == NULL)
        throw std::runtime_error("Internal error: options' container is empty.");

    namespace bpo = boost::program_options;
    bpo::options_description desc("Options");
    desc.add_options()
        ("id", bpo::value<string>()->required(), "Device ID")
        ("io-threads", bpo::value<int>()->default_value(1), "Number of I/O threads")
        ("processor-task", bpo::value<string>()->default_value("FairTestDetectorMQRecoTask"), "Name of the Processor Task")
        ("input-socket-type", bpo::value<string>()->required(), "Input socket type: sub/pull")
        ("input-buff-size", bpo::value<int>()->required(), "Input buffer size in number of messages (ZeroMQ)/bytes(nanomsg)")
        ("input-method", bpo::value<string>()->required(), "Input method: bind/connect")
        ("input-address", bpo::value<string>()->required(), "Input address, e.g.: \"tcp://localhost:5555\"")
        ("output-socket-type", bpo::value<string>()->required(), "Output socket type: pub/push")
        ("output-buff-size", bpo::value<int>()->required(), "Output buffer size in number of messages (ZeroMQ)/bytes(nanomsg)")
        ("output-method", bpo::value<string>()->required(), "Output method: bind/connect")
        ("output-address", bpo::value<string>()->required(), "Output address, e.g.: \"tcp://localhost:5555\"")
        ("help", "Print help messages");

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(_argc, _argv, desc), vm);

    if ( vm.count("help") )
    {
        LOG(INFO) << "FairMQ Test Detector Processor" << endl << desc;
        return false;
    }

    bpo::notify(vm);

    if ( vm.count("id") )
        _options->id = vm["id"].as<string>();

    if ( vm.count("io-threads") )
        _options->ioThreads = vm["io-threads"].as<int>();

    if ( vm.count("processor-task") )
        _options->processorTask = vm["processor-task"].as<string>();

    if ( vm.count("input-socket-type") )
        _options->inputSocketType = vm["input-socket-type"].as<string>();

    if ( vm.count("input-buff-size") )
        _options->inputBufSize = vm["input-buff-size"].as<int>();

    if ( vm.count("input-method") )
        _options->inputMethod = vm["input-method"].as<string>();

    if ( vm.count("input-address") )
        _options->inputAddress = vm["input-address"].as<string>();

    if ( vm.count("output-socket-type") )
        _options->outputSocketType = vm["output-socket-type"].as<string>();

    if ( vm.count("output-buff-size") )
        _options->outputBufSize = vm["output-buff-size"].as<int>();

    if ( vm.count("output-method") )
        _options->outputMethod = vm["output-method"].as<string>();

    if ( vm.count("output-address") )
        _options->outputAddress = vm["output-address"].as<string>();

    return true;
}

int main(int argc, char** argv)
{
    s_catch_signals();

    DeviceOptions_t options;
    try
    {
        if (!parse_cmd_line(argc, argv, &options))
            return 0;
    }
    catch (exception& e)
    {
        LOG(ERROR) << e.what();
        return 1;
    }

    LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
    FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
    FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

    processor.SetTransport(transportFactory);

    FairMQChannel inputChannel(options.inputSocketType, options.inputMethod, options.inputAddress);
    inputChannel.UpdateSndBufSize(options.inputBufSize);
    inputChannel.UpdateRcvBufSize(options.inputBufSize);
    inputChannel.UpdateRateLogging(1);

    processor.fChannels["data-in"].push_back(inputChannel);

    FairMQChannel outputChannel(options.outputSocketType, options.outputMethod, options.outputAddress);
    outputChannel.UpdateSndBufSize(options.outputBufSize);
    outputChannel.UpdateRcvBufSize(options.outputBufSize);
    outputChannel.UpdateRateLogging(1);

    processor.fChannels["data-out"].push_back(outputChannel);

    processor.SetProperty(FairMQProcessor::Id, options.id);
    processor.SetProperty(FairMQProcessor::NumIoThreads, options.ioThreads);

    if (strcmp(options.processorTask.c_str(), "FairTestDetectorMQRecoTask") == 0)
    {
        TProcessorTask* task = new TProcessorTask();
        processor.SetTask(task);
    }
    else
    {
        LOG(ERROR) << "task not supported.";
        exit(1);
    }

    processor.ChangeState(FairMQProcessor::INIT_DEVICE);
    processor.WaitForEndOfState(FairMQProcessor::INIT_DEVICE);

    processor.ChangeState(FairMQProcessor::INIT_TASK);
    processor.WaitForEndOfState(FairMQProcessor::INIT_TASK);

    processor.ChangeState(FairMQProcessor::RUN);
    processor.WaitForEndOfState(FairMQProcessor::RUN);

    processor.ChangeState(FairMQProcessor::STOP);

    processor.ChangeState(FairMQProcessor::RESET_TASK);
    processor.WaitForEndOfState(FairMQProcessor::RESET_TASK);

    processor.ChangeState(FairMQProcessor::RESET_DEVICE);
    processor.WaitForEndOfState(FairMQProcessor::RESET_DEVICE);

    processor.ChangeState(FairMQProcessor::END);

    return 0;
}
