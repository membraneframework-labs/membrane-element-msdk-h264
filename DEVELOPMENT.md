# Further element development

## To solve in version 0.1.0 before public release

- [ ] Check installation instructions on fresh system
- [ ] Check what happens when hardware-acceleration is not available (should return proper error from `create` method)
- [ ] Add CI (check if hardware-acceleration is available in a container)
- [x] Check how much CPU is used when encoding large files using simple_3_encode_vmem sample
- [x] Craete mix task for encoding files
- [x] Test for memory leaks
  - [x] no memory leaks on encode
  - [x] THER ARE memory leaks on create/delete - nope, only virutal memory
- [x] Fix segfault on infinite pipeline create-run-delete test
- [x] Format code
- [x] Fix `c_str()` usage
- [x] Check what happens when encoidng multiple streams, if we won't hang on syncp 60000
- [x] Hide "libva info"

## More features ideas

- Dynamically alloc space for payload (allow to return any number of frames in single `encode` method call)
- Add decoder and a transcoding test
  - Check if we need Parse module to extrack headers (eg. frame size) for decoder, or we can do this in decoder
- Optimize encoder, it should use less CPU resources
- Support more encoding formats: HEVC, AVC, MPEG-2, JPEG, VP9 
- Support more decoding formats: HEVC, AVC, VP8, VP9, MPEG-2, VC1, JPEG 
- Support video pre-processing filters: Color Conversion, Deinterlace, Denoise, Resize, Rotate, Composition

## Development tips

The encoder based on Intel's vmem_simple_encode sample: https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem

Other tutotrials and sambles in https://github.com/Intel-Media-SDK/MediaSDK can be used as a base for adding more features.

Add more configuraton encoder paremeters, see:
  - docs https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/samples/readme-encode_linux.md
  - code https://github.com/Intel-Media-SDK/MediaSDK/blob/master/samples/sample_encode/src/sample_encode.cpp


## Memory leaks

### In code based on Intel's simple_3_encode_vmem

Tutorial project using video memory for encoding is memory leaking, see:

```
valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./msdk/build/__bin/release/simple_3_encode_vmem -hw -g 176x96 -b 1000 -f 30/1 /home/swm/Downloads/test_stream_176x96.yuv t.h264
```

```
==24230==    by 0x1103CF: simple_alloc(void*, mfxFrameAllocRequest*, mfxFrameAllocResponse*) (common_vaapi.cpp:351)
```

Looks like this code is not working properly:

```
  // Alloc in create function
  sts = mfxAllocator.Alloc(mfxAllocator.pthis, &EncRequest, &mfxResponse);
  // Fre in destroy_state function
  (*state->mfxAllocator).Free((*state->mfxAllocator).pthis, &mfxResponse);
````

### In code based on Intel's simple_3_encode

This example using system memory (no `mfxAllocator` is used) works fine with no memory leaks.

It's slower by a very small margin than simple_3_encode_vmem.
