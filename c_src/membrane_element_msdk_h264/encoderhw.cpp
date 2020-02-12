#include "encoderhw.h"

void handle_destroy_state(UnifexEnv* env, State* state) {
  printf("destory state 1\n"); fflush(stdout);
  /*
  //(*state->mfxENC).Close();
  printf("destory state 2\n"); fflush(stdout);
  //mfxFrameAllocResponse mfxResponse;
  printf("destory state 3\n"); fflush(stdout);
  //(*state->mfxAllocator).Free(state->mfxAllocator->pthis, &mfxResponse);
  printf("destory state 4\n"); fflush(stdout);
  //Release();

  printf("destory state 5\n"); fflush(stdout);
  // Destroy objects in state
  printf("destory state 6\n"); fflush(stdout);
  delete state->pmfxSurfaces;
  printf("destory state 7\n"); fflush(stdout);
  delete state->mfxAllocator;
  printf("destory state 8\n"); fflush(stdout);
  delete state->mfxENC;
  printf("destory state 9\n"); fflush(stdout);
  delete state->mfxBS;
  printf("destory state 10\n"); fflush(stdout);
  delete state->session;
  printf("destory state 11\n"); fflush(stdout);
  */
}

UNIFEX_TERM create(UnifexEnv* env, int frame_width, int frame_height, char* pix_fmt, int bitrate, char* target_usage, int framerate_num, int framerate_denom) {
  State * state = unifex_alloc_state(env);

  printf("create.1\n"); fflush(stdout);
  // Default parameters
  if (framerate_num == 0) {
    framerate_num = 30;
    framerate_denom = 1;
  }

  // Validate parameters
  if (bitrate < 0) {
    std::string msg = "Error: invalid bitrate";
  unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }
  if (frame_width < 1) {
    std::string msg = "Error: invalid width";
  unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }
  if (frame_height < 1) {
    std::string msg = "Error: invalid height";
  unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }
  if (framerate_num < 0) {
    std::string msg = "Error: invalid framerate_num";
  unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }
  if (framerate_denom < 0) {
    std::string msg = "Error: invalid framerate_denom";
    unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }

printf("\n\n\n");
printf(pix_fmt);
printf("\n\n\n");
fflush(stdout);

  printf("create.2\n"); fflush(stdout);
  mfxU16 optionsChromaFormat;
  if (strcmp(pix_fmt, "I420") == 0) {
    optionsChromaFormat = MFX_CHROMAFORMAT_YUV420;
  } else {
    std::string msg = "Error: invalid pix_fmt";
  unifex_release_state(env, state);
    return create_result_error(env, const_cast<char*>(msg.c_str()));
  }

  // Configuration parameters
  mfxU16 optionsBitrate = bitrate;
  mfxU16 optionsFrameRateN = framerate_num;
  mfxU16 optionsFrameRateD = framerate_denom;
  mfxU16 optionsCropX = 0;
  mfxU16 optionsCropY = 0;
  mfxU16 optionsWidth = frame_width;
  mfxU16 optionsHeight = frame_height;
  mfxU16 optionsTargetUsage = StrToTargetUsage(target_usage);

  // TODO: Read from options?
  mfxU16 optionsCodecProfile = MFX_PROFILE_AVC_HIGH; // Hardcoded :high profile

  // Initialize Intel Media SDK session
  // - MFX_IMPL_AUTO_ANY selects HW acceleration if available (on any adapter)
  // - Version 1.0 is selected for greatest backwards compatibility.
  // OS specific notes
  // - On Windows both SW and HW libraries may present
  // - On Linux only HW library only is available
  //   If more recent API features are needed, change the version accordingly
  // https://github.com/Intel-Media-SDK/MediaSDK/blob/9fd26ab972e9f7a2bd74e242827a26bc78330872/api/include/mfxcommon.h#L47
  // mfxIMPL impl = options.values.impl;
  printf("create.3\n"); fflush(stdout);
  mfxIMPL impl = MFX_IMPL_HARDWARE;
  mfxVersion ver = { { 0, 1 } };

  MFXVideoSession& session = *(new MFXVideoSession);
  mfxFrameAllocator& mfxAllocator = *(new mfxFrameAllocator);

  mfxStatus sts = MFX_ERR_NONE;
  sts = Initialize(impl, ver, &session, &mfxAllocator);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  // Create Media SDK encoder
  MFXVideoENCODE& mfxENC = *(new MFXVideoENCODE(session));

  // Set required video parameters for encode
  // - In this example we are encoding an AVC (H.264) stream
  mfxVideoParam mfxEncParams;
  memset(&mfxEncParams, 0, sizeof(mfxEncParams));

  mfxEncParams.mfx.CodecId = MFX_CODEC_AVC;
  //mfxEncParams.mfx.CodecProfile = optionsCodecProfile;
  mfxEncParams.mfx.TargetUsage = optionsTargetUsage;
  mfxEncParams.mfx.TargetKbps = optionsBitrate;
  mfxEncParams.mfx.RateControlMethod = MFX_RATECONTROL_VBR;
  mfxEncParams.mfx.FrameInfo.FrameRateExtN = optionsFrameRateN;
  mfxEncParams.mfx.FrameInfo.FrameRateExtD = optionsFrameRateD;
  mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
  mfxEncParams.mfx.FrameInfo.ChromaFormat = optionsChromaFormat;
  mfxEncParams.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
  mfxEncParams.mfx.FrameInfo.CropX = optionsCropX;
  mfxEncParams.mfx.FrameInfo.CropY = optionsCropY;
  mfxEncParams.mfx.FrameInfo.CropW = optionsWidth;
  mfxEncParams.mfx.FrameInfo.CropH = optionsHeight;
  // Width must be a multiple of 16
  // Height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
  mfxEncParams.mfx.FrameInfo.Width = MSDK_ALIGN16(optionsWidth);
  mfxEncParams.mfx.FrameInfo.Height =
      (MFX_PICSTRUCT_PROGRESSIVE == mfxEncParams.mfx.FrameInfo.PicStruct) ?
      MSDK_ALIGN16(optionsHeight) :
      MSDK_ALIGN32(optionsHeight);

  mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

  // Validate video encode parameters (optional)
  // - In this example the validation result is written to same structure
  // - MFX_WRN_INCOMPATIBLE_VIDEO_PARAM is returned if some of the video parameters are not supported,
  //   instead the encoder will select suitable parameters closest matching the requested configuration
  sts = mfxENC.Query(&mfxEncParams, &mfxEncParams);
  MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }


  printf("create.4\n"); fflush(stdout);
  // Query number of required surfaces for encoder
  mfxFrameAllocRequest EncRequest;
  memset(&EncRequest, 0, sizeof(EncRequest));
  sts = mfxENC.QueryIOSurf(&mfxEncParams, &EncRequest);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  printf("create.5\n"); fflush(stdout);
  // Allocate required surfaces
  mfxFrameAllocResponse mfxResponse;
  sts = mfxAllocator.Alloc(mfxAllocator.pthis, &EncRequest, &mfxResponse);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }
  mfxU16 nEncSurfNum = mfxResponse.NumFrameActual;


  printf("create.6\n"); fflush(stdout);
  // Allocate surface headers (mfxFrameSurface1) for encoder
  std::vector<mfxFrameSurface1>& pmfxSurfaces = *(new std::vector<mfxFrameSurface1>(nEncSurfNum));
  for (int i = 0; i < nEncSurfNum; i++) {
      memset(&pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
      pmfxSurfaces[i].Info = mfxEncParams.mfx.FrameInfo;
      pmfxSurfaces[i].Data.MemId = mfxResponse.mids[i]; // MID (memory id) represent one video NV12 surface
  }

  printf("create.7\n"); fflush(stdout);
  // Initialize the Media SDK encoder
  sts = mfxENC.Init(&mfxEncParams);
  MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  printf("create.8\n"); fflush(stdout);
  // Retrieve video parameters selected by encoder.
  // - BufferSizeInKB parameter is required to set bit stream buffer size
  mfxVideoParam par;
  memset(&par, 0, sizeof(par));
  sts = mfxENC.GetVideoParam(&par);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return create_result_error(env, MSDK_ERR_MSG(sts));
  }

  printf("create.9\n"); fflush(stdout);
  // Prepare Media SDK bit stream buffer
  mfxBitstream& mfxBS = *(new mfxBitstream);
  memset(&mfxBS, 0, sizeof(mfxBS));
  mfxBS.MaxLength = par.mfx.BufferSizeInKB * 1000;
  std::vector<mfxU8>& bstData = *(new std::vector<mfxU8>(mfxBS.MaxLength));
  mfxBS.Data = bstData.data();

  printf("create.10\n"); fflush(stdout);
  // Create NIF state

  state->a = 0;
  state->pmfxSurfaces = &pmfxSurfaces;
  state->mfxAllocator = &mfxAllocator;
  state->mfxENC = &mfxENC;
  state->mfxBS = &mfxBS;
  state->bstData = &bstData;
  state->session = &session;

  printf("create.11\n"); fflush(stdout);
  unifex_release_state(env, state);

  printf("create.12\n"); fflush(stdout);
  return create_result_ok(env, state);
}

