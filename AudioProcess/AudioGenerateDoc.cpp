// AudioGenerateDoc.cpp : implementation of the CAudioGenerateDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AudioProcess.h"
#endif

#include "AudioGenerateDoc.h"
#include "GenerateDlg.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAudioGenerateDoc

IMPLEMENT_DYNCREATE(CAudioGenerateDoc, CDocument)

BEGIN_MESSAGE_MAP(CAudioGenerateDoc, CDocument)
	ON_COMMAND(ID_GENERATE_FILEOUTPUT, &CAudioGenerateDoc::OnGenerateFileoutput)
	ON_COMMAND(ID_GENERATE_AUDIOOUTPUT, &CAudioGenerateDoc::OnGenerateAudiooutput)
	ON_COMMAND(ID_GENERATE_SINEWAVE, &CAudioGenerateDoc::OnGenerateSinewave)
	ON_COMMAND(ID_GEN_PARAMETERS, &CAudioGenerateDoc::OnGenParameters)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_FILEOUTPUT, &CAudioGenerateDoc::OnUpdateGenerateFileoutput)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_AUDIOOUTPUT, &CAudioGenerateDoc::OnUpdateGenerateAudiooutput)
	ON_COMMAND(ID_QUESTIONA_PART1, &CAudioGenerateDoc::OnQuestionaPart1)
	ON_COMMAND(ID_QUESTIONA_PART2, &CAudioGenerateDoc::OnQuestionaPart2)
	ON_COMMAND(ID_QUESTIONA_PART3, &CAudioGenerateDoc::OnQuestionaPart3)
	ON_COMMAND(ID_QUESTIONA_PART4, &CAudioGenerateDoc::OnQuestionaPart4)
	ON_COMMAND(ID_QUESTIONA_PART5, &CAudioGenerateDoc::OnQuestionaPart5)
	ON_COMMAND(ID_QUESTIONA_PART6, &CAudioGenerateDoc::OnQuestionaPart6)
	ON_COMMAND(ID_QUESTIONA_PART7, &CAudioGenerateDoc::OnQuestionaPart7)
	ON_COMMAND(ID_GENERATE_SQUAREWAVE, &CAudioGenerateDoc::OnGenerateSquarewave)
	ON_COMMAND(ID_QUESTIONA_PARTA, &CAudioGenerateDoc::OnQuestionaPart8)
	END_MESSAGE_MAP()


/*! Example procedure that generates a sine wave.
 * 
 * The sine wave frequency is set by m_freq1
 */
void CAudioGenerateDoc::OnGenerateSinewave()
{
	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		audio[0] = short(m_amplitude * sin(time * 2 * M_PI * m_freq1));
		audio[1] = short(m_amplitude * sin(time * 2 * M_PI * m_freq1));

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}

void CAudioGenerateDoc::OnGenerateSquarewave()
{
	// Setup variables
	m_freq1 = 882;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		short sqSample = 0, harmonic = 1;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sqSample += RangeBound(m_amplitude / harmonic
				* sin(time * 2 * M_PI * frequency));

			harmonic += 2;
		}

		audio[0] = RangeBound(sqSample);
		audio[1] = RangeBound(sqSample);

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


// CAudioGenerateDoc construction/destruction

CAudioGenerateDoc::CAudioGenerateDoc()
{
	m_freq1 = 440.;
	m_freq2 = 440.;
	m_vibratoamp = 20.;

	m_audiooutput = true;
	m_fileoutput = false;

	m_numChannels = 2;
	m_sampleRate = 44100.;
	m_duration = 10.0;
	m_amplitude = 3276.7;
}

CAudioGenerateDoc::~CAudioGenerateDoc()
{
}

BOOL CAudioGenerateDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;

	return TRUE;
}


void CAudioGenerateDoc::OnGenParameters()
{
	CGenerateDlg dlg;

	// Initialize dialog data
	dlg.m_Channels = m_numChannels;
	dlg.m_SampleRate = m_sampleRate;
	dlg.m_Duration = m_duration;
	dlg.m_Amplitude = m_amplitude;
	dlg.m_freq1 = m_freq1;
	dlg.m_freq2 = m_freq2;
	dlg.m_vibratoamp = m_vibratoamp;

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		// Accept the values
		m_numChannels = dlg.m_Channels;
		m_sampleRate = dlg.m_SampleRate;
		m_duration = dlg.m_Duration;
		m_amplitude = dlg.m_Amplitude;
		m_freq1 = dlg.m_freq1;
		m_freq2 = dlg.m_freq2;
		m_vibratoamp = dlg.m_vibratoamp;

		UpdateAllViews(NULL);
	}
}


#ifdef SHARED_HANDLERS

