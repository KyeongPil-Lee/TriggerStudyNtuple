#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLorentzVector.h>
#include <TStopwatch.h>
#include <TTimeStamp.h>
#include <TString.h>
#include <TLegend.h>
#include <THStack.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TAttMarker.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TApplication.h>
#include <vector>
#include <TMath.h>
#include <TSystem.h>

#include <Include/NtupleHandle.h>
#include <Include/Object.h>
#include <Include/PlotTools.h>

class TnPPair
{
public:
	KPMuon tag;
	KPMuon probe;
	Bool_t isValid;
	Bool_t isPassingProbe;
	Double_t mass;
	Int_t nVertices;

	TnPPair( KPMuon tagCand, KPMuon probeCand )
	{
		this->Init();
		this->tag = tagCand;
		this->probe = probeCand;
	}

	void Validation(NtupleHandle *ntuple)
	{
		if( this->IsTag(this->tag, ntuple) && this->IsProbe(this->probe, ntuple) && this->IsGoodPair(this->tag, this->probe) )
		{
			this->isValid = kTRUE;
			this->mass = (tag.LVec_P + probe.LVec_P).M();
			this->nVertices = ntuple->nVertices;

			if( this->IsPassingProbe(this->probe, ntuple) )
				this->isPassingProbe = kTRUE;
		}
	}

	Double_t Mass()
	{
		if( !this->isValid )
		{
			cout << "INVALID PAIR! DO NOT USE THIS TNP PAIR!!";
			return -999;
		}

		return this->mass;
	}

// protected:
	// -- user-defined tag condition -- //
	virtual Bool_t IsTag( KPMuon tagCand, NtupleHandle *ntuple )
	{
		Bool_t flag = kFALSE;

		cout << "default IsTag" << endl;

		return flag;
	}

	// -- user-defined probe condition -- //
	virtual Bool_t IsProbe( KPMuon probCand, NtupleHandle *ntuple )
	{
		Bool_t flag = kFALSE;

		cout << "default IsProbe" << endl;

		return flag;
	}

	virtual Bool_t IsGoodPair( KPMuon tagCand, KPMuon probeCand )
	{
		Bool_t flag = kFALSE;

		Double_t mass = (tagCand.LVec_P + probeCand.LVec_P).M();
		Double_t deltaR = tagCand.LVec_P.DeltaR(probeCand.LVec_P);

		if( mass > 49.9 && mass < 130.1 && deltaR > 0.3 )
			flag = kTRUE;

		return flag;
	}

	// -- user-defined passing probe condition -- //
	virtual Bool_t IsPassingProbe( KPMuon probe, NtupleHandle *ntuple )
	{
		Bool_t flag = kFALSE;
		return flag;
	}

	void Init()
	{
		this->mass = -999;
		this->isValid = kFALSE;
		this->isPassingProbe = kFALSE;
		this->nVertices = -999;
	}
};

class TnPHistProducer
{
public:
	Double_t pTCut;

	Int_t nPtBin;
	Int_t nHighPtBin;
	Int_t nEtaBin;
	Int_t nPhiBin;
	Int_t nVtxBin;

	Double_t *arr_PtBinEdge;
	Double_t *arr_HighPtBinEdge;
	Double_t *arr_EtaBinEdge;
	Double_t *arr_PhiBinEdge;
	Double_t *arr_VtxBinEdge;

	vector<TH1D*> vec_HistPtBinPass;
	vector<TH1D*> vec_HistHighPtBinPass;
	vector<TH1D*> vec_HistEtaBinPass;
	vector<TH1D*> vec_HistPhiBinPass;
	vector<TH1D*> vec_HistVtxBinPass;

	vector<TH1D*> vec_HistPtBinFail;
	vector<TH1D*> vec_HistHighPtBinFail;
	vector<TH1D*> vec_HistEtaBinFail;
	vector<TH1D*> vec_HistPhiBinFail;
	vector<TH1D*> vec_HistVtxBinFail;

	vector<TH1D*> vec_TemplateHist;

	TnPHistProducer()
	{
		this->Init();
	}

	~TnPHistProducer()
	{
		for(const auto& hist : vec_HistPtBinPass )
			delete hist;

		for(const auto& hist : vec_HistPtBinFail )
			delete hist;

		for(const auto& hist : vec_HistHighPtBinPass )
			delete hist;

		for(const auto& hist : vec_HistHighPtBinFail )
			delete hist;

		for(const auto& hist : vec_HistEtaBinPass )
			delete hist;

		for(const auto& hist : vec_HistEtaBinFail )
			delete hist;

		for(const auto& hist : vec_HistPhiBinPass )
			delete hist;

		for(const auto& hist : vec_HistPhiBinFail )
			delete hist;

		for(const auto& hist : vec_HistVtxBinPass )
			delete hist;

		for(const auto& hist : vec_HistVtxBinFail )
			delete hist;


		for(const auto& templateHist : vec_TemplateHist )
			delete templateHist;
	}

