#include "decoder.h"

const mfxU32 MAX_FRAMES_IN_PAYLOAD = 16;

void handle_destroy_state(UnifexEnv* env, State* state) {
  // Clean up resources
  //  - It is recommended to close Media SDK components first, before releasing allocated surfaces, since
  //    some surfaces may still be locked by internal Media SDK resources.
  (*state->mfxENC).Close();
  mfxFrameAllocResponse mfxResponse;
  (*state->mfxAllocator).Free((*state->mfxAllocator).pthis, &mfxResponse);
  Release();

  // Destroy objects in state
  delete state->pmfxSurfaces;
  delete state->mfxAllocator;
  delete state->mfxENC;
  delete state->mfxBS;
  delete state->session;
}

UNIFEX_TERM create(UnifexEnv* env, int frame_width, int frame_height, char* pix_fmt) {
  UNIFEX_TERM result;

  // Validate parameters
  if (frame_width < 1) {
    return create_result_error(env, MSDK_C_STR("Error: invalid width"));
  }
  if (frame_height < 1) {
    return create_result_error(env, MSDK_C_STR("Error: invalid height"));
  }
  mfxU16 optionsChromaFormat;
  if (strcmp(pix_fmt, "I420") == 0) {
    optionsChromaFormat = MFX_CHROMAFORMAT_YUV420;
  } else if (strcmp(pix_fmt, "I422") == 0) {
    optionsChromaFormat = MFX_CHROMAFORMAT_YUV422;
  } else {
    return create_result_error(env, MSDK_C_STR("Error: invalid pix_fmt"));
  }










  // Initialize Intel Media SDK session
  // - MFX_IMPL_AUTO_ANY selects HW acceleration if available (on any adapter)
  // - Version 1.0 is selected for greatest backwards compatibility.
  // OS specific notes
  // - On Windows both SW and HW libraries may present
  // - On Linux only HW library only is available
  //   If more recent API features are needed, change the version accordingly
  // https://github.com/Intel-Media-SDK/MediaSDK/blob/9fd26ab972e9f7a2bd74e242827a26bc78330872/api/include/mfxcommon.h#L47
  // mfxIMPL impl = options.values.impl;
  mfxIMPL impl = MFX_IMPL_HARDWARE;
  mfxVersion ver = { { 0, 1 } };
  MFXVideoSession& session = *(new MFXVideoSession);
  mfxFrameAllocator& mfxAllocator = *(new mfxFrameAllocator);

  mfxStatus sts = MFX_ERR_NONE;
  sts = Initialize(impl, ver, &session, &mfxAllocator);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }
 
  // Create Media SDK decoder
  MFXVideoDECODE& mfxDEC = *(new MFXVideoDECODE(session));

  // Set required video parameters for decode
  mfxVideoParam mfxEncParams;
  memset(&mfxEncParams, 0, sizeof(mfxEncParams));
  mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;
  mfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;


    // Prepare Media SDK bit stream buffer
    // - Arbitrary buffer size for this example
  mfxBitstream& mfxBS = *(new mfxBitstream);
  memset(&mfxBS, 0, sizeof(mfxBS));
  mfxBS.MaxLength = 1024 * 1024;
  std::vector<mfxU8>& bstData = *(new std::vector<mfxU8>(mfxBS.MaxLength));
  mfxBS.Data = bstData.data();



