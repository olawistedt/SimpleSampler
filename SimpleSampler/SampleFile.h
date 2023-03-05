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
  bool loadFile(const char* fileName);
  Stereo SampleFile::getStereo();
  sf_count_t mCurrentSample;
  std::string mFileName;
private:
  SF_INFO    mSfinfo;
  double*    mBuffer;
  sf_count_t mSize;
};
