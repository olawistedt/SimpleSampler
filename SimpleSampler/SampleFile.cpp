#include "SampleFile.h"
#include <assert.h>

SampleFile::SampleFile()
  : mBuffer(NULL), mSize(0), mSfinfo({ 0, 0, 0, 0, 0, 0 }), mCurrentSample(0)
{
}

SampleFile::~SampleFile()
{
  if (mBuffer != NULL) { delete mBuffer; mBuffer = NULL; }
}

// Called by GUI
bool SampleFile::loadFile()
{
  assert(mFileName != L"");

#ifdef _WIN32
  SNDFILE* sndFile = sf_wchar_open(mFileName.c_str(), SFM_READ, &mSfinfo);
#endif

  if (sndFile == NULL)
  {
    mBuffer = NULL;
    return false;
  }

  sf_count_t buffSize = mSfinfo.frames * mSfinfo.channels;

  assert(mSfinfo.channels == 1 || mSfinfo.channels == 2); // Only mono or stereo files will work

  if (mBuffer) { delete mBuffer; }

  mBuffer = new double[buffSize];

  mSize = sf_read_double(sndFile, mBuffer, buffSize);

  sf_close(sndFile);

  mCurrentSample = mSfinfo.frames;

  return true;
}

// Called by DSP
Stereo SampleFile::getStereo()
{
  Stereo stereo = { 0.0, 0.0 };

  if (mBuffer == NULL) { return stereo; } // Silent
  if (mCurrentSample >= mSfinfo.frames) { return stereo; } // Silent
  if (mCurrentSample < mSfinfo.frames && mBuffer != NULL)
  {
    if (mSfinfo.channels == 2) // Is stereo
    {
      stereo.left = mBuffer[mCurrentSample * 2]; // * m_fLeftVolume;
      stereo.right = mBuffer[mCurrentSample * 2 + 1]; // *m_fRightVolume;
    }
    else // Is mono
    {
      assert(mSfinfo.channels == 1); // Only mono files will work
      stereo.left = mBuffer[mCurrentSample]; // * m_fLeftVolume;
      stereo.right = mBuffer[mCurrentSample]; // *m_fRightVolume;
    }
    mCurrentSample++;
  }
  return stereo;
}
