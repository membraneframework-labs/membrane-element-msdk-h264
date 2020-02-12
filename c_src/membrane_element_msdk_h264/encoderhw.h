#pragma once

#include <iostream>
#include "msdk/common_utils.h"
#include "msdk_membrane/membrane_utils.h"

struct MyState {
  int a;
  std::vector<mfxFrameSurface1>* pmfxSurfaces;
  MFXVideoSession* session;
  MFXVideoENCODE* mfxENC;
  mfxFrameAllocator* mfxAllocator;
  std::vector<mfxU8>* bstData;
  mfxBitstream* mfxBS;
};

typedef struct MyState UnifexNifState;

typedef UnifexNifState State;

#include "_generated/encoderhw.h"