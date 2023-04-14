#include "SampleFile.h"
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
extern HINSTANCE ghInst;

typedef SNDFILE* (*LPFN_sf_wchar_open)       (const wchar_t*, int, SF_INFO*);
typedef int		     (*LPFN_sf_close)		  (SNDFILE*);
typedef sf_count_t(*LPFN_sf_read_double) (SNDFILE*, double*, sf_count_t);

static LPFN_sf_wchar_open  lpfn_sf_wchar_open;    // Function pointer
static LPFN_sf_close       lpfn_sf_close;
static LPFN_sf_read_double lpfn_sf_read_double;

#endif

SampleFile::SampleFile()
  : mBuffer(NULL), mSize(0), mSfinfo({ 0, 0, 0, 0, 0, 0 }), mCurrentSample(0)
{
  LoadDllFunctions();
}

SampleFile::~SampleFile()
{
  if (mBuffer != NULL) { delete mBuffer; mBuffer = NULL; }
}

#ifdef _WIN32
void SampleFile::LoadDllFunctions()
{
  CHAR buffer[2048];
  GetModuleFileName(ghInst, buffer, sizeof(buffer));
  *(strrchr(buffer, '\\') + 1) = '\0';  // Cut off the path to vst dll.
  std::string pluginDir(buffer);
  std::string sndFileDll = pluginDir + "\\sndfile.dll";

  HINSTANCE hDLL;              // Handle to DLL

  hDLL = LoadLibrary(sndFileDll.c_str());

  if (hDLL == NULL)
  {
    std::string sMess = "The sndfile.dll was not found. No samples can be loaded.\n"
      "Close down SimpleSampler and place the "
      "sndfile.dll in the ";
    sMess += pluginDir;
    sMess += " directory.\n"
      "Then restart your application.";

    MessageBox(NULL, sMess.c_str(),
      "Loading sndfile.dll Error", MB_OK);
    exit(1);
  }

  lpfn_sf_wchar_open = (LPFN_sf_wchar_open)GetProcAddress(hDLL, "sf_wchar_open");
  lpfn_sf_close = (LPFN_sf_close)GetProcAddress(hDLL, "sf_close");
  lpfn_sf_read_double = (LPFN_sf_read_double)GetProcAddress(hDLL, "sf_read_double");

  if (!lpfn_sf_wchar_open ||
    !lpfn_sf_close ||
    !lpfn_sf_read_double)
  {
    MessageBox(NULL, "The version of the sndfile.dll is probably wrong.", "Loading sndfile.dll Error", MB_OK);
    FreeLibrary(hDLL);
    exit(1);
  }
}
#endif

// Called by GUI
bool SampleFile::loadFile()
{
  //  assert(mFileName != L"");

#ifdef _WIN32
  SNDFILE* sndFile = lpfn_sf_wchar_open(mFileName.c_str(), SFM_READ, &mSfinfo);
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

  mSize = lpfn_sf_read_double(sndFile, mBuffer, buffSize);

  lpfn_sf_close(sndFile);

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
