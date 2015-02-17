// AudioProcessDoc.cpp : implementation of the CAudioProcessDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AudioProcess.h"
#endif

#include "AudioProcessDoc.h"
#include "ProcessDlg.h"

#include <vector>

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAudioProcessDoc::OnProcessCopy()
{
	// Call to open the processing output
	if (!ProcessBegin()) return;

	short audio[2];

	for (int i = 0; i < SampleFrames(); i++)
	{
		ProcessReadFrame(audio);

		audio[0] = short(audio[0] * m_amplitude);
		audio[1] = short(audio[1] * m_amplitude);

		ProcessWriteFrame(audio);

		// The progress control
		if (!ProcessProgress(double(i) / SampleFrames())) break;
	}


	// Call to close the generator output
	ProcessEnd();
}


// CAudioProcessDoc

IMPLEMENT_DYNCREATE(CAudioProcessDoc, CDocument)

BEGIN_MESSAGE_MAP(CAudioProcessDoc, CDocument)
	ON_COMMAND(ID_PROCESS_FILEOUTPUT, &CAudioProcessDoc::OnProcessFileoutput)
	ON_COMMAND(ID_PROCESS_AUDIOOUTPUT, &CAudioProcessDoc::OnProcessAudiooutput)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_FILEOUTPUT, &CAudioProcessDoc::OnUpdateProcessFileoutput)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_AUDIOOUTPUT, &CAudioProcessDoc::OnUpdateProcessAudiooutput)
	ON_COMMAND(ID_PROCESS_COPY, &CAudioProcessDoc::OnProcessCopy)
	ON_COMMAND(ID_PROCESS_PARAMETERS, &CAudioProcessDoc::OnProcessParameters)
	ON_COMMAND(ID_QUESTIONB_PARTB, &CAudioProcessDoc::OnQuestionbPartb)
	ON_COMMAND(ID_QUESTIONB_PARTB32789, &CAudioProcessDoc::OnQuestionbPartb2)
	ON_COMMAND(ID_QUESTIONB_PARTB32790, &CAudioProcessDoc::OnQuestionbPartb3)
	ON_COMMAND(ID_QUESTIONB_PARTB32791, &CAudioProcessDoc::OnQuestionbPartb4)
	END_MESSAGE_MAP()


// CAudioProcessDoc construction/destruction

CAudioProcessDoc::CAudioProcessDoc()
{
	m_audiooutput = true;
	m_fileoutput = false;

	m_numChannels = 2;
	m_sampleRate = 44100.;
	m_numSampleFrames = 0;
	m_amplitude = 1.0;
}

CAudioProcessDoc::~CAudioProcessDoc()
{
}

BOOL CAudioProcessDoc::OnNewDocument()
{
	return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// CAudioProcessDoc serialization

void CAudioProcessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CAudioProcessDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CAudioProcessDoc::InitializeSearchContent()
{
	CString strSearchContent;
// Set search contents from document's data. 
// The content parts should be separated by ";"

// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CAudioProcessDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS



// CAudioProcessDoc diagnostics

#ifdef _DEBUG
void CAudioProcessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAudioProcessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



// CAudioProcessDoc commands


BOOL CAudioProcessDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName)) return FALSE;

	if (!m_wavein.Open(lpszPathName)) return FALSE;

	m_sampleRate = m_wavein.SampleRate();
	m_numChannels = m_wavein.NumChannels();
	m_numSampleFrames = m_wavein.NumSampleFrames();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// The following functions manage the audio processing process, 
// directing output to the waveform buffer, file, and/or audio 
// output.  
//
/////////////////////////////////////////////////////////////////////////////


//
// Name :        CAudioProcessDoc::ProcessBegin()
// Description : This function starts the audio processing process.
//               It opens the waveform storage, opens the file
//               if file output is requested, and opens the 
//               audio output if audio output is requested.
//               Be sure to call EndProcess() when done.
// Returns :     true if successful...
//

