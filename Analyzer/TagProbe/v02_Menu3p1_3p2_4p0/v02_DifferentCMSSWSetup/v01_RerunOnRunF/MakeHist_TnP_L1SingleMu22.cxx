#include <TagProbe/MakeHist_TnP_L1SingleMu22.h>

void Setup( HistProducer* producer, TString dataType );

void MakeHist_TnP_L1SingleMu22(TString dataType)
{
	HistProducer* producer = new HistProducer();
	Setup( producer, dataType );

	producer->Produce();
}

void Setup( HistProducer* producer, TString dataType )
{
	TString dataPath = gSystem->Getenv("KP_DATA_PATH");

	if( dataType == "Menu3p1" )
	{
		producer->SetOutputFileName("ROOTFile_MakeHist_TnP_L1SingleMu22_Menu3p1.root" );

		producer->AddDataPath( dataPath+"/92X/TSntuple_v20171214_Menu3p1_SingleMuon_Run2017Fv1_305832_to_305862_GoldenJSON/ntuple_*.root");
	}
	else if( dataType == "Menu3p2" )
	{
		producer->SetOutputFileName("ROOTFile_MakeHist_TnP_L1SingleMu22_Menu3p2.root" );

		producer->AddDataPath( dataPath+"/92X/TSntuple_v20171214_Menu3p2_SingleMuon_Run2017Fv1_305832_to_305862_GoldenJSON/ntuple_*.root");
	}
	else if( dataType == "Menu4p0" )
	{
		producer->SetOutputFileName("ROOTFile_MakeHist_TnP_L1SingleMu22_Menu4p0.root" );

		producer->AddDataPath( dataPath+"/92X/TSntuple_v20171214_Menu4p0_SingleMuon_Run2017Fv1_305832_to_305862_GoldenJSON/ntuple_*.root");
	}

}