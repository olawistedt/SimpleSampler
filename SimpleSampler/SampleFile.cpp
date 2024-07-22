#include "SampleFile.h"
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#ifndef APP_API
extern HINSTANCE ghInst;
#endif

typedef SNDFILE *(*LPFN_sf_wchar_open)(const wchar_t *, int, SF_INFO *);
typedef int (*LPFN_sf_close)(SNDFILE *);
typedef sf_count_t (*LPFN_sf_read_double)(SNDFILE *, double *, sf_count_t);

static LPFN_sf_wchar_open lpfn_sf_wchar_open;  // Function pointer
static LPFN_sf_close lpfn_sf_close;
static LPFN_sf_read_double lpfn_sf_read_double;

#endif

SampleFile::SampleFile() :
  mBuffer(NULL),
  mSize(0),
  mSfinfo({ 0, 0, 0, 0, 0, 0 }),
  mCurrentSample(0),
  mNrOfSampleChannels(0),
  mInLoadingFile(false)
{
  LoadDllFunctions();
}

SampleFile::~SampleFile()
{
  if (mBuffer != NULL)
  {
    delete mBuffer;
    mBuffer = NULL;
  }
}

#ifdef _WIN32
void
SampleFile::LoadDllFunctions()
{
  CHAR buffer[2048];
#ifndef APP_API
  GetModuleFileName(ghInst, buffer, sizeof(buffer));
#else
  GetModuleFileName(nullptr, buffer, sizeof(buffer));
#endif

  *(strrchr(buffer, '\\') + 1) = '\0';  // Cut off the path to vst dll.
  std::string pluginDir(buffer);
  std::string sndFileDll = pluginDir + "\\sndfile.dll";

  HINSTANCE hDLL;  // Handle to DLL

  hDLL = LoadLibrary(sndFileDll.c_str());

  if (hDLL == NULL)
  {
    std::string sMess = "The sndfile.dll was not found. No samples can be loaded.\n"
                        "Close down SimpleSampler and place the "
                        "sndfile.dll in the ";
    sMess += pluginDir;
    sMess += " directory.\n"
             "Then restart your application.";

    MessageBox(NULL, sMess.c_str(), "Loading sndfile.dll Error", MB_OK);
    exit(1);
  }

  lpfn_sf_wchar_open = (LPFN_sf_wchar_open)GetProcAddress(hDLL, "sf_wchar_open");
  lpfn_sf_close = (LPFN_sf_close)GetProcAddress(hDLL, "sf_close");
  lpfn_sf_read_double = (LPFN_sf_read_double)GetProcAddress(hDLL, "sf_read_double");

  if (!lpfn_sf_wchar_open || !lpfn_sf_close || !lpfn_sf_read_double)
  {
    MessageBox(NULL,
               "The version of the sndfile.dll is probably wrong.",
               "Loading sndfile.dll Error",
               MB_OK);
    FreeLibrary(hDLL);
    exit(1);
  }
}
#endif

// Called by GUI
bool
SampleFile::loadFile()
{
  mInLoadingFile = true;
  //  assert(mFileName != L"");

  OutputDebugStringW(std::wstring(L"Loading file '" + mFileName + L"'\n").c_str());

#ifdef _WIN32
  SNDFILE *sndFile = lpfn_sf_wchar_open(mFileName.c_str(), SFM_READ, &mSfinfo);
#endif

  if (sndFile == NULL)
  {
    OutputDebugStringW(std::wstring(L"File not found or couldn't be read '" + mFileName +
                                    L"' Using current buffer instead\n")
                           .c_str());
    mInLoadingFile = false;
    mFrames = mSize / mNrOfSampleChannels;
    // Let the mBuffer still be valid, could contain the previous loaded sample.
    return true;  // This is a valid behaviour.
  }

  mFrames = static_cast<unsigned long>(mSfinfo.frames);

  mNrOfSampleChannels = mSfinfo.channels;
  mSize = mFrames * mNrOfSampleChannels;

  assert(mNrOfSampleChannels == 1 ||
         mNrOfSampleChannels == 2);  // Only mono or stereo files will work

  if (mBuffer)
  {
    delete mBuffer;
  }

  mBuffer = new double[mSize];

  lpfn_sf_read_double(sndFile, mBuffer, mSize);

  lpfn_sf_close(sndFile);

  mCurrentSample = mSize / mNrOfSampleChannels;

  mInLoadingFile = false;

  return true;
}

void
SampleFile::reverse()
{
  if (mBuffer == NULL)
  {
    return;
  }
  double *tmpBuffer = new double[mFrames];
  if (mNrOfSampleChannels == 2)  // Is stereo
  {
    for (int i = 0; i < mFrames; i += 2)
    {
      tmpBuffer[i] = mBuffer[mFrames - i - 1];
      tmpBuffer[i + 1] = mBuffer[mFrames - i];
    }
    for (int i = 0; i < mFrames; i += 2)
    {
      mBuffer[i] = tmpBuffer[i];
      mBuffer[i + 1] = tmpBuffer[i + 1];
    }
  }
  else  // Mono
  {
    for (int i = 0; i < mFrames; i += 1)
    {
      tmpBuffer[i] = mBuffer[mFrames - i];
    }
    for (int i = 0; i < mFrames; i += 1)
    {
      mBuffer[i] = tmpBuffer[i];
    }
  }
  delete tmpBuffer;
}
// Called by DSP
Stereo
SampleFile::getStereo()
{
  Stereo stereo = { 0.0, 0.0 };

  if (mInLoadingFile || mBuffer == NULL)
  {
    return stereo;
  }  // Silent
  if (mCurrentSample >= mFrames)
  {
    return stereo;
  }  // Silent
  if (mCurrentSample < mFrames && mBuffer != NULL)
  {
    if (mNrOfSampleChannels == 2)  // Is stereo
    {
      stereo.left = mBuffer[mCurrentSample * 2] * mVelocity / 127.0;       // * m_fLeftVolume;
      stereo.right = mBuffer[mCurrentSample * 2 + 1] * mVelocity / 127.0;  // *m_fRightVolume;
    }
    else  // Is mono
    {
      assert(mNrOfSampleChannels == 1);                            // Only mono files will work
      stereo.left = mBuffer[mCurrentSample] * mVelocity / 127.0;   // * m_fLeftVolume;
      stereo.right = mBuffer[mCurrentSample] * mVelocity / 127.0;  // *m_fRightVolume;
    }
    mCurrentSample++;
  }
  return stereo;
}
