defmodule Membrane.Element.Msdk.H264.EncoderSysmem.Native do
  @moduledoc """
  Native module responsible for calling the C++ encoder implementation.

  Based on [https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode](https://github.com/Intel-Media-SDK/MediaSDK/tree/master/tutorials/simple_3_encode)
  """
  use Unifex.Loader
end
