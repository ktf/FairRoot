 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
# Create a library called "libTutorial7" 

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/fairmq
${CMAKE_SOURCE_DIR}/fairmq/devices
${CMAKE_SOURCE_DIR}/base/MQ
${CMAKE_SOURCE_DIR}/base/MQ/policies/Sampler
${CMAKE_SOURCE_DIR}/base/MQ/policies/Serialization
${CMAKE_SOURCE_DIR}/base/MQ/policies/Storage
${CMAKE_SOURCE_DIR}/base/MQ/baseMQtools
${CMAKE_SOURCE_DIR}/example/Tutorial3/data
${CMAKE_SOURCE_DIR}/example/Tutorial7
${CMAKE_SOURCE_DIR}/example/Tutorial7/data
${CMAKE_SOURCE_DIR}/example/Tutorial7/devices
${CMAKE_SOURCE_DIR}/example/Tutorial7/devices/policy
${CMAKE_SOURCE_DIR}/example/Tutorial7/devices/policy/serialization
${CMAKE_SOURCE_DIR}/example/Tutorial7/devices/policy/task
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${ZMQ_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


If (Boost_FOUND)
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7All.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7All.sh )
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7GenerateData.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7GenerateData.sh )
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7AllTuto3.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7AllTuto3.sh )
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7Sampler.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7Sampler.sh )
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7Processor.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7Processor.sh )
  configure_file( ${CMAKE_SOURCE_DIR}/example/Tutorial7/macro/startTuto7Sink.sh.in ${CMAKE_BINARY_DIR}/bin/startTuto7Sink.sh )

EndIf (Boost_FOUND)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
) 

link_directories(${LINK_DIRECTORIES})

Set(SRCS
  data/MyDigi.cxx
  data/MyHit.cxx
  data/MyPodData.cxx
#${CMAKE_SOURCE_DIR}/example/Tutorial3/data/FairTestDetectorDigi.cxx
#${CMAKE_SOURCE_DIR}/example/Tutorial3/data/FairTestDetectorHit.cxx
#${CMAKE_SOURCE_DIR}/example/Tutorial3/data/FairTestDetectorPayload.cxx
)


Set(LINKDEF Tutorial7LinkDef.h)
Set(LIBRARY_NAME Tutorial7)

If (Boost_FOUND)
  Set(DEPENDENCIES
    Base FairMQ BaseMQ 
    boost_thread boost_system boost_serialization boost_program_options 
    RooFitCore RooFit Foam MathMore FairTestDetector
    )
EndIf (Boost_FOUND)

GENERATE_LIBRARY()

#Add_Subdirectory(macro)


If (Boost_FOUND)
  Set(Exe_Names
    tuto7SamplerBoost
    tuto7ProcessorBoost
    tuto7FileSinkBoost

    tuto7SamplerBin
    tuto7ProcessorBin
    tuto7FileSinkBin

    tuto7SamplerRoot
    tuto7ProcessorRoot
    tuto7FileSinkRoot

    tuto7GenerateData

    tuto7FairTestDetectorSamplerBoost
    tuto7FairTestDetectorProcessorBoost
    tuto7FairTestDetectorFileSinkBoost
    tuto7FairTestDetectorSamplerBin
    tuto7FairTestDetectorProcessorBin
    tuto7FairTestDetectorFileSinkBin
  )



  set(Exe_Source
    
    run/runSamplerBoostT7.cxx
    run/runProcessorBoostT7.cxx
    run/runFileSinkBoostT7.cxx

    run/runSamplerBinT7.cxx
    run/runProcessorBinT7.cxx
    run/runFileSinkBinT7.cxx

    run/runSamplerRootT7.cxx
    run/runProcessorRootT7.cxx
    run/runFileSinkRootT7.cxx

    run/runGenerateData.cxx

    run/FairTestDetector/runSamplerBoostT7b.cxx
    run/FairTestDetector/runProcessorBoostT7b.cxx
    run/FairTestDetector/runFileSinkBoostT7b.cxx
    run/FairTestDetector/runSamplerBinT7b.cxx
    run/FairTestDetector/runProcessorBinT7b.cxx
    run/FairTestDetector/runFileSinkBinT7b.cxx
    )

############################################################
# Check if we run on Ubuntu version>=11 and if it is the case
# add --no-as-needed flag to the linker.
# it is needed on Ubuntu xx>=11 to load Ttree in executables
# see : https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14064
# Alternative is to instantiate a TApplication or load manually with gsystem in main file
set(UBUNTU_GREATERTHAN_v11_LINKER_FLAG )

if(NOT CMAKE_SYSTEM_NAME MATCHES Darwin)
    # use the LSB stuff if possible 
    EXECUTE_PROCESS(
       COMMAND cat /etc/lsb-release
       COMMAND grep DISTRIB_ID
       COMMAND awk -F= "{ print $2 }"
       COMMAND tr "\n" " "
       COMMAND sed "s/ //"
       OUTPUT_VARIABLE LSB_ID
       RESULT_VARIABLE LSB_ID_RESULT
    )
    EXECUTE_PROCESS(
       COMMAND cat /etc/lsb-release
       COMMAND grep DISTRIB_RELEASE
       COMMAND awk -F= "{ print $2 }"
       COMMAND tr "\n" " "
       COMMAND sed "s/ //"
       OUTPUT_VARIABLE LSB_VER
       RESULT_VARIABLE LSB_VER_RESULT
    )
    
    #message("LSB output: ${LSB_ID_RESULT}:${LSB_ID} ${LSB_VER_RESULT}:${LSB_VER}")
    if(NOT ${LSB_ID} STREQUAL "")
        # found some, use it :D
        set(INSTALLER_PLATFORM "${LSB_ID}-${LSB_VER}" CACHE PATH "Installer chosen platform")
        if(${LSB_ID} STREQUAL "Ubuntu" AND ${LSB_VER} VERSION_GREATER "10.0")
            #message(STATUS "Running on ${INSTALLER_PLATFORM} which is more recent than Ubuntu 11.0")
            #message(STATUS "--no-as-needed flag is added to the linker of Tutorial7 executables")
            set(UBUNTU_GREATERTHAN_v11_LINKER_FLAG "-Wl,--no-as-needed")
        endif()
    else(NOT ${LSB_ID} STREQUAL "")
       set(INSTALLER_PLATFORM "linux-generic" CACHE PATH "Installer chosen platform")
    endif(NOT ${LSB_ID} STREQUAL "")
endif(NOT CMAKE_SYSTEM_NAME MATCHES Darwin)
############################################################



  List(LENGTH Exe_Names _length)
  Math(EXPR _length ${_length}-1)

  ForEach(_file RANGE 0 ${_length})
    List(GET Exe_Names ${_file} _name)
    List(GET Exe_Source ${_file} _src)
    Set(EXE_NAME ${_name})
    Set(SRCS ${_src})
    Set(DEPENDENCIES Tutorial7 ${UBUNTU_GREATERTHAN_v11_LINKER_FLAG})
    GENERATE_EXECUTABLE()
  EndForEach(_file RANGE 0 ${_length})
EndIf (Boost_FOUND)