mfxStatus WriteBitStreamFrameToPayload(mfxBitstream* pMfxBitstream, UnifexEnv* env, UnifexPayload**& out_frames, int* out_frames_written_ptr, int* out_frames_allocated_ptr)
{
    if (!pMfxBitstream)
       return MFX_ERR_NULL_PTR;

    // Allocate more memory for payload if needed
    // if (*out_frames_written_ptr >= (*out_frames_allocated_ptr)) {
    //   *out_frames_allocated_ptr = *out_frames_allocated_ptr * 2;
    //   printf("  increasing out_frames_allocated = %d\n", *out_frames_allocated_ptr);
    //   **out_frames = (UnifexPayload*) unifex_realloc(**out_frames, (*out_frames_allocated_ptr) * sizeof(**out_frames));
    // }

    out_frames[*out_frames_written_ptr] = unifex_payload_alloc(env, UNIFEX_PAYLOAD_SHM, pMfxBitstream->DataLength);
    memcpy(out_frames[*out_frames_written_ptr]->data, pMfxBitstream->Data + pMfxBitstream->DataOffset, pMfxBitstream->DataLength);

    *out_frames_written_ptr = *out_frames_written_ptr + 1;
    pMfxBitstream->DataLength = 0;

    return MFX_ERR_NONE;
}