// Support for thumbnails
void CAudioGenerateDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CAudioGenerateDoc::InitializeSearchContent()
{
	CString strSearchContent;
// Set search contents from document's data. 
// The content parts should be separated by ";"

// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CAudioGenerateDoc::SetSearchContent(const CString& value)
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



// CAudioGenerateDoc diagnostics

#ifdef _DEBUG
void CAudioGenerateDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAudioGenerateDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



// CAudioGenerateDoc commands


/////////////////////////////////////////////////////////////////////////////
//
// The following functions manage the audio generation process, 
// directing output to the waveform buffer, file, and/or audio 
// output.  
//
/////////////////////////////////////////////////////////////////////////////


//
// Name :        CAudioGenerateDoc::GenerateBegin()
// Description : This function starts the audio generation process.
//               It opens the waveform storage, opens the file
//               if file output is requested, and opens the 
//               audio output if audio output is requested.
//               Be sure to call EndGenerate() when done.
// Returns :     true if successful...
//

bool CAudioGenerateDoc::GenerateBegin()
{
	// 
	// Waveform storage
	//

	m_waveformBuffer.Start(NumChannels(), SampleRate());

	if (m_fileoutput)
	{
		if (!OpenGenerateFile(m_wave)) return false;
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
// Name :        CAudioGenerateDoc::GenerateWriteFrame()
// Description : Write a frame of output to the current generation device.
// Parameters :  p_frame - An arrays with the number of channels in samples
//               in it.
//

void CAudioGenerateDoc::GenerateWriteFrame(short* p_frame)
{
	m_waveformBuffer.Frame(p_frame);

	if (m_fileoutput) m_wave.WriteFrame(p_frame);

	if (m_audiooutput) m_soundstream.WriteFrame(p_frame);
}


//
// Name :        CAudioGenerateDoc::GenerateEnd()
// Description : End the generation process.
//

void CAudioGenerateDoc::GenerateEnd()
{
	m_waveformBuffer.End();

	if (m_fileoutput) m_wave.close();

	if (m_audiooutput) m_soundstream.Close();

	ProgressEnd(this);
}

//
// Name :        CAudioGenerateDoc::OpenGenerateFile()
// Description : This function opens the audio file for output.
// Returns :     true if successful...
//

bool CAudioGenerateDoc::OpenGenerateFile(CWaveOut& p_wave)
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


short CAudioGenerateDoc::RangeBound(double d)
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

void CAudioGenerateDoc::OnGenerateFileoutput()
{
	m_fileoutput = !m_fileoutput;
}


void CAudioGenerateDoc::OnGenerateAudiooutput()
{
	m_audiooutput = !m_audiooutput;
}


void CAudioGenerateDoc::OnUpdateGenerateFileoutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_fileoutput);
}


void CAudioGenerateDoc::OnUpdateGenerateAudiooutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_audiooutput);
}


