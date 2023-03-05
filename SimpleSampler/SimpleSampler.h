#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SampleFile.h"
#include "FileBrowser.h"

const int kNumPresets = 1;

enum EParams
{
  kParamGain = 0,
  kNumParams
};

enum ECtrlTags
{
  kCtrlTagVersionNumber = 0,
  kCtrlTagSlider,
  kCtrlTagTitle
};

using namespace iplug;
using namespace igraphics;

class SimpleSampler final : public Plugin
{
public:
  SimpleSampler(const InstanceInfo& info);

#if IPLUG_EDITOR
  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
  bool SerializeState(IByteChunk& chunk) const override;
  int UnserializeState(const IByteChunk& chunk, int startPos) override;
#endif
  
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;

protected:
  IMidiQueue mMidiQueue;
#endif

private:
  static void ChangeSampleFile(SimpleSampler* simpleSampler, unsigned char nr, std::wstring fileName);

  SampleFile mSampleFile[12];
};