UNIFEX_TERM encode(UnifexEnv* env, UnifexPayload * payload, UnifexNifState* state) {
  UNIFEX_TERM response;
  // ===================================
  // Initialize payload
  //

  printf("encode.1\n"); fflush(stdout);
  mfxU32 max_frames = 16;
  int out_frames_written = 0;
  int out_frames_allocated = max_frames;
  int* out_frames_written_ptr = &out_frames_written;
  int* out_frames_allocated_ptr = &out_frames_allocated;
  UnifexPayload** out_frames = (UnifexPayload**) unifex_alloc((*out_frames_allocated_ptr) * sizeof(*out_frames));

  mfxStatus sts = MFX_ERR_NONE;
  int nEncSurfIdx = 0;
  mfxSyncPoint syncp;
  mfxU32 nFrame = 0;

  // Fetch objects from state
  std::vector<mfxFrameSurface1>& pmfxSurfaces = *state->pmfxSurfaces;
  mfxFrameAllocator& mfxAllocator = *state->mfxAllocator;
  MFXVideoENCODE& mfxENC = *state->mfxENC;
  mfxBitstream& mfxBS = *state->mfxBS;
  MFXVideoSession& session = *state->session;

  printf("encode.2\n"); fflush(stdout);
  // ===================================
  // Stage 1: Main encoding loop
  //

  unsigned char* payloadDataPtr = payload->data;
  unsigned char* payloadDataEndPtr = payload->data + payload->size;

  while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts) {
  printf("encode.2.1\n"); fflush(stdout);
      if (nFrame > max_frames) {
        // WARNING: this leaves some frames in payload discraded!
        // Maybe return error?
        // printf("BREAKING ON FRAME %d\n", nFrame); fflush(stdout);
        // break;
  unifex_release_state(env, state);
        std::string msg = "Error: received more thatn " + std::to_string(max_frames) + " frames in a single payload.";
        return encode_result_error(env, const_cast<char*>(msg.c_str()));
      }

  printf("encode.2.2\n"); fflush(stdout);
      nEncSurfIdx = GetFreeSurfaceIndex(pmfxSurfaces);   // Find free frame surface
  printf("encode.2.2.1\n"); fflush(stdout);
      if (nEncSurfIdx == MFX_ERR_NOT_FOUND) {
        printf("encode.2.2.2 Returning error msg\n"); fflush(stdout);
        response = encode_result_error(env, MSDK_ERR_MSG(MFX_ERR_MEMORY_ALLOC));
        unifex_release_state(env, state);
        return response;
        //return encode_result_error(env, "asd");
      }

  printf("encode.2.3\n"); fflush(stdout);
      // Surface locking required when read/write video surfaces
      sts = mfxAllocator.Lock(mfxAllocator.pthis, pmfxSurfaces[nEncSurfIdx].Data.MemId, &(pmfxSurfaces[nEncSurfIdx].Data));
      MSDK_BREAK_ON_ERROR(sts);
  printf("encode.2.4\n"); fflush(stdout);

      sts = LoadRawFrameMemory(&pmfxSurfaces[nEncSurfIdx], payloadDataPtr, payloadDataEndPtr);
      MSDK_BREAK_ON_ERROR(sts);

  printf("encode.2.5\n"); fflush(stdout);
      sts = mfxAllocator.Unlock(mfxAllocator.pthis, pmfxSurfaces[nEncSurfIdx].Data.MemId, &(pmfxSurfaces[nEncSurfIdx].Data));
      MSDK_BREAK_ON_ERROR(sts);

  printf("encode.2.6\n"); fflush(stdout);
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

  printf("encode.2.7\n"); fflush(stdout);
      if (MFX_ERR_NONE == sts) {
          sts = session.SyncOperation(syncp, 60000);      // Synchronize. Wait until encoded frame is ready
          if (sts != MFX_ERR_NONE) {
    unifex_release_state(env, state);
            return encode_result_error(env, MSDK_ERR_MSG(sts));
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, out_frames, out_frames_written_ptr, out_frames_allocated_ptr);
          MSDK_BREAK_ON_ERROR(sts);

          ++nFrame;
          // printf("Frame number: %d\r", nFrame); fflush(stdout);
      }
  }

  printf("encode.3\n"); fflush(stdout);
  // MFX_ERR_MORE_DATA means that the input file has ended, need to go to buffering loop, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
    printf("encode.3 Returning error msg\n"); fflush(stdout);
    response = encode_result_error(env, MSDK_ERR_MSG(sts));
    //response = encode_result_error(env, "asd");
    unifex_release_state(env, state);
    return response;
  }

  printf("encode.3.1\n"); fflush(stdout);
  //
  // Stage 2: Retrieve the buffered encoded frames (what was left in the buffer)
  //
  while (MFX_ERR_NONE <= sts) {
      for (;;) {
  printf("encode.3.2\n"); fflush(stdout);
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

  printf("encode.3.3\n"); fflush(stdout);
      if (MFX_ERR_NONE == sts) {
          sts = session.SyncOperation(syncp, 60000);      // Synchronize. Wait until encoded frame is ready
          if (sts != MFX_ERR_NONE) {
    unifex_release_state(env, state);
            return encode_result_error(env, MSDK_ERR_MSG(sts));
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, out_frames, out_frames_written_ptr, out_frames_allocated_ptr);
          MSDK_BREAK_ON_ERROR(sts);

  printf("encode.3.4\n"); fflush(stdout);
          ++nFrame;
          // printf("Frame number: %d\r", nFrame); fflush(stdout);
      }
  }

  printf("encode.3.5\n"); fflush(stdout);
  //printf("--- All done! out_frames_written = %d\n", *out_frames_written_ptr); fflush(stdout);

  // MFX_ERR_MORE_DATA indicates that there are no more buffered frames, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
    unifex_release_state(env, state);
    return encode_result_error(env, MSDK_ERR_MSG(sts));
  }

  printf("encode.4\n"); fflush(stdout);
  // Prepare return values
  response = encode_result_ok(env, out_frames, out_frames_written);
  for (int i = 0; i < out_frames_written; i++) {
    unifex_payload_release(out_frames[i]);
  }
  if (out_frames != NULL) {
    unifex_free(out_frames);
  }

  return response;
}

