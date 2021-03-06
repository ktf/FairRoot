#Examples

##Tutorial1:
How to implement a simple detector using the old ASCII interface to describe the geometry (see Detector Geometry and Media for more details about this geometry interface)
See also Adding new detector for a detailed description of the detector class implementation, MC point class and the CMakeLists.txt

To run this tutorial:

After building FairRoot successfully go to the build directory and call the configuration script (config.sh or config.csh according to our shell)
cd to the Tutorial1/macros directory
run the following command: root run_tutorial1.C (you can also load the macro into root and then run it)
This will start a simulation session with Geant3 and create 10 events generated by a box generator, the input in the box generator can be chosen by the user, by default in this macro we use "pions" (see macro lines 20-22) for details.
To choose Geant4 you just have to change TGeant3 to TGeant4 in line 8 of the macro, or of you are loading the macro to root before exciting it you just give TGeant4 as second argument to the run_tutorial1 call, i.e:

```bash
root>.L run_tutorial1.C
root>run_tutorial(10, "TGeant4")
```

##Tutorial2

This example show you how to:

Simulate a simple detector (run_tutorial2.C)
Read the simulated data, and the digitization parameters from "parameters/tutdet.digi.par" and create digitized data (create_digis.C)
Read the digitized data for further reconstruction (read_digis.C)

##Tutorial3
This example demonstrate

detector simulation
event by event digitization
time based digitization
Message Queue based reconstruction: The Message Queue based reconstruction demonstrates the usage of FairMQ.
Any number of "sampler" processes (FairMQSampler, FairMQSamplerTask, TestDetectorDigiLoader) send the data generated in the digitization step to a number of "processor" processes which do the reconstruction with the data (FairMQProcessor, FairMQProcessorTask, FairTestDetectorMQRecoTask). After the reconstruction task, the processes send the output data to a number of "sink" processes, which write the data to disk (FairMQFileSink).
All the communication between these processes is done via FairMQ and can be configured to run over network (tcp) or on a local machine with inter-process communication (ipc).
Currently, the number of processes and their parameters are configured with bash scripts/command line parameters. The scripts are located in the macro directory, together with README.md for more details on how to configure them.
Event display
##Tutorial4
This example demonstrate:

using ROOT geometry as input for detector description, i.e: the geometry is created and saved to a ROOT file from a macro ( Create_Tutorial4_Geometry.C). The ROOT file is read by the detector classes during simulation
##Tutorial5
Shows how to use FairDB interface to write and read parameter to a database system.
Two type of parameter classes are implemented with

simple data members ( FairDbTutPar)
complex data members i.e ROOT object ( FairDbTutParBin)
In both cases, corresponding macro to read and write as well as a script.
To setup the database to be used (dbconfig.sh) are available from the macro directory.
rutherford
simple simulation of the Rutherford experiment, with event display.

##Tutorial6

#### Initialisation of parameter within MQ Tasks ####

* Introduction

This example demonstrates the usage of the runtimeDb services ( including comunication to SQL databases via FairDB ) 
in order to initialise parameter at a certain time within FairMQ Tasks. 
Once a parameter has been initialised, it is cached together with its interval of validity. 
Further calls to an update will only trigger an initialistation if the update time falls outside of the parameter 
interval of validity. 
In all other cases, a pointer to the valid parameter will be forwarded to the MQ process.   


* Getting started

In order to run the example you should first create a dummy simulation root file and convert it to
a dummy digits root file. This fisrt operation is done using the following predefined macros

simulation:
```bash
root>.L run_sim.C
root>run_sim(10000, "TGeant3")
```
digitisation:
```bash
root>.L run_digi.C
root>run_digi("TGeant3")
```
this should create the following root file in the subdirectory data. 


```bash
-rw-r--r--  1 denis  staff   9.2K Aug 11 08:36 FairRunInfo_testdigi_TGeant3.root
-rw-r--r--  1 denis  staff    11K Aug 11 08:36 FairRunInfo_testrun_TGeant3.root
-rw-r--r--  1 denis  staff   9.6K Aug 11 08:36 geofile_full.root
-rw-r--r--  1 denis  staff   277K Aug 11 08:36 testdigi_TGeant3.root
-rw-r--r--  1 denis  staff   7.9K Aug 11 08:36 testdigi_TGeant3.root.out.root
-rw-r--r--  1 denis  staff    37K Aug 11 08:36 testparams_TGeant3.root
-rw-r--r--  1 denis  staff    85M Aug 11 08:36 testrun_TGeant3.root
```


* Launch the FairDbMQ dispatcher

With the FairMQDB the FairMQ Tasks do not directly communicate to the backend parameter store but
initialisation messages are routed through a continuously running process i.e the FairDbMQ dispatcher. 
The user should launch the daemon once i.e
 
```bash
<my_fairroot_path>/build/bin/db_dispatcher &
```

The FairDbMQ dispatcher is then continuously listening to connection comming from 

 * FairMQ Tasks requiring an initialisation
 * Assigned FairDBMQWorker process to handle this request 

Once the FairDbMQDispatcher process is run, you should see  the following  prompt

```bash
-I- FairDbMQDispatcher:: Queueing Daemon Started ...
```
meaning that process initialised itself properly and is running on the background.



* Launch the Assigned FairDbMQWorker

In order to handle an initialisation request, the user should launch at least one FairDbMQ worker
process which as the FairDbMQDispatcher is daemonized.

```bash
<my_fairroot_path>/build/bin/db_tut6worker &
```

Once the FairDbMQworker process is run, you should see  the following  prompt

```bash
[12:07:03][DEBUG] -I- FairDbMQWorker::Run() Set Io input: 

[12:07:03][DEBUG] -I- FairDbMQWorker:: input IO: Text File: /Users/denis/fairroot/fairbase/example/tutorial5/macros/ascii-example.par

[12:07:03][DEBUG] -I- FairDbMQWorker::Run() Set Io Output: 

-I- FairDbMQTutWorker:: FairDbTutPar is created
-I-FairDbTutContFact::createContainer TUTParDefault
-I- FAIRDbConnectionPool  fGlobalSeqNoDbNo  0
-I-  FairDbMQWorker::Run() RTDB initialised ... ready to work ...: 
-I-  FairDbMQWorker Local cache container 
Key: TUTParDefault
Values
OBJ: FairDbTutPar	TUTParDefault	Default tutorial parameters
```
Meaning that the assigned  FairDbMQWorker  is properly initialised and is waiting to handle initialisation requests for the
container stored in its internal cache, namely in this case the FairDbTutPar parameter object.

* SQL Database settings

Furthermore, in this example, the FairDbMQWorker will use an SQL server to fetch the updated parameters values if needed. 
In FairDB services are used within the assigned worker, the user, before launching it, needs to define  the connection to a Database system (MySQL, PostGreSQL or SQlite )  
as it required by the FairDb Library i.e 

As an example, such  a definition  for a local MySQL server is done exporting the following environment varaiables: 

```bash
export FAIRDB_TSQL_URL="mysql://localhost/r3b"
export FAIRDB_TSQL_USER="scott"
export FAIRDB_TSQL_PSWD="tiger"
```

A more complete script is provided for setting up the main  supported RDBMS backends namely MySQL, PostGreSQL and SQLite:

```bash
<my_fairroot_path>/build/bin/dbconfig.sh
```


* Main MQ Tasks

After these steps are done, to run the example for parameter initialisation just 
run the dummy  MQtasks scripts ( sampler - processor ( with initialisation) - file_sink ) .

```bash
<my_fairroot_path>/build/bin/run_sps_init.sh
```


##Flp2epn
Simple example for Message based processing using FairRoot.