// TODO: do we want to decode header, or provice params??
    sts = ReadBitStreamDataMemory(&mfxBS, fSource.get());
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = mfxDEC.DecodeHeader(&mfxBS, &mfxVideoParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);









  // Validate video encode parameters (optional)
  // - In this example the validation result is written to same structure
  // - MFX_WRN_INCOMPATIBLE_VIDEO_PARAM is returned if some of the video parameters are not supported,
  //   instead the encoder will select suitable parameters closest matching the requested configuration
  sts = mfxENC.Query(&mfxEncParams, &mfxEncParams);
  MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    delete &mfxENC;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  // Query number of required surfaces for encoder
  mfxFrameAllocRequest EncRequest;
  memset(&EncRequest, 0, sizeof(EncRequest));
  sts = mfxENC.QueryIOSurf(&mfxEncParams, &EncRequest);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    delete &mfxENC;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  // Allocate required surfaces
  mfxFrameAllocResponse mfxResponse;

  sts = mfxAllocator.Alloc(mfxAllocator.pthis, &EncRequest, &mfxResponse);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    delete &mfxENC;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }
  mfxU16 nEncSurfNum = mfxResponse.NumFrameActual;

  // Allocate surface headers (mfxFrameSurface1) for encoder
  std::vector<mfxFrameSurface1>& pmfxSurfaces = *(new std::vector<mfxFrameSurface1>(nEncSurfNum));
  for (int i = 0; i < nEncSurfNum; i++) {
      memset(&pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
      pmfxSurfaces[i].Info = mfxEncParams.mfx.FrameInfo;
      pmfxSurfaces[i].Data.MemId = mfxResponse.mids[i]; // MID (memory id) represent one video NV12 surface
  }

  // Initialize the Media SDK encoder
  sts = mfxENC.Init(&mfxEncParams);
  MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    delete &mfxENC;
    delete &pmfxSurfaces;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  // Retrieve video parameters selected by encoder.
  // - BufferSizeInKB parameter is required to set bit stream buffer size
  mfxVideoParam par;
  memset(&par, 0, sizeof(par));
  sts = mfxENC.GetVideoParam(&par);
  if (sts != MFX_ERR_NONE) {
    delete &session;
    delete &mfxAllocator;
    delete &mfxENC;
    delete &pmfxSurfaces;
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }
  // Create NIF state
  State * state = unifex_alloc_state(env);
  state->pmfxSurfaces = &pmfxSurfaces;
  state->mfxAllocator = &mfxAllocator;
  state->mfxENC = &mfxENC;
  state->mfxBS = &mfxBS;
  state->bstData = &bstData;
  state->session = &session;

  result = create_result_ok(env, state);

  unifex_release_state(env, state);
  return result;
}

mfxStatus WriteBitStreamFrameToPayload(mfxBitstream* pMfxBitstream, UnifexEnv* env, UnifexPayload**& outFrames, int* outFramesWrittenPtr, int* outFramesAllocatedPtr)
{
    if (!pMfxBitstream)
       return MFX_ERR_NULL_PTR;

    // realloc if needed
    // if (*outFramesWrittenPtr >= (*outFramesAllocatedPtr)) {
    //   *outFramesAllocatedPtr = *outFramesAllocatedPtr * 2;
    //   printf("  increasing outFramesAllocated = %d\n", *outFramesAllocatedPtr);
    //   **outFrames = (UnifexPayload*) unifex_realloc(**outFrames, (*outFramesAllocatedPtr) * sizeof(**outFrames));
    // }

    outFrames[*outFramesWrittenPtr] = unifex_payload_alloc(env, UNIFEX_PAYLOAD_SHM, pMfxBitstream->DataLength);
    memcpy(outFrames[*outFramesWrittenPtr]->data, pMfxBitstream->Data + pMfxBitstream->DataOffset, pMfxBitstream->DataLength);

    *outFramesWrittenPtr = *outFramesWrittenPtr + 1;
    pMfxBitstream->DataLength = 0;

    return MFX_ERR_NONE;
}