	void Save( TFile *f_output )
	{
		f_output->cd();
		for(const auto& hist : vec_HistPtBinPass )
			hist->Write();

		for(const auto& hist : vec_HistPtBinFail )
			hist->Write();

		for(const auto& hist : vec_HistHighPtBinPass )
			hist->Write();

		for(const auto& hist : vec_HistHighPtBinFail )
			hist->Write();

		for(const auto& hist : vec_HistEtaBinPass )
			hist->Write();

		for(const auto& hist : vec_HistEtaBinFail )
			hist->Write();

		for(const auto& hist : vec_HistPhiBinPass )
			hist->Write();

		for(const auto& hist : vec_HistPhiBinFail )
			hist->Write();

		for(const auto& hist : vec_HistVtxBinPass )
			hist->Write();

		for(const auto& hist : vec_HistVtxBinFail )
			hist->Write();


		for(const auto& templateHist : vec_TemplateHist )
			templateHist->Write();
	}

	void SetPtThreshold( Double_t _value )
	{
		this->pTCut = _value;
	}

	void Fill( TnPPair *pair, Double_t weight = 1.0 )
	{
		if( pair->isPassingProbe )
		{
			this->FindBinAndFill( pair, weight, pair->probe.Pt, this->nPtBin, this->arr_PtBinEdge, this->vec_HistPtBinPass, kFALSE );
			this->FindBinAndFill( pair, weight, pair->probe.Pt, this->nHighPtBin, this->arr_HighPtBinEdge, this->vec_HistHighPtBinPass, kFALSE );
			this->FindBinAndFill( pair, weight, pair->probe.Eta, this->nEtaBin, this->arr_EtaBinEdge, this->vec_HistEtaBinPass );
			this->FindBinAndFill( pair, weight, pair->probe.Phi, this->nPhiBin, this->arr_PhiBinEdge, this->vec_HistPhiBinPass );
			this->FindBinAndFill( pair, weight, pair->nVertices, this->nVtxBin, this->arr_VtxBinEdge, this->vec_HistVtxBinPass );

		}
		else
		{
			this->FindBinAndFill( pair, weight, pair->probe.Pt, this->nPtBin, this->arr_PtBinEdge, this->vec_HistPtBinFail, kFALSE );
			this->FindBinAndFill( pair, weight, pair->probe.Pt, this->nHighPtBin, this->arr_HighPtBinEdge, this->vec_HistHighPtBinFail, kFALSE );
			this->FindBinAndFill( pair, weight, pair->probe.Eta, this->nEtaBin, this->arr_EtaBinEdge, this->vec_HistEtaBinFail );
			this->FindBinAndFill( pair, weight, pair->probe.Phi, this->nPhiBin, this->arr_PhiBinEdge, this->vec_HistPhiBinFail );
			this->FindBinAndFill( pair, weight, pair->nVertices, this->nVtxBin, this->arr_VtxBinEdge, this->vec_HistVtxBinFail );
		}
	}
private:
	void Init()
	{
		this->pTCut = 0;

		vector<Double_t> vec_PtBinEdge = {0, 10, 15, 20, 22, 24, 26, 27, 28, 30, 40, 50, 60, 80, 120, 200, 500};
		vector<Double_t> vec_HighPtBinEdge = {0, 10, 15, 20, 25, 30, 40, 45, 48, 50, 52, 55, 60, 80, 120, 200, 500};
		vector<Double_t> vec_EtaBinEdge = {-2.4, -2.1, -1.6, -1.2, -0.9, -0.3, -0.2, 0, 0.2, 0.3, 0.9, 1.2, 1.6, 2.1, 2.4};
		Double_t degree15 = 3.141592 / 12.0;
		vector<Double_t> vec_PhiBinEdge =
		{
			(-1)*degree15*12, (-1)*degree15*11, (-1)*degree15*9, (-1)*degree15*7, (-1)*degree15*5, 
			(-1)*degree15*3, (-1)*degree15*1, degree15*1, degree15*3, degree15*5, 
			degree15*7, degree15*9, degree15*11, degree15*12
		};
		vector<Double_t> vec_VtxBinEdge = 
		{
			2.5, 4.5, 6.5, 8.5, 10.5, 12.5, 14.5, 16.5, 18.5, 20.5, 
			22.5, 24.5, 26.5, 28.5, 30.5, 32.5, 34.5, 36.5, 38.5, 40.5,
			42.5, 44.5, 46.5, 48.5, 50.5
		};

		this->CopyVectorToArray( vec_PtBinEdge, this->nPtBin, this->arr_PtBinEdge );
		this->CopyVectorToArray( vec_HighPtBinEdge, this->nHighPtBin, this->arr_HighPtBinEdge );
		this->CopyVectorToArray( vec_EtaBinEdge, this->nEtaBin, this->arr_EtaBinEdge );
		this->CopyVectorToArray( vec_PhiBinEdge, this->nPhiBin, this->arr_PhiBinEdge );
		this->CopyVectorToArray( vec_VtxBinEdge, this->nVtxBin, this->arr_VtxBinEdge );

		this->MakeHistVector( "Pt", this->vec_HistPtBinPass, this->vec_HistPtBinFail, this->nPtBin );
		this->MakeHistVector( "HighPt", this->vec_HistHighPtBinPass, this->vec_HistHighPtBinFail, this->nHighPtBin );
		this->MakeHistVector( "Eta", this->vec_HistEtaBinPass, this->vec_HistEtaBinFail, this->nEtaBin );
		this->MakeHistVector( "Phi", this->vec_HistPhiBinPass, this->vec_HistPhiBinFail, this->nPhiBin );
		this->MakeHistVector( "Vtx", this->vec_HistVtxBinPass, this->vec_HistVtxBinFail, this->nVtxBin );


		// -- empty histogram: just for storing binnings used in making TnP mass histograms
		// -- these "template" histograms can be used by cloning them when the efficiencies are calculated using the other code -- // 
		TH1D* hEffTemplatePt = new TH1D("hEffTemplatePt", "", this->nPtBin, this->arr_PtBinEdge);
		vec_TemplateHist.push_back( hEffTemplatePt );

		TH1D* hEffTemplateHighPt = new TH1D("hEffTemplateHighPt", "", this->nHighPtBin, this->arr_HighPtBinEdge);
		vec_TemplateHist.push_back( hEffTemplateHighPt );

		TH1D* hEffTemplateEta = new TH1D("hEffTemplateEta", "", this->nEtaBin, this->arr_EtaBinEdge);
		vec_TemplateHist.push_back( hEffTemplateEta );

		TH1D* hEffTemplatePhi = new TH1D("hEffTemplatePhi", "", this->nPhiBin, this->arr_PhiBinEdge);
		vec_TemplateHist.push_back( hEffTemplatePhi );

		TH1D* hEffTemplateVtx = new TH1D("hEffTemplateVtx", "", this->nVtxBin, this->arr_VtxBinEdge);
		vec_TemplateHist.push_back( hEffTemplateVtx );
	}

