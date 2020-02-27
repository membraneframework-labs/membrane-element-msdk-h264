defmodule Membrane.Element.Msdk.H264.EncoderVmem.Native do
  @moduledoc """
  Native module responsible for calling the C++ encoder implementation.

  Based on [https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem](https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode_vmem)

  Warning! This implementation has an unsolved memory leak, see: [https://github.com/Intel-Media-SDK/MediaSDK/issues/1984](https://github.com/Intel-Media-SDK/MediaSDK/issues/1984)
  """
  use Unifex.Loader
end