UNIFEX_TERM encode(UnifexEnv* env, UnifexPayload * payload, UnifexNifState* state) {
  UNIFEX_TERM response;

  // ===================================
  // Initialize payload
  //
  int outFramesWritten = 0;
  int outFramesAllocated = MAX_FRAMES_IN_PAYLOAD;
  int* outFramesWrittenPtr = &outFramesWritten;
  int* outFramesAllocatedPtr = &outFramesAllocated;
  UnifexPayload** outFrames = (UnifexPayload**) unifex_alloc((*outFramesAllocatedPtr) * sizeof(*outFrames));

  // ===================================
  // Start encoding the frames
  //

  mfxStatus sts = MFX_ERR_NONE;
  int nEncSurfIdx = 0;
  mfxSyncPoint syncp;
  mfxU32 nFrame = 0;

  std::vector<mfxFrameSurface1>& pmfxSurfaces = *state->pmfxSurfaces;
  mfxFrameAllocator& mfxAllocator = *state->mfxAllocator;
  MFXVideoENCODE& mfxENC = *state->mfxENC;
  mfxBitstream& mfxBS = *state->mfxBS;
  MFXVideoSession& session = *state->session;
  unsigned char* payloadDataPtr = payload->data;
  unsigned char* payloadDataEndPtr = payload->data + payload->size;

  // ===================================
  // Stage 1: Main encoding loop
  //

  while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts) {
      if (nFrame > MAX_FRAMES_IN_PAYLOAD) {
        response = encode_result_error(env, MSDK_C_STR("Error: received more than " + std::to_string(MAX_FRAMES_IN_PAYLOAD) + " frames in a single payload."));
        goto exit_encode;
      }

      nEncSurfIdx = GetFreeSurfaceIndex(pmfxSurfaces);   // Find free frame surface
      if (nEncSurfIdx == MFX_ERR_NOT_FOUND) {
        response = encode_result_error(env, MSDK_ERR_MSG(MFX_ERR_MEMORY_ALLOC));
        goto exit_encode;
      }

      // Surface locking required when read/write video surfaces
      sts = mfxAllocator.Lock(mfxAllocator.pthis, pmfxSurfaces[nEncSurfIdx].Data.MemId, &(pmfxSurfaces[nEncSurfIdx].Data));
      MSDK_BREAK_ON_ERROR(sts);

      sts = LoadRawFrameMemory(&pmfxSurfaces[nEncSurfIdx], payloadDataPtr, payloadDataEndPtr);
      MSDK_BREAK_ON_ERROR(sts);

      sts = mfxAllocator.Unlock(mfxAllocator.pthis, pmfxSurfaces[nEncSurfIdx].Data.MemId, &(pmfxSurfaces[nEncSurfIdx].Data));
      MSDK_BREAK_ON_ERROR(sts);

      for (;;) {
          // Encode a frame asychronously (returns immediately)
          sts = mfxENC.EncodeFrameAsync(NULL, &pmfxSurfaces[nEncSurfIdx], &mfxBS, &syncp);

          if (MFX_ERR_NONE < sts && !syncp) {     // Repeat the call if warning and no output
              if (MFX_WRN_DEVICE_BUSY == sts)
                  MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
          } else if (MFX_ERR_NONE < sts && syncp) {
              sts = MFX_ERR_NONE;     // Ignore warnings if output is available
              break;
          } else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts) {
              // Allocate more bitstream buffer memory here if needed...
              break;
          } else
              break;
      }

      if (MFX_ERR_NONE == sts) {
          sts = session.SyncOperation(syncp, 60000);      // Synchronize. Wait until encoded frame is ready
          if (sts != MFX_ERR_NONE) {
            response = encode_result_error(env, MSDK_ERR_MSG(sts));
            goto exit_encode;
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, outFrames, outFramesWrittenPtr, outFramesAllocatedPtr);

          MSDK_BREAK_ON_ERROR(sts);

          ++nFrame;
          //printf("Frame number: %d\r", nFrame);
          //fflush(stdout);
      }
  }

  // MFX_ERR_MORE_DATA means that the input file has ended, need to go to buffering loop, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
    response = encode_result_error(env, MSDK_ERR_MSG(sts));
    goto exit_encode;
  }

  //
  // Stage 2: Retrieve the buffered encoded frames (what was left in the buffer)
  //
  while (MFX_ERR_NONE <= sts) {
      for (;;) {
          // Encode a frame asychronously (returns immediately)
          sts = mfxENC.EncodeFrameAsync(NULL, NULL, &mfxBS, &syncp);

          if (MFX_ERR_NONE < sts && !syncp) {     // Repeat the call if warning and no output
              if (MFX_WRN_DEVICE_BUSY == sts)
                  MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
          } else if (MFX_ERR_NONE < sts && syncp) {
              sts = MFX_ERR_NONE;     // Ignore warnings if output is available
              break;
          } else
              break;
      }

      if (MFX_ERR_NONE == sts) {
          sts = session.SyncOperation(syncp, 60000);      // Synchronize. Wait until encoded frame is ready
          if (sts != MFX_ERR_NONE) {
            response = encode_result_error(env, MSDK_ERR_MSG(sts));
            goto exit_encode;
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, outFrames, outFramesWrittenPtr, outFramesAllocatedPtr);
          MSDK_BREAK_ON_ERROR(sts);

          ++nFrame;
          //printf("Frame number: %d\r", nFrame);
          //fflush(stdout);
      }
  }

  //printf("--- All done! outFramesWritten = %d\n", *outFramesWrittenPtr); fflush(stdout);

  // MFX_ERR_MORE_DATA indicates that there are no more buffered frames, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
    response = encode_result_error(env, MSDK_ERR_MSG(sts));
    goto exit_encode;
  }

  // Prepare return values
  response = encode_result_ok(env, outFrames, outFramesWritten);