	void CopyVectorToArray( vector<Double_t>& vec, Int_t &nBin, Double_t*& arr )
	{
		nBin = (Int_t)vec.size()-1; // -- # bins = # bin edges-1 -- //
		arr = new Double_t[nBin+1]; // -- dynamic allocation -- //
		for(Int_t i=0; i<nBin+1; i++)
			arr[i] = vec[i];
	}

	void MakeHistVector( TString Type, vector<TH1D*>& vec_HistPass, vector<TH1D*>& vec_HistFail, Int_t nBin )
	{
		Double_t minMass = 50;
		Double_t maxMass = 130;
		Int_t nMassBin = (Int_t)(maxMass - minMass); // -- bin size = 1 GeV -- //

		TString histNameBase = TString::Format("h%s", Type.Data());

		for(Int_t i=0; i<nBin; i++)
		{
			TString binInfo = TString::Format("%02dbin", i);

			TString histNamePass = histNameBase + "Pass_" + binInfo;
			TH1D* hTempPass = new TH1D(histNamePass, "", nMassBin, minMass, maxMass );
			vec_HistPass.push_back( hTempPass );

			TString histNameFail = histNameBase + "Fail_" + binInfo;
			TH1D* hTempFail = new TH1D(histNameFail, "", nMassBin, minMass, maxMass );
			vec_HistFail.push_back( hTempFail );
		}
	}

