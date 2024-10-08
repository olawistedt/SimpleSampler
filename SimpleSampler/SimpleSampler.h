#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SampleFile.h"
#include "FileBrowser.h"

const double kSimplesamplerVersion = 1.1;
const double kGuard = 111.111;
const int kNumPresets = 1;
const int kNrOfReverseButtons = 4;

enum EParams
{
  kParamMasterVolume = 0,
  kParamBrowse,
  kParamUp = kParamBrowse + 12,
  kParamDown = kParamUp + 12,
  kParamReverse0 = kParamDown + 12,
  kNumParams = kParamReverse0 + kNrOfReverseButtons
};

enum ECtrlTags
{
  kCtrlTagVersionNumber = 0,
  kCtrlTagSampleName0,
  kCtrlTagBrowse0 = kCtrlTagSampleName0 + 12,
  kCtrlTagUp0 = kCtrlTagBrowse0 + 12,
  kCtrlTagDown0 = kCtrlTagUp0 + 12,
  kCtrlTagMasterVolume = kCtrlTagDown0 + 12,
  kCtrlTagReverse0,
  kNumCtrls = kCtrlTagReverse0 + kNrOfReverseButtons
};

using namespace iplug;
using namespace igraphics;

class SimpleSampler final : public Plugin
{
public:
  SimpleSampler(const InstanceInfo &info);
  virtual ~SimpleSampler();

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
  void OnIdle() override;
  void OnParamChange(int paramIdx) override;
  void OnParamChangeUI(int paramIdx, EParamSource source = kUnknown) override;

  void ProcessBlock(sample **inputs, sample **outputs, int nFrames) override;

protected:
  IMidiQueue mMidiQueue;
#endif

private:
  double mPlugUIScale;
  double mMasterVolume;
  void ChangeSampleFile(unsigned char nr, std::wstring fileName);

  SampleFile mSampleFile[12];
};
