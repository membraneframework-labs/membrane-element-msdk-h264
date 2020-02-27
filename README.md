# Membrane Multimedia Framework: IntelMediaSDK H264 Element

[![CircleCI](https://circleci.com/gh/membraneframework/membrane-element-msdk-h264.svg?style=svg)](https://circleci.com/gh/membraneframework/membrane-element-MSDK-h264)

This package provides [Membrane Multimedia Framework](https://membraneframework.org)
element that can be used to encode video streams using [IntelMediaSDK](https://www.IntelMediaSDK.org)
and [x264](https://www.videolan.org/developers/x264.html).

Documentation is available at [HexDocs](https://hexdocs.pm/membrane_element_msdk_h264/)

## Installation

Add the following line to your `deps` in `mix.exs`. Run `mix deps.get`.

```elixir
{:membrane_element_msdk_h264, "~> 0.1.0"}
```

```bash
sudo apt-get install libmfx1 libmfx-tools libva-dev libmfx-dev libavcodec-dev libavformat-dev libavutil-dev
```

## Supported platforms

Linux x86-64, tested and docs provided for Ubuntu.

Will work on all Intel processors with graphical chipsets - basically any Intel processor that is not 10 years old.

For details check: https://github.com/intel/media-driver#supported-platformsk

## Benchmarks

System: Ubuntu Intel i7-6650U CPU @ 2.20GHz (2 cores / 4 threads).

Sample: [Big_Buck_Bunny_1080_10s_30MB.mp4](https://test-videos.co.uk/bigbuckbunny/mp4-h264converted) to raw format (890MB)

Bitrate: 6000 Kbps.

Encoder | Time | CPU usage
------- | ---- | ---------
[Intel's simple_3_encode_vmem](https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem) | 5s | 70%
[Membrane Element MSDK h264](https://github.com/membraneframework/membrane-element-msdk-h264) | 5s | 300%
[Membrane Element FFMpeg h264](https://github.com/membraneframework/membrane-element-ffmpeg-h264) | 20s | 400% (CPU bottleneck)

## Copyright and License

Copyright 2020, [Software Mansion](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)

[![Software Mansion](https://membraneframework.github.io/static/logo/swm_logo_readme.png)](https://swmansion.com/?utm_source=git&utm_medium=readme&utm_campaign=membrane)
