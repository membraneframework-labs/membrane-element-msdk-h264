#pragma once

#include "msdk/common_utils.h"
#include "msdk_membrane/membrane_utils.h"

struct MyState {
  std::vector<mfxFrameSurface1> *pEncSurfaces;
  std::vector<mfxU8> *surfaceBuffersData;
  std::vector<mfxU8> *bstData;
  MFXVideoENCODE *mfxENC;
  mfxBitstream *mfxBS;
  MFXVideoSession *session;
};

typedef struct MyState UnifexNifState;

typedef UnifexNifState State;

#include "_generated/encodersysmem.h"