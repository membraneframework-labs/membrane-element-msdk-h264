#pragma once

#include "mfxstructures.h"
#include <map>
#include <memory>
#include <string.h>
#include <string>

mfxU16 StrToTargetUsage(std::string strInput);
size_t MemoryRead(unsigned char *destPtr, int count, size_t size,
                  unsigned char *&srcPtr, unsigned char *srcPtrEnd);
mfxStatus ReadPlaneDataMemory(mfxU16 w, mfxU16 h, mfxU8 *buf, mfxU8 *ptr,
                              mfxU16 pitch, mfxU16 offset,
                              unsigned char *&payloadData,
                              unsigned char *payloadDataEnd);
mfxStatus LoadRawFrameMemory(mfxFrameSurface1 *pSurface,
                             unsigned char *&payloadData,
                             unsigned char *payloadDataEnd);

std::string ErrString(int err, const char *filestr, int line);

#define MSDK_ERR_MSG(ERR)                                                      \
  { const_cast<char *>(ErrString(ERR, __FILE__, __LINE__).c_str()) }
#define MSDK_C_STR(STR)                                                        \
  { const_cast<char *>(std::string(STR).c_str()) }
