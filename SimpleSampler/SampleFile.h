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
  Stereo SampleFile::getStereo();
  sf_count_t mCurrentSample;
  std::wstring mFileName;
private:
#ifdef _WIN32
  void LoadDllFunctions();
#endif
  SF_INFO    mSfinfo;
  double*    mBuffer;
  sf_count_t mSize;
  std::string mDllDir;
};
