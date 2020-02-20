module Membrane.Element.Msdk.H264.Encoder.Native

spec create(
       frame_width :: int,
       frame_height :: int,
       pix_fmt :: atom,
       bitrate :: int,
       usage_rate :: atom,
       framerate_num :: int,
       framerate_denom :: int
     ) :: {:ok :: label, state} | {:error :: label, reason :: atom}

spec encode(payload, state) :: {:ok :: label, [payload]} | {:error :: label, reason :: atom}

spec flush(state) :: {:ok :: label, frames :: [payload]} | {:error :: label, reason :: atom}

dirty :cpu, encode: 2, flush: 1