void CAudioGenerateDoc::OnQuestionaPart1()
{
	// Setup variables
	m_freq1 = 622.25;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		// the amount of sawtooth you can hear
		auto fac = time / m_duration;

		short sawSample = 0, harmonic = 1;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sawSample += RangeBound(m_amplitude / harmonic
				* sin(time * 2 * M_PI * frequency));

			harmonic++;
		}

		auto sinSample = RangeBound(m_amplitude * sin(time * 2 * M_PI * m_freq1));
		auto sample = RangeBound(fac * sawSample + (1 - fac) * sinSample);

		audio[0] = sample;
		audio[1] = sample;

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart2()
{
	// Setup variables
	m_freq1 = 622.25;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		// the amount of square wave you can hear
		auto fac = time / m_duration;

		short sawSample = 0, harmonic = 1;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sawSample += RangeBound(m_amplitude / harmonic
				* sin(time * 2 * M_PI * frequency));

			harmonic += 2;
		}

		auto sinSample = RangeBound(m_amplitude * sin(time * 2 * M_PI * m_freq1));
		audio[0] = RangeBound(fac * sawSample + (1 - fac) * sinSample);
		audio[1] = RangeBound(fac * sawSample + (1 - fac) * sinSample);

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart3()
{
	// Setup variables
	m_freq1 = 622.25;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		// the amount of triangle wave you can hear
		auto fac = time / m_duration;

		short sawSample = 0, harmonic = 1, phase = 0;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sawSample += RangeBound(pow(-1, phase) * m_amplitude / pow(harmonic, 2)
				* sin(time * 2 * M_PI * frequency));

			harmonic += 2;
			phase++;
		}

		auto sinSample = RangeBound(m_amplitude * sin(time * 2 * M_PI * m_freq1));
		audio[0] = RangeBound(fac * sawSample + (1 - fac) * sinSample);
		audio[1] = RangeBound(fac * sawSample + (1 - fac) * sinSample);

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart4()
{
	// Setup variables
	m_freq1 = 100;
	m_freq2 = 4000;
	m_duration = 10;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	// the amount of the dominant wave you can hear
	double fac;

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		auto lowSinSample = RangeBound(m_amplitude * sin(time * 2 * M_PI * m_freq1));
		auto highSinSample = RangeBound(m_amplitude * sin(time * 2 * M_PI * m_freq2));

		short sample;

		if (time < m_duration / 2.)
		{
			fac = 2. * time / m_duration;
		}
		else
		{
			fac = 2. * (1 - time / m_duration);
		}

		sample = RangeBound(fac * highSinSample + (1 - fac) * lowSinSample);

		audio[0] = sample;
		audio[1] = sample;

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart5()
{
	// Setup variables
	m_freq1 = 554.36;
	m_amplitude = 3000;
	m_duration = 10;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	// the amount of the dominant wave you can hear
	double fac;
	double radiansOne, radiansTwo;
	radiansOne = radiansTwo = 0;

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		if (time < m_duration / 2.)
		{
			fac = 2. * time / m_duration;
		}
		else
		{
			fac = 2. * (1 - time / m_duration);
		}

		auto sample = RangeBound(m_amplitude * sin(radiansOne));

		audio[0] = sample;
		audio[1] = sample;

		auto diff = 10 * sin(radiansTwo);

		auto vibRate = (3. - 0.1) * fac + 0.1;

		radiansOne += 2 * M_PI * (m_freq1 + diff) / m_sampleRate;
		radiansTwo += 2 * M_PI * vibRate / m_sampleRate;

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}

	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart6()
{
	// Setup variables
	m_freq1 = 587.33;
	m_amplitude = 3000;
	m_duration = 10;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	short audio[2];

	// the amount the tremolo rate varies by
	double fac;
	double radiansOne, radiansTwo;
	radiansOne = radiansTwo = 0;

	for (double time = 0.; time < m_duration; time += 1. / m_sampleRate)
	{
		if (time < m_duration / 2.)
		{
			fac = 2. * time / m_duration;
		}
		else
		{
			fac = 2. * (1 - time / m_duration);
		}

		auto diff = (1 + .1 * sin(radiansTwo));
		auto sample = RangeBound(m_amplitude * diff * sin(radiansOne));

		audio[0] = sample;
		audio[1] = sample;

		auto tremRate = (5. - 0.1) * fac + 0.1;

		radiansOne += 2 * M_PI * m_freq1 / m_sampleRate;
		radiansTwo += 2 * M_PI * tremRate / m_sampleRate;

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart7()
{
	// Setup variables
	m_freq1 = 882;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	vector<short> wavetable;
	wavetable.resize(m_sampleRate / m_freq1);

	short audio[2];

	double time = 0;
	for (auto i = 0; i < m_sampleRate / m_freq1; i++ , time += 1. / m_sampleRate)
	{
		short sample = 0, harmonic = 1;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sample += RangeBound(m_amplitude / harmonic
				* sin(time * 2 * M_PI * frequency));

			harmonic += 2;
		}

		wavetable[i] = RangeBound(sample);
	}

	auto p = 0;
	for (time = 0.; time < m_duration; p++ , time += 1. / m_sampleRate)
	{
		p %= int(m_sampleRate / m_freq1);
		audio[0] = wavetable[p];
		audio[1] = wavetable[p];

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}


void CAudioGenerateDoc::OnQuestionaPart8()
{
	// Setup variables
	m_freq1 = 882;
	m_duration = 5;

	// Call to open the generator output
	if (!GenerateBegin()) return;

	vector<short> wavetable;
	wavetable.resize(m_sampleRate / m_freq1);

	short audio[2];

	double time = 0;
	for (auto i = 0; i < m_sampleRate / m_freq1; i++ , time += 1. / m_sampleRate)
	{
		short sample = 0, harmonic = 1;
		double frequency = 0;

		while (frequency < m_sampleRate / 2.)
		{
			frequency = m_freq1 * harmonic;

			sample += RangeBound(m_amplitude / harmonic
				* sin(time * 2 * M_PI * frequency));

			harmonic++;
		}

		wavetable[i] = RangeBound(sample);
	}

	auto p = 0;
	for (time = 0.; time < m_duration; p++ , time += 1. / m_sampleRate)
	{
		p %= int(m_sampleRate / m_freq1);
		audio[0] = wavetable[p];
		audio[1] = wavetable[p];

		GenerateWriteFrame(audio);

		// The progress control
		if (!GenerateProgress(time / m_duration)) break;
	}


	// Call to close the generator output
	GenerateEnd();
}