bool CAudioProcessDoc::ProcessBegin()
{
	m_wavein.Rewind();

	// 
	// Waveform storage
	//

	m_waveformBuffer.Start(NumChannels(), SampleRate());

	if (m_fileoutput)
	{
		if (!OpenProcessFile(m_waveout)) return false;
	}

	ProgressBegin(this);

	if (m_audiooutput)
	{
		m_soundstream.SetChannels(NumChannels());
		m_soundstream.SetSampleRate(int(SampleRate()));

		m_soundstream.Open(((CAudioProcessApp *)AfxGetApp())->DirSound());
	}

	return true;
}


//
// Name :        CAudioProcessDoc::ProcessReadFrame()
// Description : Read a frame of input from the current audio file.
//

void CAudioProcessDoc::ProcessReadFrame(short* p_frame)
{
	m_wavein.ReadFrame(p_frame);
}


//
// Name :        CAudioProcessDoc::ProcessWriteFrame()
// Description : Write a frame of output to the current generation device.
//

void CAudioProcessDoc::ProcessWriteFrame(short* p_frame)
{
	m_waveformBuffer.Frame(p_frame);

	if (m_fileoutput) m_waveout.WriteFrame(p_frame);

	if (m_audiooutput) m_soundstream.WriteFrame(p_frame);
}


//
// Name :        CAudioProcessDoc::ProcessEnd()
// Description : End the generation process.
//

void CAudioProcessDoc::ProcessEnd()
{
	m_waveformBuffer.End();

	if (m_fileoutput) m_waveout.close();

	if (m_audiooutput) m_soundstream.Close();

	ProgressEnd(this);
}

//
// Name :        CAudioProcessDoc::OpenProcessFile()
// Description : This function opens the audio file for output.
// Returns :     true if successful...
//

bool CAudioProcessDoc::OpenProcessFile(CWaveOut& p_wave)
{
	p_wave.NumChannels(m_numChannels);
	p_wave.SampleRate(m_sampleRate);

	static WCHAR BASED_CODE szFilter[] = L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(FALSE, L".wav", NULL, 0, szFilter, NULL);
	if (dlg.DoModal() != IDOK) return false;

	p_wave.open(dlg.GetPathName());
	if (p_wave.fail()) return false;

	return true;
}


void CAudioProcessDoc::OnProcessFileoutput()
{
	m_fileoutput = !m_fileoutput;
}


void CAudioProcessDoc::OnProcessAudiooutput()
{
	m_audiooutput = !m_audiooutput;
}


void CAudioProcessDoc::OnUpdateProcessFileoutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_fileoutput);
}


void CAudioProcessDoc::OnUpdateProcessAudiooutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_audiooutput);
}


void CAudioProcessDoc::OnProcessParameters()
{
	CProcessDlg dlg;

	dlg.m_amplitude = m_amplitude;

	if (dlg.DoModal() == IDOK)
	{
		m_amplitude = dlg.m_amplitude;
	}
}


short CAudioProcessDoc::RangeBound(double d)
{
	if (d > 32767)
	{
		return 32767;
	}
	if (d < -32768)
	{
		return -32768;
	}
	return short(d);
}


void CAudioProcessDoc::OnQuestionbPartb()
{
	// Call to open the processing output
	if (!ProcessBegin()) return;

	short audio[2];

	int queuesize = 2 * SampleFrames();

	vector<short> queue;
	queue.resize(queuesize);

	int wrloc = 0;

	// Keep track of time
	double time = 0;

	for (double i = 0; i < SampleFrames(); i++ , time += 1. / SampleRate())
	{
		ProcessReadFrame(audio);

		wrloc = (wrloc + 2) % queuesize;
		queue[wrloc] = audio[0];
		queue[wrloc + 1] = audio[1];

		auto delay = 0.006 + sin(0.25 * 2 * M_PI * time) * 0.004;
		int delaylength = int((delay * SampleRate() + 0.5)) * 2;
		int rdloc = (wrloc + queuesize - delaylength) % queuesize;

		audio[0] = audio[0] / 2 + queue[rdloc++] / 2;
		audio[1] = audio[1] / 2 + queue[rdloc] / 2;

		audio[0] = RangeBound(audio[0] * m_amplitude);
		audio[1] = RangeBound(audio[1] * m_amplitude);

		ProcessWriteFrame(audio);

		// The progress control
		if (!ProcessProgress(double(i) / SampleFrames())) break;
	}

	// Call to close the generator output
	ProcessEnd();
}