UNIFEX_TERM flush(UnifexEnv* env, UnifexNifState* state) {
  // ===================================
  // Initialize payload
  //

  int max_frames = 16;
  int out_frames_written = 0;
  int out_frames_allocated = max_frames;
  int* out_frames_written_ptr = &out_frames_written;
  int* out_frames_allocated_ptr = &out_frames_allocated;
  UnifexPayload** out_frames = (UnifexPayload**) unifex_alloc((*out_frames_allocated_ptr) * sizeof(*out_frames));

  mfxStatus sts = MFX_ERR_NONE;
  mfxSyncPoint syncp;
  mfxU32 nFrame = 0;

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
            unifex_release_state(env, state);
            return flush_result_error(env, MSDK_ERR_MSG(sts));
          }

          sts = WriteBitStreamFrameToPayload(&mfxBS, env, out_frames, out_frames_written_ptr, out_frames_allocated_ptr);
          MSDK_BREAK_ON_ERROR(sts);

          ++nFrame;
          // printf("Frame number: %d\r", nFrame);
          fflush(stdout);
      }
  }

  // printf("--- All done! out_frames_written = %d\n", *out_frames_written_ptr); fflush(stdout);

  // MFX_ERR_MORE_DATA indicates that there are no more buffered frames, exit in case of other errors
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  if (sts != MFX_ERR_NONE) {
  unifex_release_state(env, state);
    return flush_result_error(env, MSDK_ERR_MSG(sts));
  }

  // Prepare return values
  UNIFEX_TERM response = encode_result_ok(env, out_frames, out_frames_written);
  for (int i = 0; i < out_frames_written; i++) {
    unifex_payload_release(out_frames[i]);
  }
  if (out_frames != NULL) {
    unifex_free(out_frames);
  }

  return response;
}
