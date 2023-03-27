#include "SimpleSampler.h"
#include "IPlug_include_in_plug_src.h"
#include <filesystem>

#if IPLUG_EDITOR
#include "IControls.h"
#endif

extern std::wstring gLastBrowsedFile;




////////////////////////////////////////////////////////////////////////////////////////
// The buttons
////////////////////////////////////////////////////////////////////////////////////////
class BounceBtnControl : public IBSwitchControl
{
public:
  BounceBtnControl(float x, float y, const IBitmap& bitmap, int paramIdx);
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
  int mParamIdx;
};

BounceBtnControl::BounceBtnControl(float x, float y, const IBitmap& bitmap, int paramIdx) :
  IBSwitchControl(x, y, bitmap, paramIdx)
{
  mParamIdx = paramIdx;
}

void BounceBtnControl::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  SetValue(1.0);
  SetDirty();
}

void BounceBtnControl::OnMouseUp(float x, float y, const IMouseMod& mod)
{
  SetValue(0.0);
  SetDirty();
}

////////////////////////////////////////////////////////////////////////////////////////
// SimpleSampler
////////////////////////////////////////////////////////////////////////////////////////

SimpleSampler::SimpleSampler(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 85., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {

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

    pGraphics->SetLayoutOnResize(true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    //    pGraphics->AttachPanelBackground(COLOR_LIGHT_GRAY);
    //    pGraphics->AttachControl(new IVSliderControl(sliderBounds, kParamGain), kCtrlTagSlider);
    //    pGraphics->AttachControl(new ITextControl(titleBounds, "SimpleSampler", IText(30)), kCtrlTagTitle);
    //    WDL_String buildInfoStr;
    //    GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
    //    pGraphics->AttachControl(new ITextControl(versionBounds, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)), kCtrlTagVersionNumber);

        //
        // Background
        //
    pGraphics->LoadBitmap(BACKGROUND_FN, 1, true);
    pGraphics->AttachBackground(BACKGROUND_FN);

    // The browse buttons
    for (int i = 0; i < 12; ++i)
    {
      pGraphics->AttachControl(new IBSwitchControl(37 + i * 80, 250, folderBtnBitmap[i], kParamBrowse + i), kCtrlTagBrowse0 + i);
    }
    
    // The arrow buttons
    for (int i = 0; i < 12; ++i)
    {
      pGraphics->AttachControl(new IBSwitchControl(55 + i * ((upBtnBitmap.W() / 2) + 48), 230, upBtnBitmap, kParamUp + i), kCtrlTagUp0 + i);
      pGraphics->AttachControl(new IBSwitchControl(55 + i * ((downBtnBitmap.W() / 2) + 48), 310, downBtnBitmap, kParamDown + i), kCtrlTagDown0 + i);
    }
  };
#endif
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
bool SimpleSampler::SerializeState(IByteChunk& chunk) const
{
#ifdef _DEBUG
  OutputDebugString("SerializeState() called\n");
#endif

  TRACE

    bool savedOK = true;

  // Set version of the preset format.
  double version = 1.0;
  savedOK &= (chunk.Put(&version) > 0);

  // Save parameters except the leds and the parameters that are stored in sequencer.
  int n = NParams();
  for (int i = kParamGain; i < n && savedOK; ++i)
  {
    const IParam* pParam = GetParam(i);
    Trace(TRACELOC, " %s %d %f", pParam->GetName(), i, pParam->Value());
    double v = pParam->Value();
    savedOK &= (chunk.Put(&v) > 0);
  }

  for (int i = 0; i < 12; i++)
  {
    int j = -1;
    do
    {
      j++;
      double dChar = mSampleFile[i].mFileName[j];
      savedOK &= (chunk.Put(&dChar) > 0);
    } while (mSampleFile[i].mFileName[j] != 0);
  }

  assert(savedOK == true);

  return savedOK;
}

//
// From hard disk to plugin.
//
int SimpleSampler::UnserializeState(const IByteChunk& chunk, int startPos)
{
#ifdef _DEBUG
  OutputDebugString("UnserializeState() called\n");
#endif

  TRACE

    ENTER_PARAMS_MUTEX

    int n = NParams(), pos = startPos;

  // Check version for the preset format
  double version;
  pos = chunk.Get(&version, pos);
  assert(version == 1.0);

  for (int i = kParamGain; i < n && pos >= 0; ++i)
  {
    IParam* pParam = GetParam(i);
    double v = 0.0;
    pos = chunk.Get(&v, pos);
//    pParam->Set(v);
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
  }


  //OnParamReset(kPresetRecall);

  LEAVE_PARAMS_MUTEX

    return pos;
}
#endif // IPLUG_EDITOR

void SimpleSampler::ChangeSampleFile(unsigned char nr, std::wstring wFileName)
{
  //using convert_type = std::codecvt_utf8<wchar_t>;
  //std::wstring_convert<convert_type, wchar_t> converter;
  //std::string converted_str = converter.to_bytes(wFileName);
  //simpleSampler->mSampleFile[nr].mFileName = converted_str.c_str();

  mSampleFile[nr].mFileName = wFileName;
  mSampleFile[nr].loadFile();
}

void SimpleSampler::OnReset()
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

void SimpleSampler::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;
  mMidiQueue.Add(msg); // Take care of MIDI events in ProcessBlock()
}

void SimpleSampler::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  Stereo stereo;

  //  const int nChans = NOutChansConnected();
  for (int offset = 0; offset < nFrames; ++offset)
  {
    while (!mMidiQueue.Empty())
    {
      IMidiMsg msg = mMidiQueue.Peek();
      if (msg.mOffset > offset) break;

      if (msg.StatusMsg() == IMidiMsg::kNoteOn)
      {
        if (msg.NoteNumber() >= 36 && msg.NoteNumber() <= 47)
        {
          mSampleFile[msg.NoteNumber() - 36].mCurrentSample = 0;
        }
      }
      mMidiQueue.Remove();
    }

    outputs[0][offset] = 0.0;
    outputs[1][offset] = 0.0;
    for (int i = 0; i < 12; i++)
    {
      stereo = mSampleFile[i].getStereo(); // Note: Call getStereo() only one time, than use .left and .right to get the values.
      outputs[0][offset] += stereo.left;
      outputs[1][offset] += stereo.right;
    }
    outputs[0][offset] /= 12.0;
    outputs[1][offset] /= 12.0;

    outputs[0][offset] *= GetParam(kParamGain)->Value() / 100. * 8;
    outputs[1][offset] *= GetParam(kParamGain)->Value() / 100. * 8;
  }
  mMidiQueue.Flush(nFrames);
}
#endif

