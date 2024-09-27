
#ifdef _DEBUG
#define TRACER_BUILD
#define TRACETOSTDOUT
#endif

#include "SimpleSampler.h"
#include "IPlug_include_in_plug_src.h"
#include <filesystem>

#if IPLUG_EDITOR
#include "IControls.h"
#endif

extern std::wstring gLastBrowsedFile;

////////////////////////////////////////////////////////////////////////////////////////
// Windows specific
////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
std::filesystem::path
getDllPath()
{
  wchar_t dllPath[MAX_PATH];
  HMODULE hModule = GetModuleHandleA("SimpleSampler.vst3");
  if (hModule == NULL)
  {
    throw std::runtime_error("Failed to get module handle.");
  }
  GetModuleFileName(hModule, dllPath, MAX_PATH);
  return std::filesystem::path(dllPath).parent_path();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// The browse buttons
////////////////////////////////////////////////////////////////////////////////////////
class BounceBtnBrowseControl : public IBSwitchControl
{
public:
  BounceBtnBrowseControl(float x, float y, const IBitmap &bitmap, int paramIdx);
  void OnMouseDown(float x, float y, const IMouseMod &mod) override;
  void OnMouseUp(float x, float y, const IMouseMod &mod) override;
  //  void OnMouseOut() override;
};

BounceBtnBrowseControl::BounceBtnBrowseControl(float x,
                                               float y,
                                               const IBitmap &bitmap,
                                               int paramIdx) :
  IBSwitchControl(x, y, bitmap, paramIdx)
{
}

void
BounceBtnBrowseControl::OnMouseDown(float x, float y, const IMouseMod &mod)
{
  SetValue(0.0);
  SetDirty();
}

void
BounceBtnBrowseControl::OnMouseUp(float x, float y, const IMouseMod &mod)
{
  SetValue(1.0);
  SetDirty();
}

////////////////////////////////////////////////////////////////////////////////////////
// The arrow buttons
////////////////////////////////////////////////////////////////////////////////////////
class BounceBtnArrowControl : public IBSwitchControl
{
public:
  BounceBtnArrowControl(float x, float y, const IBitmap &bitmap, int paramIdx);
  void OnMouseDown(float x, float y, const IMouseMod &mod) override;
  void OnMouseUp(float x, float y, const IMouseMod &mod) override;
  //  void OnMouseOut() override;
};

BounceBtnArrowControl::BounceBtnArrowControl(float x,
                                             float y,
                                             const IBitmap &bitmap,
                                             int paramIdx) :
  IBSwitchControl(x, y, bitmap, paramIdx)
{
}

void
BounceBtnArrowControl::OnMouseDown(float x, float y, const IMouseMod &mod)
{
  SetValue(1.0);
  SetDirty();
}

void
BounceBtnArrowControl::OnMouseUp(float x, float y, const IMouseMod &mod)
{
  SetValue(0.0);
  SetDirty();
}

////////////////////////////////////////////////////////////////////////////////////////
// SimpleSampler
////////////////////////////////////////////////////////////////////////////////////////

SimpleSampler::SimpleSampler(const InstanceInfo &info) :
  Plugin(info, MakeConfig(kNumParams, kNumPresets)),
  mMasterVolume(85.0)
{
  GetParam(kParamMasterVolume)->InitDouble("Master Volume", 85., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR  // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS); };

  mLayoutFunc = [&](IGraphics *pGraphics)
  {
    // Load Bitmaps
    IBitmap folderBtnBitmap[12];
    folderBtnBitmap[0] = pGraphics->LoadBitmap((PNGBTNFOLDERC_FN), 2, true);
    folderBtnBitmap[1] = pGraphics->LoadBitmap((PNGBTNFOLDERCc_FN), 2, true);
    folderBtnBitmap[2] = pGraphics->LoadBitmap((PNGBTNFOLDERD_FN), 2, true);
    folderBtnBitmap[3] = pGraphics->LoadBitmap((PNGBTNFOLDERDd_FN), 2, true);
    folderBtnBitmap[4] = pGraphics->LoadBitmap((PNGBTNFOLDERE_FN), 2, true);
    folderBtnBitmap[5] = pGraphics->LoadBitmap((PNGBTNFOLDERF_FN), 2, true);
    folderBtnBitmap[6] = pGraphics->LoadBitmap((PNGBTNFOLDERFf_FN), 2, true);
    folderBtnBitmap[7] = pGraphics->LoadBitmap((PNGBTNFOLDERG_FN), 2, true);
    folderBtnBitmap[8] = pGraphics->LoadBitmap((PNGBTNFOLDERGg_FN), 2, true);
    folderBtnBitmap[9] = pGraphics->LoadBitmap((PNGBTNFOLDERA_FN), 2, true);
    folderBtnBitmap[10] = pGraphics->LoadBitmap((PNGBTNFOLDERAa_FN), 2, true);
    folderBtnBitmap[11] = pGraphics->LoadBitmap((PNGBTNFOLDERB_FN), 2, true);

    const IBitmap upBtnBitmap = pGraphics->LoadBitmap((PNGBTNUP_FN), 2, true);
    const IBitmap downBtnBitmap = pGraphics->LoadBitmap((PNGBTNDOWN_FN), 2, true);
    const IBitmap reverseBtnBitmap = pGraphics->LoadBitmap((PNGBTNREVERSE_FN), 2, true);
    const IBitmap masterKnobBtnBitmap = pGraphics->LoadBitmap((PNGKNOBMASTER_FN), 101, true);

    pGraphics->SetLayoutOnResize(true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    //    pGraphics->AttachPanelBackground(COLOR_DARK_GRAY);

    pGraphics->AttachControl(new IBKnobControl(870, 32, masterKnobBtnBitmap, kParamMasterVolume),
                             kCtrlTagMasterVolume);

    for (int i = 0; i < 12; i++)
    {
      pGraphics->AttachControl(new ITextControl({ 37 + static_cast<float>(i) * 80,
                                                  200,
                                                  37 + (static_cast<float>(i) + 1) * 80,
                                                  220 },
                                                "Empty1",
                                                IText(14, COLOR_WHITE)),
                               kCtrlTagSampleName0 + i);
    }

    //    WDL_String buildInfoStr;
    //    GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
    //    pGraphics->AttachControl(new ITextControl(versionBounds, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)), kCtrlTagVersionNumber);

    //
    // Background
    //
    pGraphics->LoadBitmap(BACKGROUND_FN, 1, true);
    pGraphics->AttachBackground(BACKGROUND_FN);

    for (int i = 0; i < 12; ++i)
    {
      // The browse buttons
      pGraphics->AttachControl(new BounceBtnBrowseControl(37 + static_cast<float>(i) * 80,
                                                          250,
                                                          folderBtnBitmap[i],
                                                          kParamBrowse + i),
                               kCtrlTagBrowse0 + i);
      // The arrow buttons
      pGraphics->AttachControl(new BounceBtnArrowControl(55 + static_cast<float>(i) *
                                                                  ((upBtnBitmap.W() / 2) + 48),
                                                         230,
                                                         upBtnBitmap,
                                                         kParamUp + i),
                               kCtrlTagUp0 + i);
      pGraphics->AttachControl(new BounceBtnArrowControl(55 + static_cast<float>(i) *
                                                                  ((downBtnBitmap.W() / 2) + 48),
                                                         310,
                                                         downBtnBitmap,
                                                         kParamDown + i),
                               kCtrlTagDown0 + i);
    }
    for (int i = 0; i < kNrOfReverseButtons; i++)
    {
      // The reverse buttons
      pGraphics->AttachControl(new IBSwitchControl(37 + (12 - kNrOfReverseButtons) * 80 +
                                                       static_cast<float>(i) * 80,
                                                   145,
                                                   reverseBtnBitmap,
                                                   kParamReverse0 + i),
                               kCtrlTagReverse0 + i);
    }
  };
#endif
}

void
SimpleSampler::OnUIOpen()
{
  for (int i = 0; i < 12; i++)
  {
    std::string stripped = GetNarrowFileName(mSampleFile[i].mFileName);
    ((ITextControl *)(GetUI()->GetControlWithTag(kCtrlTagSampleName0 + i)))
        ->SetStr(stripped.c_str());
    GetUI()->GetControlWithTag(kCtrlTagSampleName0 + i)->SetDirty(true);
  }

  IEditorDelegate::OnUIOpen();
}

//#if IPLUG_EDITOR
//void SimpleSampler::OnParentWindowResize(int width, int height)
//{
//  if (GetUI())
//    GetUI()->Resize(width, height, 1.f, false);
//}
//#endif

//
// Save plugin settings to hard drive. First save is junk.
//
#if IPLUG_EDITOR
bool
SimpleSampler::SerializeState(IByteChunk &chunk) const
{
#ifdef _DEBUG
  OutputDebugString("SerializeState() called\n");
#endif

  TRACE

  bool savedOK = true;

  // Set version of the preset format.
  double version = kSimplesamplerVersion;
  savedOK &= (chunk.Put(&version) > 0);

  // Save parameters except the leds and the parameters that are stored in sequencer.
  int n = NParams();
  for (int i = kParamMasterVolume; i < n && savedOK; ++i)
  {
    const IParam *pParam = GetParam(i);
    Trace(TRACELOC, " %s %d %f", pParam->GetName(), i, pParam->Value());
    double v = pParam->Value();
    savedOK &= (chunk.Put(&v) > 0);
  }

  for (int i = 0; i < 12; i++)
  {
    // Save the sample path
    int j = -1;
    do
    {
      j++;
      double dChar = mSampleFile[i].mFileName[j];
      savedOK &= (chunk.Put(&dChar) > 0);
    } while (mSampleFile[i].mFileName[j] != 0);

    // Save the sample content
    double nrOfSamples = mSampleFile[i].mSize;
    savedOK &= (chunk.Put(&nrOfSamples) > 0);  // First the size of the sample buffer
    if (nrOfSamples > 0)
    {
      double nrOfChannels = mSampleFile[i].mNrOfSampleChannels;
      assert(nrOfChannels == 1.0 || nrOfChannels == 2.0);
      savedOK &= (chunk.Put(&nrOfChannels) > 0);
      for (j = 0; j < mSampleFile[i].mSize; j++)  // Then the sample values
      {
        savedOK &= (chunk.Put(&(mSampleFile[i].mBuffer[j])) > 0);
      }
    }
  }

  // Put a guard here
  savedOK &= (chunk.Put(&kGuard) > 0);

  assert(savedOK == true);

  return savedOK;
}

//
// From hard disk to plugin.
//
int
SimpleSampler::UnserializeState(const IByteChunk &chunk, int startPos)
{

  //  return startPos;

  //
  // Ask if preset should be loaded IF a file named ask is in the plugin directory
  //
#ifdef _WIN32
  std::filesystem::path dllDirectory = getDllPath();
  std::filesystem::path filePath = dllDirectory / "ask";
  if (std::filesystem::exists(filePath))
  {
    int msgboxID =
        MessageBox(NULL,
                   L"Do you want to load the preset/project for the SimpleSampler plugin?",
                   L"SimpleSampler Plugin Message",
                   MB_ICONQUESTION | MB_YESNO);
    if (IDNO == msgboxID)
    {
      return startPos;
    }
  }
#endif

  TRACE

  Trace(TRACELOC, "Entering function %s", __FUNCSIG__);

  ENTER_PARAMS_MUTEX

  int n = NParams(), pos = startPos;

  // Check version for the preset format
  double version;
  pos = chunk.Get(&version, pos);
#ifdef _DEBUG
  std::wstring mess = L"### Preset file version: " + std::to_wstring(version) + L"\n";
  OutputDebugStringW(mess.c_str());
#endif

  if (version > kSimplesamplerVersion)
  {
    MessageBox(NULL,
               L"This project/preset uses a newer version of The SimpleSampler. Please upgrade to "
               L"the latest version of the plugin.",
               L"SimpleSampler Plugin Error Message",
               MB_OK | MB_ICONERROR);
    return pos;
  }

  if (version == 1.0)
  {
    n = 37;  // The version 1.0 did only have 37 parameters.
  }

  for (int i = kParamMasterVolume; i < n && pos >= 0; ++i)
  {
    IParam *pParam = GetParam(i);
    double v = 0.0;
    pos = chunk.Get(&v, pos);
    pParam->Set(v);
    Trace(TRACELOC, "%d %s %f", i, pParam->GetName(), pParam->Value());
  }

  for (int i = 0; i < 12; i++)
  {
    int j = -1;
    mSampleFile[i].mFileName = L"";
    double dChar;
    do
    {
      j++;
      pos = chunk.Get(&dChar, pos);
      if (dChar != 0.0)
      {
        mSampleFile[i].mFileName += static_cast<wchar_t>(dChar);
      }
    } while (dChar != 0.0);
#ifdef _DEBUG
    std::wstring mess = L"### Sample file name: " + mSampleFile[i].mFileName + L"\n";
    OutputDebugStringW(mess.c_str());
#endif

    static_cast<ITextControl *>(GetUI()->GetControlWithTag(kCtrlTagSampleName0 + i))
        ->SetStr(GetNarrowFileName(mSampleFile[i].mFileName).c_str());

    if (version >= kSimplesamplerVersion)
    {
      double nrOfSamples;
      pos = chunk.Get(&nrOfSamples, pos);
      if (nrOfSamples > 0)
      {
        mSampleFile[i].mSize = nrOfSamples;
        mSampleFile[i].mBuffer = new double[nrOfSamples];
        double nrOfChannels;
        pos = chunk.Get(&nrOfChannels, pos);
        mSampleFile[i].mNrOfSampleChannels = nrOfChannels;
        assert(nrOfChannels == 1.0 || nrOfChannels == 2.0);

        mSampleFile[i].mCurrentSample = nrOfSamples;
        mSampleFile[i].mFrames = nrOfSamples / nrOfChannels;

        for (j = 0; j < nrOfSamples; j++)
        {
          double sample;
          pos = chunk.Get(&sample, pos);
          mSampleFile[i].mBuffer[j] = sample;
        }
      }
    }
  }

  // Read the guard.
  double guard;
  pos = chunk.Get(&guard, pos);
  assert(guard == kGuard);
  if (guard != kGuard)
  {
    MessageBox(NULL,
               L"Something went wrong while loading the preset.",
               L"SimpleSampler Plugin Error Message",
               MB_OK | MB_ICONERROR);
  }


  //  OnParamReset(kPresetRecall);
  //  IPluginBase::UnserializeState(chunk, pos);)

  LEAVE_PARAMS_MUTEX

  Trace(TRACELOC, "Leaving %s", __FUNCSIG__);

  return pos;
}
#endif  // IPLUG_EDITOR

void
SimpleSampler::ChangeSampleFile(unsigned char nr, std::wstring wFileName)
{
  mSampleFile[nr].mFileName = wFileName;
  mSampleFile[nr].loadFile();
}

void
SimpleSampler::OnReset()
{
  for (int i = 0; i < 12; ++i)
  {
    if (mSampleFile[i].mFileName != L"")
    {
      mSampleFile[i].loadFile();
    }
  }
}

#if IPLUG_DSP

void
SimpleSampler::ProcessMidiMsg(const IMidiMsg &msg)
{
  TRACE;
  mMidiQueue.Add(msg);  // Take care of MIDI events in ProcessBlock()
}

void
SimpleSampler::ProcessBlock(sample **inputs, sample **outputs, int nFrames)
{
  Stereo stereo;

  for (int offset = 0; offset < nFrames; ++offset)
  {
    while (!mMidiQueue.Empty())
    {
      IMidiMsg msg = mMidiQueue.Peek();
      if (msg.mOffset > offset)
        break;

      if (msg.StatusMsg() == IMidiMsg::kNoteOn)
      {
        if (msg.NoteNumber() >= 36 && msg.NoteNumber() <= 47)
        {
          mSampleFile[msg.NoteNumber() - 36].mCurrentSample = 0;
          mSampleFile[msg.NoteNumber() - 36].mVelocity = static_cast<float>(msg.Velocity());
        }

        //// The first holder can send MIDI.
        //if (msg.NoteNumber() == 36)
        //{
        //  IMidiMsg midiMessage;
        //  midiMessage.MakeNoteOnMsg(msg.NoteNumber(), msg.Velocity(), 0);
        //  IPlugVST3ProcessorBase::SendMidiMsg(midiMessage);
        //}
        //// The first holder can send MIDI.
      }

      //// The first holder can send MIDI.
      //if (msg.NoteNumber() == 36)
      //{
      //  if (msg.StatusMsg() == IMidiMsg::kNoteOff)
      //  {
      //    IMidiMsg midiMessage;
      //    midiMessage.MakeNoteOffMsg(msg.NoteNumber(), 0);
      //    IPlugVST3ProcessorBase::SendMidiMsg(midiMessage);
      //  }
      //}
      //// The first holder can send MIDI.


      mMidiQueue.Remove();
    }

    for (int i = 0; i < 24; i += 2)
    {
      stereo =
          mSampleFile[i / 2]
              .getStereo();  // Note: Call getStereo() only one time, than use .left and .right to get the values.
      outputs[i][offset] = stereo.left * mMasterVolume / 100.0;
      outputs[i + 1][offset] = stereo.right * mMasterVolume / 100.0;
    }
  }
  mMidiQueue.Flush(nFrames);
}
#endif

std::string
SimpleSampler::GetNarrowFileName(std::wstring f)
{
  std::wstring stripped =
      f.substr(f.rfind(L"\\") + 1,
               f.length() - (f.rfind(L"\\") + 1) - (f.length() - f.rfind(L".")));

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string s = converter.to_bytes(stripped);

  return s;
}

#if IPLUG_DSP

// OnParamChange() must be used to pick up parameter changes done during a audio mixdown. For
// parameters that are doing this and also affects the gui must be handled in both
// OnParamChange() and in OnParamChangeUI().
void
SimpleSampler::OnParamChange(int paramIdx)
{
  double value = GetParam(paramIdx)->Value();

  // Master Volume
  if (paramIdx == kParamMasterVolume)
  {
    mMasterVolume = value;
  }
}

void
SimpleSampler::OnParamChangeUI(int paramIdx, EParamSource source)
{
  if (source != kUI && source != kHost)
  {
    return;
  }

  double value = GetParam(paramIdx)->Value();

  // Master Volume
  if (paramIdx == kParamMasterVolume)
  {
    mMasterVolume = value;
  }

  // Browse buttons
  if (paramIdx >= kParamBrowse && paramIdx < kParamBrowse + 12)
  {
    if (value == 0.0)
    {
      return;
    }

#ifdef _DEBUG
    std::wstring mess1 =
        L"Pressed browse file nr " + std::to_wstring(paramIdx - kParamBrowse) + L"\n";
    OutputDebugStringW(mess1.c_str());
#endif

    gLastBrowsedFile = mSampleFile[paramIdx - kParamBrowse].mFileName;
    BasicFileOpen();

    std::string stripped = GetNarrowFileName(gLastBrowsedFile);
    ((ITextControl *)(GetUI()->GetControlWithTag(kCtrlTagSampleName0 + paramIdx - kParamBrowse)))
        ->SetStr(stripped.c_str());
    GetUI()->GetControlWithTag(kCtrlTagSampleName0 + paramIdx - kParamBrowse)->SetDirty(true);

#ifdef _DEBUG
    std::wstring mess2 = L"File choosed " + gLastBrowsedFile + L"\n";
    OutputDebugStringW(mess2.c_str());
#endif

    // Is it stupid to call GUI here?
    GetUI()->GetControlWithTag(kCtrlTagBrowse0 + paramIdx - kParamBrowse)->SetValue(0.0);
    GetUI()->GetControlWithTag(kCtrlTagBrowse0 + paramIdx - kParamBrowse)->SetDirty(true);

    ChangeSampleFile(paramIdx - kParamBrowse, gLastBrowsedFile);
  }

  // Arrow buttons
  if (paramIdx >= kParamUp && paramIdx < kParamUp + 12 ||
      paramIdx >= kParamDown && paramIdx < kParamDown + 12)
  {
    if (value == 0.0)
    {
      return;
    }
    int sampleNr;
    if (paramIdx >= kParamUp && paramIdx < kParamUp + 12)
    {
      sampleNr = paramIdx - kParamUp;
#ifdef _DEBUG
      std::wstring mess1 = L"Pressed up nr " + std::to_wstring(sampleNr) + L"\n";
      OutputDebugStringW(mess1.c_str());
#endif
    }
    else
    {
      sampleNr = paramIdx - kParamDown;
#ifdef _DEBUG
      std::wstring mess2 = L"Pressed down nr " + std::to_wstring(sampleNr) + L"\n";
      OutputDebugStringW(mess2.c_str());
#endif
    }
    std::wstring filePath = mSampleFile[sampleNr].mFileName;
    std::wstring directory_path = filePath.substr(0, filePath.rfind('\\'));
    std::wstring upFile = L"";
    std::wstring previousFile = L"";
    std::wstring fileFound;
    bool takeTheNextOne = false;

    if (directory_path != L"")
    {
      if (paramIdx >= kParamUp && paramIdx < kParamUp + 12)
      {
        for (const auto &file : std::filesystem::directory_iterator(directory_path))
        {
          std::wstring currentFile(file.path().c_str());
          if (currentFile == filePath)
          {
            if (upFile == L"")
            {
              fileFound = currentFile;
              break;
            }
            else
            {
              fileFound = upFile;
            }
          }
          upFile = currentFile;
        }
      }
      if (paramIdx >= kParamDown && paramIdx < kParamDown + 12)
      {
        for (const auto &file : std::filesystem::directory_iterator(directory_path))
        {
          std::wstring currentFile(file.path().c_str());
          if (takeTheNextOne)
          {
            fileFound = currentFile;
            break;
          }
          if (currentFile == filePath)
          {
            takeTheNextOne = true;
          }
        }
      }
      if (fileFound == L"")  // No file was found. The file searching for must be the last one.
      {
        fileFound = filePath;
      }
    }
#ifdef _DEBUG
    std::wstring mess = L"File stepped to " + fileFound + L"\n";
    OutputDebugStringW(mess.c_str());
#endif
    ChangeSampleFile(sampleNr, fileFound);  // Is it wise to do this? Change file in DSP thread?

    std::string fn = GetNarrowFileName(mSampleFile[sampleNr].mFileName);
    static_cast<ITextControl *>(GetUI()->GetControlWithTag(kCtrlTagSampleName0 + sampleNr))
        ->SetStr(fn.c_str());
    GetUI()->GetControlWithTag(kCtrlTagSampleName0 + sampleNr)->SetDirty(true);
  }
  if (paramIdx >= kParamReverse0 && paramIdx < kParamReverse0 + kNrOfReverseButtons)
  {
    mSampleFile[paramIdx - kParamReverse0 + 12 - kNrOfReverseButtons].reverse();
  }
}
#endif
