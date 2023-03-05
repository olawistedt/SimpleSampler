#include "SimpleSampler.h"
#include "IPlug_include_in_plug_src.h"

#if IPLUG_EDITOR
#include "IControls.h"
#endif

extern std::wstring gLastBrowsedFile;

SimpleSampler::SimpleSampler(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 85., 0., 100.0, 0.01, "%");

  mSampleFile[0].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[1].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\CL\\CL.WAV");
  mSampleFile[2].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\SD\\SD2550.WAV");
  mSampleFile[3].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\CP\\CP.WAV");
  mSampleFile[4].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[5].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[6].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[7].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[8].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[9].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[10].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  mSampleFile[11].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\Samples\\Roland TR-808 sampled by Michael Fischer\\BD\\BD0010.WAV");
  //  mSampleFile[0].loadFile("C:\\Users\\ola\\OneDrive\\egen musik\\MP3-Mixdowns\\toypiano-piano - drums - arr2.wav");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {

    // Load Bitmaps
    const IBitmap folderBtnBitmap = pGraphics->LoadBitmap((PNGFOLDERBTN_FN), 2, true);

    const IRECT bounds = pGraphics->GetBounds();
    const IRECT innerBounds = bounds.GetPadded(-10.f);
    const IRECT sliderBounds = innerBounds.GetFromLeft(150).GetMidVPadded(100);
    const IRECT versionBounds = innerBounds.GetFromTRHC(300, 20);
    const IRECT titleBounds = innerBounds.GetCentredInside(200, 50);

    if (pGraphics->NControls()) {
      pGraphics->GetBackgroundControl()->SetTargetAndDrawRECTs(bounds);
      pGraphics->GetControlWithTag(kCtrlTagSlider)->SetTargetAndDrawRECTs(sliderBounds);
      pGraphics->GetControlWithTag(kCtrlTagTitle)->SetTargetAndDrawRECTs(titleBounds);
      pGraphics->GetControlWithTag(kCtrlTagVersionNumber)->SetTargetAndDrawRECTs(versionBounds);
      return;
    }

    pGraphics->SetLayoutOnResize(true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Size, true);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->AttachPanelBackground(COLOR_LIGHT_GRAY);
    pGraphics->AttachControl(new IVSliderControl(sliderBounds, kParamGain), kCtrlTagSlider);
    pGraphics->AttachControl(new ITextControl(titleBounds, "SimpleSampler", IText(30)), kCtrlTagTitle);
    WDL_String buildInfoStr;
    GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
    pGraphics->AttachControl(new ITextControl(versionBounds, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)), kCtrlTagVersionNumber);

    //
    // The browse buttons
    //
    pGraphics->AttachControl(new IBButtonControl(50 + 0 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 0, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 1 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 1, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 2 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 2, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 3 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 3, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 4 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 4, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 5 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 5, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 6 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 6, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 7 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 7, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 8 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 8, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 9 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 9, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 10 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 10, gLastBrowsedFile);
      return; }), kNoTag, "Channels");
    pGraphics->AttachControl(new IBButtonControl(50 + 11 * 40, 150, folderBtnBitmap, [&](IControl* pCaller) {
      BasicFileOpen();
      SimpleSampler::ChangeSampleFile(this, 11, gLastBrowsedFile);
      return; }), kNoTag, "Channels");

  };
#endif
}

#if IPLUG_EDITOR
void SimpleSampler::OnParentWindowResize(int width, int height)
{
  if (GetUI())
    GetUI()->Resize(width, height, 1.f, false);
}
#endif

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
// From hard disk to BassMatrix.
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
    pParam->Set(v);
    Trace(TRACELOC, "%d %s %f", i, pParam->GetName(), pParam->Value());
  }

  for (int i = 0; i < 12; i++)
  {
    int j = -1;
    mSampleFile[i].mFileName = "";
    double dChar;
    do
    {
      j++;
      pos = chunk.Get(&dChar, pos);
      mSampleFile[i].mFileName += dChar;
    } while (dChar != 0.0);
  }


  //OnParamReset(kPresetRecall);

  LEAVE_PARAMS_MUTEX

    return pos;
}
#endif // IPLUG_EDITOR

void SimpleSampler::ChangeSampleFile(SimpleSampler* simpleSampler, unsigned char nr, std::wstring wFileName)
{
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;
  std::string converted_str = converter.to_bytes(wFileName);
  simpleSampler->mSampleFile[nr].loadFile(converted_str.c_str());
}

void SimpleSampler::OnReset()
{
}

#if IPLUG_DSP

void SimpleSampler::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;
  mMidiQueue.Add(msg); // Take care of MIDI events in ProcessBlock()
}

void SimpleSampler::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();
  for (int offset = 0; offset < nFrames; offset += nChans)
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
    for (int i = 0; i < 12; i++)
    {
      outputs[0][offset] += mSampleFile[i].getStereo().left;
      outputs[1][offset] += mSampleFile[i].getStereo().right;
    }
    outputs[0][offset] /= 12.0;
    outputs[1][offset] /= 12.0;

    outputs[0][offset] *= GetParam(kParamGain)->Value() / 100. * 8;
    outputs[1][offset] *= GetParam(kParamGain)->Value() / 100. * 8;
  }
  mMidiQueue.Flush(nFrames);
}
#endif