void CAudioProcessDoc::OnQuestionbPartb2()
{
	// Call to open the processing output
	if (!ProcessBegin()) return;

	short audio[2];

	int queuesize = 2 * SampleFrames();

	vector<short> queue;
	queue.resize(queuesize);

	int wrloc = 0;

	// Keep track of time
	double time = 0;

	for (double i = 0; i < SampleFrames(); i++ , time += 1. / SampleRate())
	{
		ProcessReadFrame(audio);

		wrloc = (wrloc + 2) % queuesize;
		queue[wrloc] = audio[0];
		queue[wrloc + 1] = audio[1];

		auto delay = 0.025 + sin(0.25 * 2 * M_PI * time) * 0.005;
		int delaylength = int((delay * SampleRate() + 0.5)) * 2;
		int rdloc = (wrloc + queuesize - delaylength) % queuesize;

		audio[0] = audio[0] / 2 + queue[rdloc++] / 2;
		audio[1] = audio[1] / 2 + queue[rdloc] / 2;

		audio[0] = RangeBound(audio[0] * m_amplitude);
		audio[1] = RangeBound(audio[1] * m_amplitude);

		ProcessWriteFrame(audio);

		// The progress control
		if (!ProcessProgress(double(i) / SampleFrames())) break;
	}

	// Call to close the generator output
	ProcessEnd();
}


void CAudioProcessDoc::OnQuestionbPartb3()
{
	// Call to open the processing output
	if (!ProcessBegin()) return;

	short audio[2];

	vector<short> wavetableL, wavetableR;
	wavetableL.resize(SampleFrames());
	wavetableR.resize(SampleFrames());

	for (auto i = 0; i < SampleFrames(); i++)
	{
		ProcessReadFrame(audio);
		wavetableL[i] = audio[0];
		wavetableR[i] = audio[1];
	}

	double time = 0; // Keep track of time
	double sample = 0; // Keep track of sample amount
	double duration = SampleFrames() / SampleRate(); // Keep track of duration

	for (int sampleNum = 0.; sampleNum < SampleFrames(); time += 1. / SampleRate())
	{
		audio[0] = wavetableL[sampleNum];
		audio[1] = wavetableR[sampleNum];
		ProcessWriteFrame(audio);

		auto rate = 1 + 0.1 * cos(M_PI * time / duration);
		sample += rate;

		sampleNum = int(sample + 0.5);

		// The progress control
		if (!ProcessProgress(double(sampleNum) / SampleFrames())) break;
	}

	// Call to close the generator output
	ProcessEnd();
}


void CAudioProcessDoc::OnQuestionbPartb4()
{
	// Call to open the processing output
	if (!ProcessBegin()) return;

	short audio[2];

	vector<short> wavetableL, wavetableR;
	wavetableL.resize(SampleFrames());
	wavetableR.resize(SampleFrames());

	for (auto i = 0; i < SampleFrames(); i++)
	{
		ProcessReadFrame(audio);
		wavetableL[i] = audio[0];
		wavetableR[i] = audio[1];
	}

	// Keep track of time
	double time = 0, sample = 0;

	for (int sampleNum = 0.; sampleNum < SampleFrames(); time += 1. / SampleRate())
	{
		audio[0] = wavetableL[sampleNum];
		audio[1] = wavetableR[sampleNum];
		ProcessWriteFrame(audio);

		auto rate = 1 + 0.1 * sin(2 * M_PI * time * 3);
		sample += rate;

		sampleNum = int(sample + 0.5);

		// The progress control
		if (!ProcessProgress(double(sampleNum) / SampleFrames())) break;
	}

	// Call to close the generator output
	ProcessEnd();
}
