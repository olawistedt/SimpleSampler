
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vSimpleSampler
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vSimpleSampler
#import <SimpleSamplerAU/IPlugAUViewController.h>
#import <SimpleSamplerAU/IPlugAUAudioUnit.h>

//! Project version number for SimpleSamplerAU.
FOUNDATION_EXPORT double SimpleSamplerAUVersionNumber;

//! Project version string for SimpleSamplerAU.
FOUNDATION_EXPORT const unsigned char SimpleSamplerAUVersionString[];

@class IPlugAUViewController_vSimpleSampler;
