#include "membrane_utils.h"

mfxU16 StrToTargetUsage(std::string strInput) {
  std::map<std::string, mfxU16> tu;
  tu["quality"] = (mfxU16)MFX_TARGETUSAGE_1;
  tu["veryslow"] = (mfxU16)MFX_TARGETUSAGE_1;
  tu["slower"] = (mfxU16)MFX_TARGETUSAGE_2;
  tu["slow"] = (mfxU16)MFX_TARGETUSAGE_3;
  tu["medium"] = (mfxU16)MFX_TARGETUSAGE_4;
  tu["balanced"] = (mfxU16)MFX_TARGETUSAGE_4;
  tu["fast"] = (mfxU16)MFX_TARGETUSAGE_5;
  tu["faster"] = (mfxU16)MFX_TARGETUSAGE_6;
  tu["veryfast"] = (mfxU16)MFX_TARGETUSAGE_7;
  tu["speed"] = (mfxU16)MFX_TARGETUSAGE_7;
  tu["1"] = (mfxU16)MFX_TARGETUSAGE_1;
  tu["2"] = (mfxU16)MFX_TARGETUSAGE_2;
  tu["3"] = (mfxU16)MFX_TARGETUSAGE_3;
  tu["4"] = (mfxU16)MFX_TARGETUSAGE_4;
  tu["5"] = (mfxU16)MFX_TARGETUSAGE_5;
  tu["6"] = (mfxU16)MFX_TARGETUSAGE_6;
  tu["7"] = (mfxU16)MFX_TARGETUSAGE_7;

  if (tu.find(strInput) == tu.end())
    return 0;
  else
    return tu[strInput];
}

size_t MemoryRead(unsigned char *destPtr, int count, size_t size,
                  unsigned char *&srcPtr, unsigned char *srcPtrEnd) {
  size_t bytesToRead = count * size;
  size_t bytesLeft = srcPtrEnd - srcPtr;
  if (bytesToRead > bytesLeft) {
    bytesToRead = bytesLeft;
  }
  memcpy(destPtr, srcPtr, bytesToRead);
  srcPtr = srcPtr + bytesToRead;

  return bytesToRead;
}

mfxStatus ReadPlaneDataMemory(mfxU16 w, mfxU16 h, mfxU8 *buf, mfxU8 *ptr,
                              mfxU16 pitch, mfxU16 offset,
                              unsigned char *&payloadData,
                              unsigned char *payloadDataEnd) {
  mfxU32 nBytesRead;
  for (mfxU16 i = 0; i < h; i++) {
    nBytesRead = (mfxU32)MemoryRead(buf, 1, w, payloadData, payloadDataEnd);
    if (w != nBytesRead)
      return MFX_ERR_MORE_DATA;
    for (mfxU16 j = 0; j < w; j++)
      ptr[i * pitch + j * 2 + offset] = buf[j];
  }
  return MFX_ERR_NONE;
}

mfxStatus LoadRawFrameMemory(mfxFrameSurface1 *pSurface,
                             unsigned char *&payloadData,
                             unsigned char *payloadDataEnd) {
  mfxStatus sts = MFX_ERR_NONE;
  mfxU32 nBytesRead;
  mfxU16 w, h, i, pitch;
  mfxU8 *ptr;
  mfxFrameInfo *pInfo = &pSurface->Info;
  mfxFrameData *pData = &pSurface->Data;

  if (pInfo->CropH > 0 && pInfo->CropW > 0) {
    w = pInfo->CropW;
    h = pInfo->CropH;
  } else {
    w = pInfo->Width;
    h = pInfo->Height;
  }

  pitch = pData->Pitch;
  ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;

  // read luminance plane
  for (i = 0; i < h; i++) {
    // nBytesRead = (mfxU32) fread(ptr + i * pitch, 1, w, fSource);
    nBytesRead =
        (mfxU32)MemoryRead(ptr + i * pitch, 1, w, payloadData, payloadDataEnd);
    if (w != nBytesRead)
      return MFX_ERR_MORE_DATA;
  }

  mfxU8 buf[2048]; // maximum supported chroma width for nv12
  w /= 2;
  h /= 2;
  ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;
  if (w > 2048)
    return MFX_ERR_UNSUPPORTED;

  // load U
  sts = ReadPlaneDataMemory(w, h, buf, ptr, pitch, 0, payloadData,
                            payloadDataEnd);
  if (MFX_ERR_NONE != sts)
    return sts;
  // load V
  ReadPlaneDataMemory(w, h, buf, ptr, pitch, 1, payloadData, payloadDataEnd);
  if (MFX_ERR_NONE != sts)
    return sts;

  return MFX_ERR_NONE;
}

std::string ErrString(int err, const char *filestr, int line) {
  char message[256];

  switch (err) {
  case 0:
    sprintf(message, "No error.");
    break;
  case -1:
    sprintf(message, "Unknown error: %s %d", filestr, line);
    break;
  case -2:
    sprintf(message, "Null pointer.  Check filename/path + permissions? %s %d",
            filestr, line);
    break;
  case -3:
    sprintf(message, "Unsupported feature/library load error. %s %d", filestr,
            line);
    break;
  case -4:
    sprintf(message, "Could not allocate memory. %s %d", filestr, line);
    break;
  case -5:
    sprintf(message, "Insufficient IO buffers. %s %d", filestr, line);
    break;
  case -6:
    sprintf(message, "Invalid handle. %s %d", filestr, line);
    break;
  case -7:
    sprintf(message, "Memory lock failure. %s %d", filestr, line);
    break;
  case -8:
    sprintf(message, "Function called before initialization. %s %d", filestr,
            line);
    break;
  case -9:
    sprintf(message, "Specified object not found. %s %d", filestr, line);
    break;
  case -10:
    sprintf(message, "More input data expected. %s %d", filestr, line);
    break;
  case -11:
    sprintf(message, "More output surfaces expected. %s %d", filestr, line);
    break;
  case -12:
    sprintf(message, "Operation aborted. %s %d", filestr, line);
    break;
  case -13:
    sprintf(message, "HW device lost. %s %d", filestr, line);
    break;
  case -14:
    sprintf(message, "Incompatible video parameters. %s %d", filestr, line);
    break;
  case -15:
    sprintf(message, "Invalid video parameters. %s %d", filestr, line);
    break;
  case -16:
    sprintf(message, "Undefined behavior. %s %d", filestr, line);
    break;
  case -17:
    sprintf(message, "Device operation failure. %s %d", filestr, line);
    break;
  case -18:
    sprintf(message, "More bitstream data expected. %s %d", filestr, line);
    break;
  case -19:
    sprintf(message, "Incompatible audio parameters. %s %d", filestr, line);
    break;
  case -20:
    sprintf(message, "Invalid audio parameters. %s %d", filestr, line);
    break;
  default:
    sprintf(message, "Error code %d,\t%s\t%d", err, filestr, line);
  }

  return message;
}
