#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SampleFile.h"
#include "FileBrowser.h"

const int kNumPresets = 1;

enum EParams
{
  kParamMasterVolume = 0,
  kParamBrowse,
  kParamUp = kParamBrowse + 12,
  kParamDown = kParamUp + 12,
  kNumParams = kParamDown + 12
};

enum ECtrlTags
{
  kCtrlTagVersionNumber = 0,
  kCtrlTagSampleName0,
  kCtrlTagBrowse0 = kCtrlTagSampleName0 + 12,
  kCtrlTagUp0 = kCtrlTagBrowse0 + 12,
  kCtrlTagDown0 = kCtrlTagUp0 + 12,
  kCtrlTagMasterVolume = kCtrlTagDown0 + 12,
  kNumCtrls
};

using namespace iplug;
using namespace igraphics;

class SimpleSampler final : public Plugin
{
public:
  SimpleSampler(const InstanceInfo &info);

#if IPLUG_EDITOR
  //  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
  bool SerializeState(IByteChunk &chunk) const override;
  int UnserializeState(const IByteChunk &chunk, int startPos) override;
  void OnUIOpen() override;

#endif
  std::string GetNarrowFileName(std::wstring file);

#if IPLUG_DSP  // http://bit.ly/2S64BDd
  void ProcessMidiMsg(const IMidiMsg &msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnParamChangeUI(int paramIdx, EParamSource source = kUnknown) override;

  void ProcessBlock(sample **inputs, sample **outputs, int nFrames) override;

protected:
  IMidiQueue mMidiQueue;
#endif

private:
  double mMasterVolume;
  void ChangeSampleFile(unsigned char nr, std::wstring fileName);

  SampleFile mSampleFile[12];
};
