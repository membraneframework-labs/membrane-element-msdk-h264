# Membrane Multimedia Framework: IntelMediaSDK H264 Element

[![CircleCI](https://circleci.com/gh/membraneframework/membrane-element-msdk-h264.svg?style=svg)](https://circleci.com/gh/membraneframework/membrane-element-MSDK-h264)

This package provides [Membrane Multimedia Framework](https://membraneframework.org)
element that can be used to encode video streams using [IntelMediaSDK](https://www.IntelMediaSDK.org)
and [x264](https://www.videolan.org/developers/x264.html).

Documentation is available at [HexDocs](https://hexdocs.pm/membrane_element_msdk_h264/)

## Installation

Follow the Intel's guide to get the runtime and development environment installed:
https://github.com/Intel-Media-SDK/MediaSDK/wiki/Intel-media-stack-on-Ubuntu

Add the following line to your `deps` in `mix.exs`. Run `mix deps.get`.

```elixir
{:membrane_element_msdk_h264, "~> 0.2.0"}
```

```bash
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev
```

## Supported platforms

Linux x86-64, tested and docs provided for Ubuntu.

Will work on all Intel processors with graphical chipsets - basically any Intel processor that is not 10 years old.

For details check: https://github.com/intel/media-driver#supported-platformsk

## Benchmarks

System: Ubuntu Intel i7-6650U CPU @ 2.20GHz (2 cores / 4 threads).

Sample: Big_Buck_Bunny_1080_10s_30MB.mp4 converted to raw format (890MB) (https://test-videos.co.uk/bigbuckbunny/mp4-h264)

Bitrate: 6000 Kbps.

Encoder | Time | CPU usage
------- | ---- | ---------
Membrane MSDK | 20s | 400%
Membrane FFMpeg | 5s | 250%

## Future development

The encoder based on Inte's vmem_simple_encode sample: https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem

Files in `membrane-element-msdk-h264/c_src/membrane_element_msdk_h264/msdk` are copied from https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/common and not modified.

Add more configuraton encoder paremeters, see:
  * docs https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/samples/readme-encode_linux.md
  * code https://github.com/Intel-Media-SDK/MediaSDK/blob/master/samples/sample_encode/src/sample_encode.cpp

Support more encoding formats: HEVC, AVC, MPEG-2, JPEG, VP9 
Support more decoding formats: HEVC, AVC, VP8, VP9, MPEG-2, VC1, JPEG 
Support video pre-processing filters: Color Conversion, Deinterlace, Denoise, Resize, Rotate, Composition


## Copyright and License

Copyright 2020, [Software Mansion](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)

[![Software Mansion](https://membraneframework.github.io/static/logo/swm_logo_readme.png)](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)