exit_encode:
  for (int i = 0; i < outFramesWritten; i++) {
    unifex_payload_release(outFrames[i]);
  }
  if (outFrames != NULL) {
    unifex_free(outFrames);
  }
  return response;
}

UNIFEX_TERM flush(UnifexEnv* env, UnifexNifState* state) {
  UNIFEX_TERM response;

  // ===================================
  // Initialize payload
  //

  int outFramesWritten = 0;
  int outFramesAllocated = MAX_FRAMES_IN_PAYLOAD;
  int* outFramesWrittenPtr = &outFramesWritten;
  int* outFramesAllocatedPtr = &outFramesAllocated;
  UnifexPayload** outFrames = (UnifexPayload**) unifex_alloc((*outFramesAllocatedPtr) * sizeof(*outFrames));

  // ===================================
  // Start encoding the frames
  //

  mfxStatus sts = MFX_ERR_NONE;
  //int nEncSurfIdx = 0;
  mfxSyncPoint syncp;
  mfxU32 nFrame = 0;

  //std::vector<mfxFrameSurface1>& pmfxSurfaces = *state->pmfxSurfaces;
  //mfxFrameAllocator& mfxAllocator = *state->mfxAllocator;
  MFXVideoENCODE& mfxENC = *state->mfxENC;
  mfxBitstream& mfxBS = *state->mfxBS;
  MFXVideoSession& session = *state->session;

  //
  // Stage 2: Retrieve the buffered encoded frames (what was left in the buffer)
  //
  while (MFX_ERR_NONE <= sts) {
      for (;;) {
          // Encode a frame asychronously (returns immediately)
          sts = mfxENC.EncodeFrameAsync(NULL, NULL, &mfxBS, &syncp);

          if (MFX_ERR_NONE < sts && !syncp) {     // Repeat the call if warning and no output
              if (MFX_WRN_DEVICE_BUSY == sts)
                  MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
          } else if (MFX_ERR_NONE < sts && syncp) {
              sts = MFX_ERR_NONE;     // Ignore warnings if output is available
              break;
          } else
              break;
      }

      if (MFX_ERR_NONE == sts) {
          sts = session.SyncOperation(syncp, 60000);      // Synchronize. Wait until encoded frame is ready
          if (sts != MFX_ERR_NONE) {
            response = flush_result_error(env, MSDK_ERR_MSG(sts));
            goto exit_flush;
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, outFrames, outFramesWrittenPtr, outFramesAllocatedPtr);
          MSDK_BREAK_ON_ERROR(sts);

          ++nFrame;
          // printf("Frame number: %d\r", nFrame);
          // fflush(stdout);
      }
  }

  //printf("--- All done! outFramesWritten = %d\n", *outFramesWrittenPtr); fflush(stdout);

  // MFX_ERR_MORE_DATA indicates that there are no more buffered frames, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
    response = encode_result_error(env, MSDK_ERR_MSG(sts));
    goto exit_flush;
  }

  // Prepare return values
  response = encode_result_ok(env, outFrames, outFramesWritten);

exit_flush:
  for (int i = 0; i < outFramesWritten; i++) {
    unifex_payload_release(outFrames[i]);
  }
  if (outFrames != NULL) {
    unifex_free(outFrames);
  }

  return response;
}