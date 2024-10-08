#include "TString.h"
#include "TObjArray.h"
#include "TGrid.h"
#include "AliLog.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisDataContainer.h"
#include "AliAnalysisTaskChargeV1.h"

AliAnalysisTaskChargeV1 *AddTaskChargeV1(
    bool doNUE = false,
    bool doNUA = false,
    TString period = "LHC18q",
    bool ZDCcali = false,
    bool doVZERO = true,
    bool bCorrectForBadChannel = false,
    bool bCorrSpecZDC = false,
    bool bTrigger = false)
{
  // get the manager via the static access member. since it's static, you don't need
  // to create an instance of the class here to call the function
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    return 0x0;
  }
  // get the input event handler, again via a static method.
  // this handler is part of the managing system and feeds events
  // to your task
  if (!mgr->GetInputEventHandler())
  {
    return 0x0;
  }
  // by default, a file is open for writing. here, we get the filename
  TString fileName = AliAnalysisManager::GetCommonFileName();
  fileName += ":MyTask"; // create a subfolder in the file
  // now we create an instance of your task
  AliAnalysisTaskChargeV1 *task = new AliAnalysisTaskChargeV1("MyTask"); //
  if (!task)
    return 0x0;
  if(bTrigger) task->SelectCollisionCandidates(AliVEvent::kINT7);
  task->SetNUEOn(doNUE);
  task->SetNUAOn(doNUA);
  task->SetZDCOn(ZDCcali);
  task->SetVZEROPlaneOn(doVZERO);

  task->SetTriggerOn(bTrigger);


  //=========================================================================
  // Read in Files
  TFile *fNUEFile = nullptr;
  TFile *fNUAFile = nullptr;
  TList *fListNUE = nullptr;
  TList *fListNUA = nullptr;
  TFile *fZDCCalibFile = nullptr;
  TList *fZDCCalibList = nullptr;
  TFile *fVZEROCalibFile = nullptr;
  TList* fVZEROCalibList = nullptr;

  if (!gGrid)
    TGrid::Connect("alien://");
  
  task->SetPeriod(period);
  std::cout << "================ fPeriod Set =================" << std::endl;

  if (doNUE)
  {
    if (period.EqualTo("LHC18q"))
    {
      fNUEFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/NUE_18.root", "READ");
      fListNUE = dynamic_cast<TList *>(fNUEFile->Get("fMcEffiHij"));
    }
    if (period.EqualTo("LHC18r"))
    {
      fNUEFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/NUE_18.root", "READ");
      fListNUE = dynamic_cast<TList *>(fNUEFile->Get("fMcEffiHij"));
    }
    if (period.EqualTo("LHC15o"))
    {
      fNUEFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/RefData/reflhc15o/efficiencyBothpol.root", "READ");
      fListNUE = dynamic_cast<TList *>(fNUEFile->Get("fMcEffiHij"));
    }
    if (fListNUE)
    {
      task->SetListForNUE(fListNUE);
      std::cout << "================  NUE List Set =================" << std::endl;
    }
    else
      std::cout << "!!!!!!!!!!!!!!!NUE List not Found!!!!!!!!!!!!!!!" << std::endl;
  }
  if (doNUA)
  {

    if (period.EqualTo("LHC18q"))
    {
      fNUAFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/reflhc18q/WgtsNUAChargeAndPion_LHC18qPass3.root", "READ");
      fListNUA = dynamic_cast<TList *>(fNUAFile->Get("fNUA_ChPosChNeg"));
    }
    if (period.EqualTo("LHC18r"))
    {
      fNUAFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/reflhc18r/WgtsNUAChargeAndPion_LHC18rPass3.root", "READ");
      fListNUA = dynamic_cast<TList *>(fNUAFile->Get("fNUA_ChPosChNeg"));
    }    
    if (period.EqualTo("LHC15o"))
    {
      fNUAFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/RefData/reflhc15o/wgtPion_NUAFB768DeftwPUcut_LHC15op2_24Aug2021.root", "READ");
      fListNUA = dynamic_cast<TList *>(fNUAFile->Get("15oListNUA"));
    }
    if (fListNUA)
    {
      task->SetListForNUA(fListNUA);
      std::cout << "================  NUA List Set =================" << std::endl;
    }
    else
      std::cout << "!!!!!!!!!!!!!!!NUA List not Found!!!!!!!!!!!!!!!" << std::endl;
  }

  if (ZDCcali)
  {
    if (period.EqualTo("LHC18q"))
    {
      fZDCCalibFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/ZDCCali/RecenteringResultFinal_2018q.root", "READ");
      fZDCCalibList = dynamic_cast<TList *>(fZDCCalibFile->Get("fOutputRecenter"));
    }
    if (period.EqualTo("LHC18r"))
    {
      fZDCCalibFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/refData/ZDCCali/RecenteringResultFinal_2018r.root", "READ");
      fZDCCalibList = dynamic_cast<TList *>(fZDCCalibFile->Get("fOutputRecenter"));
    }
    if (period.EqualTo("LHC15o"))
    {
      fZDCCalibFile = TFile::Open("alien:///alice/cern.ch/user/j/jmargutt/15oHI_EZDCcalib.root","READ");
      fZDCCalibList = (TList*)(fZDCCalibFile->FindObjectAny("EZNcalib"));
      if(bCorrectForBadChannel) 
      {
        TFile* ZDCBadTowerFile = TFile::Open("alien:///alice/cern.ch/user/j/jmargutt/ZDCCalibBadChannel.root","READ");
        TList* ZDCBadTowerList = (TList*)(ZDCBadTowerFile->FindObjectAny("resp"));
        if(ZDCBadTowerList) 
        {
          task->SetBadTowerCalibList(ZDCBadTowerList);
          cout << "================BadTowerCalibList Set================" << endl;
        } 
        else 
        {
          cout << "ERROR: BadTowerCalibList not found!" << endl;
          exit(1);
        }
        delete ZDCBadTowerFile;
      }
      if(bCorrSpecZDC) 
      {
        TString ZDCRecFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
        if(bCorrectForBadChannel) ZDCRecFileName += "15o_ZDCSpectraCorr_BadCh_3.root";
        TFile* ZDCRecFile = TFile::Open(ZDCRecFileName,"READ");
        if(!ZDCRecFile) 
        {
          cout << "ERROR: ZDC Spectra Calibration not found!" << endl;
          exit(1);
        }
        TList* ZDCRecList = (TList*)(ZDCRecFile->FindObjectAny("ZDCSpectraCorr"));
        if(ZDCRecList) 
        {
          task->SetZDCSpectraCorrList(ZDCRecList);
          cout << "================ZDC Spectra Calibration Set ================"<< endl;
        }
        else 
        {
        cout << "ERROR: ZDCSpectraCorrList not found!" << endl;
        exit(1);
        }
        delete ZDCRecFile;
      }
    }
    if (fZDCCalibList)
    {
      task->SetListForZDCCalib(fZDCCalibList);
      std::cout << "================  ZDC List Set =================" << std::endl;
    }
    else
      std::cout << "!!!!!!!!!!!!!!!ZDC List not Found!!!!!!!!!!!!!!!" << std::endl;
  }
  
  if (doVZERO)
  {
    if (period.EqualTo("LHC15o")) {
      fVZEROCalibFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/CalibFiles/LHC15o/VZEROCalibFile15o.root", "READ");
      fVZEROCalibList = dynamic_cast<TList*>(fVZEROCalibFile->Get("VZEROCalibList"));
    }
    if (period.EqualTo("LHC18q")) {
      fVZEROCalibFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/CalibFiles/LHC18q/calibq2V0C18qP3.root", "READ");
      fVZEROCalibList = dynamic_cast<TList*>(fVZEROCalibFile->Get("18qlistspPerc"));
    }
    if (period.EqualTo("LHC18r")) {
      fVZEROCalibFile = TFile::Open("alien:///alice/cern.ch/user/r/ratu/CalibFiles/LHC18r/calibq2V0C18rP3.root", "READ");
      fVZEROCalibList = dynamic_cast<TList*>(fVZEROCalibFile->Get("18rlistspPerc"));
    }
    if (fVZEROCalibList) {
      task->SetListForVZEROCalib(fVZEROCalibList);
      std::cout << "================  VZERO List Set =================" << std::endl;
    } else
      std::cout << "!!!!!!!!!!!!!!!VZERO List not Found!!!!!!!!!!!!!!!" << std::endl;
  }
  // add your task to the managercd
  mgr->AddTask(task);
  // your task needs input: here we connect the manager to your task
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  //  mgr->ConnectInput(task,0,mgr->GetCommonInputContainer());
  // same for the output
  AliAnalysisDataContainer *coutput_QA = mgr->CreateContainer("ListQA", TList::Class(),
                                                              AliAnalysisManager::kOutputContainer,
                                                              fileName.Data());
  AliAnalysisDataContainer *coutput_result = mgr->CreateContainer("ListResults", TList::Class(),
                                                                  AliAnalysisManager::kOutputContainer,
                                                                  fileName.Data());
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput_result);
  mgr->ConnectOutput(task, 2, coutput_QA);

  //  mgr->ConnectOutput(task,1,mgr->CreateContainer("MyOutputContainer", TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
  // in the end, this macro returns a pointer to your task. this will be convenient later on
  // when you will run your analysis in an analysis train on grid
  return task;
}

