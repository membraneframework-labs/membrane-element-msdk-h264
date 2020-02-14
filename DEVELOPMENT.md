# Further element development

## To solve in version 0.1.0 before public release

- [ ] Dynamically alloc space for payload (allow to return any number of frames in single `encode` method call)
- [ ] Check installation instructions on fresh system
- [ ] Check what happens when hardware-acceleration is not available (should return proper error from `create` method)
- [ ] Add CI (check if hardware-acceleration is available in a container)
- [ ] Check if we need Parse module to extrack headers (eg. frame size) for decoder, or we can do this in decoder
- [ ] Check how much CPU is used when encoding large files using simple_3_encode_vmem sample

## More features ideas

- Add decoder and a transcoding test
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

