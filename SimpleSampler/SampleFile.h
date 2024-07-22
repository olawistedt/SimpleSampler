#pragma once
#include "sndfile.h"
#include <string>

struct Stereo
{
  double left;
  double right;
};

class SampleFile
{
public:
  SampleFile();
  ~SampleFile();
  bool loadFile();
  void reverse();
  Stereo SampleFile::getStereo();
  float mVelocity;
  std::wstring mFileName;
  double *mBuffer;
  unsigned long mSize;
  unsigned long mFrames;
  unsigned long mCurrentSample;
  int mNrOfSampleChannels;

private:
#ifdef _WIN32
  void LoadDllFunctions();
#endif
  SF_INFO mSfinfo;
  std::string mDllDir;
  bool mInLoadingFile;
};
