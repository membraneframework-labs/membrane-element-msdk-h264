# Membrane Multimedia Framework: IntelMediaSDK H264 Element

# TODO: to process

* Installing on Ubuntu: https://github.com/Intel-Media-SDK/MediaSDK/wiki/Intel-media-stack-on-Ubuntu
* Check available options for encoding
  * docs https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/samples/readme-encode_linux.md
  * code https://github.com/Intel-Media-SDK/MediaSDK/blob/master/samples/sample_encode/src/sample_encode.cpp


  // Additional configuration parameters to consider
  // See: https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/samples/readme-encode_linux.md
  // And: https://github.com/Intel-Media-SDK/MediaSDK/blob/master/samples/sample_encode/src/sample_encode.cpp
  //
  // [-r distance] Distance between I- or P- key frames (1 means no B-frames)
  //
  // [-vbr] [-cbr] [-qvbr] [-icq] variable bitrate options
  //
  // [-CodecProfile] [-CodecLevel]


# Benchmarks

Ubuntu Intel(R) Core(TM) i7-6650U CPU @ 2.20GHz (2 cores / 4 threads)

Big_Buck_Bunny_1080_10s_30MB.mp4
converted to
Big_Buck_Bunny_1080_10s_30MB.yuv (890MB)

Converting with bitrate of 6000 Kbps.

Software encoder element:
20s of encoding
beam CPUs usage at 400%

Hardware encoder element:
5s of encoding
beam CPUs usage at 250%

# Future development

Is based on tutorial code for vmem_simple_encode: https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem

Files in `membrane-element-msdk-h264/c_src/membrane_element_msdk_h264/msdk` are copied from https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/common and not modified.

More advanced features can be added. See examples: https://github.com/Intel-Media-SDK/MediaSDK/tree/master/samples

[![CircleCI](https://circleci.com/gh/membraneframework/membrane-element-msdk-h264.svg?style=svg)](https://circleci.com/gh/membraneframework/membrane-element-MSDK-h264)

This package provides [Membrane Multimedia Framework](https://membraneframework.org)
elements that can be used to encode, parse and decode H264 video streams using [IntelMediaSDK](https://www.IntelMediaSDK.org)
and [x264](https://www.videolan.org/developers/x264.html)

Documentation is available at [HexDocs](https://hexdocs.pm/membrane_element_msdk_h264/)


## Installation

Add the following line to your `deps` in `mix.exs`. Run `mix deps.get`.

```elixir
{:membrane_element_MSDK_h264, "~> 0.2.0"}
```

You also need to have [IntelMediaSDK](https://www.IntelMediaSDK.org) libraries installed in your system.


### Ubuntu

```bash
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev
```

### Arch/Manjaro

```bash
pacman -S IntelMediaSDK
```

### MacOS

```bash
brew install IntelMediaSDK
```

## Copyright and License

Copyright 2020, [Software Mansion](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)

[![Software Mansion](https://membraneframework.github.io/static/logo/swm_logo_readme.png)](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)