#if IPLUG_DSP
//void SimpleSampler::OnParamChange(int paramIdx)
void SimpleSampler::OnParamChangeUI(int paramIdx, EParamSource source)
{
  if (source != kUI)
  {
    return;
  }

  double value = GetParam(paramIdx)->Value();

  // Browse buttons
  if (paramIdx >= kParamBrowse && paramIdx < kParamBrowse + 12)
  {
#ifdef _DEBUG
    std::wstring mess1 = L"Pressed browse file nr " + std::to_wstring(paramIdx - kParamBrowse) + L"\n";
    OutputDebugStringW(mess1.c_str());
#endif

    //if (value == 0.0)
    //{
    //  return;
    //}
    BasicFileOpen();
#ifdef _DEBUG
    std::wstring mess2 = L"File choosed " + gLastBrowsedFile + L"\n";
    OutputDebugStringW(mess2.c_str());
#endif
    ChangeSampleFile(paramIdx - kParamBrowse, gLastBrowsedFile);
  }

  // Arrow buttons
  if (paramIdx >= kParamUp && paramIdx < kParamUp + 12 || paramIdx >= kParamDown && paramIdx < kParamDown + 12)
  {
    //if (value == 0.0)
    //{
    //  return;
    //}
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
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
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
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
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
      if (fileFound == L"") // No file was found. The file searching for must be the last one.
      {
        fileFound = filePath;
      }
    }
#ifdef _DEBUG
    std::wstring mess = L"File stepped to " + fileFound + L" Ola Wistedt \n";
    OutputDebugStringW(mess.c_str());
#endif
    ChangeSampleFile(sampleNr, fileFound); // Is it wise to do this? Change file in DSP thread?
  }
}
#endif