	void FindBinAndFill( TnPPair *pair, Double_t weight, Double_t binValue, Int_t nBin, Double_t *arr_BinEdge, vector<TH1D*> vec_Hist, Bool_t hasPtCut = kTRUE )
	{
		if( hasPtCut )
		{
			if( pair->probe.Pt < this->pTCut )
				return;
		}
		
		Double_t pairMass = pair->Mass();
		for(Int_t i=0; i<nBin; i++)
		{
			if( binValue > arr_BinEdge[i] && binValue < arr_BinEdge[i+1] )
			{				
				vec_Hist[i]->Fill( pairMass, weight );
				break;
			}
		}
	}
};

class TnPEffTool
{
public:
	TString fileName;
	Double_t massMin;
	Double_t massMax;

	TnPEffTool()
	{
		this->Init();
	}

	TnPEffTool( TString _fileName ): TnPEffTool()
	{
		this->fileName = _fileName;
	}

	void SetCutAndCountMassRange(Double_t min = 81, Double_t max = 101 )
	{
		this->massMin = min;
		this->massMax = max;
	}

	TH1D* CalcTnPEff_CutAndCount( TString varName )
	{
		TH1D* hEff = Get_Hist( this->fileName, "hEffTemplate"+varName, "hEff"+varName);
		Int_t nBin = hEff->GetNbinsX();

		vector< TH1D* > vec_HistPass;
		vector< TH1D* > vec_HistFail;

		TString histNameBase = TString::Format("h%s", varName.Data());
		for(Int_t i=0; i<nBin; i++)
		{
			TString binInfo = TString::Format("%02dbin", i);

			TString histNamePass = histNameBase + "Pass_" + binInfo;
			TH1D* hTempPass = Get_Hist( this->fileName, histNamePass );
			vec_HistPass.push_back( hTempPass );

			TString histNameFail = histNameBase + "Fail_" + binInfo;
			TH1D* hTempFail = Get_Hist( this->fileName, histNameFail );
			vec_HistFail.push_back( hTempFail );
		}

		Double_t nEventPassAllBin = 0;
		Double_t nEventTotalAllBin = 0;
		for(Int_t i=0; i<nBin; i++)
		{
			Int_t i_bin = i+1;

			Double_t nEventPass = this->CountEvent( vec_HistPass[i] );
			Double_t nEventFail = this->CountEvent( vec_HistFail[i] );

			Double_t nEventTotal = nEventPass + nEventFail;

			nEventPassAllBin += nEventPass;
			nEventTotalAllBin += nEventTotal;

			Double_t Eff = 0;
			Double_t AbsUncEff = 0;
			if( nEventTotal == 0 || nEventPass == 0 )
			{
				Eff = 0;
				AbsUncEff = 0;
			}
			else
			{
				Eff = nEventPass / nEventTotal;
				Double_t RelUncEff = this->CalcRelEffError( nEventPass, nEventTotal );
				AbsUncEff = Eff * RelUncEff;
			}

			hEff->SetBinContent(i_bin, Eff);
			hEff->SetBinError(i_bin, AbsUncEff);

			// printf("[%02d bin: from %.1lf to %.1lf] (Eff, Eff_Err, nEventPass, nEventTotal) = (%.3lf, %.3lf, %.1lf, %.1lf)\n", 
			// 	i_bin, hEff->GetBinLowEdge(i_bin), hEff->GetBinLowEdge(i_bin+1), Eff, AbsUncEff, nEventPass, nEventTotal);
		}

		Double_t effSingleValue = nEventPassAllBin / nEventTotalAllBin;
		printf("[Variable = %s] Single efficiency value = %.4lf\n", varName.Data(), effSingleValue );

		return hEff;
	}

private:
	void Init()
	{
		this->fileName = "";
		this->massMin = 81;
		this->massMax = 101;
	}

	Double_t CountEvent( TH1D* h )
	{
		Double_t nTotEvent = 0;

		Int_t nBin = h->GetNbinsX();
		for(Int_t i=0; i<nBin; i++)
		{
			Int_t i_bin = i+1;
			Double_t BinCenter = h->GetBinCenter(i_bin);
			if( BinCenter > this->massMin && BinCenter < this->massMax )
			{
				Double_t nEvent = h->GetBinContent(i_bin);
				nTotEvent += nEvent;
			}
		}
		return nTotEvent;
	}

	Double_t CalcRelEffError( Double_t nEventPass, Double_t nEventTotal )
	{
		Double_t RelUnc_NUM = sqrt(nEventPass) / nEventPass;
		Double_t RelUnc_DEN = sqrt(nEventTotal) / nEventTotal;

		Double_t RelUnc_Eff = sqrt( RelUnc_NUM*RelUnc_NUM + RelUnc_DEN*RelUnc_DEN);
		return RelUnc_Eff;
	}
};
