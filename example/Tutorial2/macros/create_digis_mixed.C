void create_digis_mixed(){

    TStopwatch timer;
    timer.Start();

    gDebug=0;

    TString dir = getenv("VMCWORKDIR");
    TString tutdir = dir + "/Tutorial2";

    TString BGinFile = "./tutorial2_pions.mc_p2.000_t0_n130.bg.root";
    TString SGinFile = "./tutorial2_pions.mc_p2.000_t0_n10.sg.root";
    TString SG1inFile = "./tutorial2_pions.mc_p2.000_t0_n20.sg1.root";
    
    TString parFile = "./tutorial2_pions.params_p2.000_t0_n130.bg.root";
    TString outFile = "./digis.mix.mc.root";

    cout << "******************************" << endl;
    cout << "Background    File: " << BGinFile << endl;
    cout << "First  Signal File: " << SGinFile << endl;
    cout << "Second signal File: " << SG1inFile << endl;
    cout << "ParamFile: " << parFile << endl;
    cout << "OutFile: " << outFile << endl;
    cout << "******************************" << endl;

    FairRunAna *fRun= new FairRunAna();
    
    //** Set BG file */
    fRun->SetBackgroundFile(BGinFile);
    //** Set first signal file */
    fRun->SetSignalFile(SGinFile,1);
    //** Set second signal file */
    fRun->SetSignalFile(SG1inFile,2);

    
    fRun->SetOutputFile(outFile);

    //----- Mix using entries  ----------------------------------------
    
    /** for each ~20 entries background 1 entry from signal chain  1 will be read  */
    //fRun->BGWindowWidthNo(20,1);
    /** for each ~30 entries background 1 entry from signal chain  2 will be read  */
    //fRun->BGWindowWidthNo(30,2);
    
    
    //----- Mix using time       ----------------------------------------
    
    /**Set the event mean time, event time will be a random number generated from (1/T)exp(-x/T) */
    fRun->SetEventMeanTime(10);
    
    /** each ~100 ns background 1 entry from signal chain  1 will be read  */
    fRun->BGWindowWidthTime(100,1);
    /** each ~60 ns background 1 entry from signal chain  2 will be read  */
    fRun->BGWindowWidthTime(60,2);
    


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* io1=new FairParRootFileIo();
    io1->open(parFile.Data(),"UPDATE");
 
    FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
    TString tutDetDigiFile = gSystem->Getenv("VMCWORKDIR");
    tutDetDigiFile += "/Tutorial2/parameters/tutdet.digi.par";
    parInput2->open(tutDetDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    rtdb->setSecondInput(parInput2);

    rtdb->print();

    //**  TUt Det Digi Producer **//

    FairTutorialDet2Digitizer *digi = new FairTutorialDet2Digitizer("tutdet","tut det task");

    // add the task
    fRun->AddTask( digi );
    
    fRun->Init();

    rtdb->getContainer("FairTutorialDet2DigiPar")->print();

    FairTutorialDet2DigiPar* DigiPar = (FairTutorialDet2DigiPar*) 
                                      rtdb->getContainer("FairTutorialDet2DigiPar");

    DigiPar->setChanged();
    DigiPar->setInputVersion(fRun->GetRunId(),1);
    rtdb->setOutput(io1);
    rtdb->saveOutput();
    rtdb->print();


    fRun->Run();

    // -----   Finish   -------------------------------------------------------

    cout << endl << endl;

    // Extract the maximal used memory an add is as Dart measurement
    // This line is filtered by CTest and the value send to CDash
    FairSystemInfo sysInfo;
    Float_t maxMemory=sysInfo.GetMaxMemory();
    cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
    cout << maxMemory;
    cout << "</DartMeasurement>" << endl;

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();

    Float_t cpuUsage=ctime/rtime;
    cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
    cout << cpuUsage;
    cout << "</DartMeasurement>" << endl;

    cout << endl << endl;
    cout << "Output file is "    << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime
         << "s" << endl << endl;
    cout << "Macro finished successfully." << endl;
}
