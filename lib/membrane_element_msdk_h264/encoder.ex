defmodule Membrane.Element.Msdk.H264.Encoder do
  @moduledoc """
  Membrane element that encodes raw video frames to H264 format using
  hardware-accelerated API available for Intel® Gen graphics hardware platforms.

  The element expects each frame to be received in a separate buffer, so the parser
  (`Membrane.Element.RawVideo.Parser`) may be required in a pipeline before
  the encoder (e.g. when input is read from `Membrane.Element.File.Source`).

  Additionaly, the encoder has to receive proper caps with picture format and dimensions
  before any encoding takes place.

  Please check `t:t/0` for available options.
  """
  use Membrane.Filter
  alias Membrane.Element.Msdk.H264.EncoderSysmem.Native
  alias Membrane.Buffer
  alias Membrane.Caps.Video.{H264, Raw}
  use Bunch
  use Bunch.Typespec

  def_input_pad :input,
    demand_unit: :buffers,
    caps: {Raw, format: one_of([:I420, :I422]), aligned: true}

  def_output_pad :output,
    caps: {H264, stream_format: :byte_stream, alignment: :au}

  @h264_profile :high
  @default_bitrate 1_000
  @default_target_usage :medium

  @list_type target_usages :: [
               :quality,
               :veryslow,
               :slower,
               :slow,
               :medium,
               :balanced,
               :fast,
               :faster,
               :veryfast,
               :speed
             ]

  def_options bitrate: [
                description: """
                Encoded bit rate (Kbits per second).
                """,
                type: :int,
                default: @default_bitrate
              ],
              target_usage: [
                description: """
                TargetUsage (TU) is similar to the "preset" concept in FFmpeg.
                It provides a quick way to change quality/speed tradeoffs in codec settings.
                """,
                type: :atom,
                spec: target_usages(),
                default: @default_target_usage
              ]

  @impl true
  def handle_init(opts) do
    {:ok, opts |> Map.merge(%{encoder_ref: nil})}
  end

  @impl true
  def handle_demand(:output, _size, :buffers, _ctx, %{encoder_ref: nil} = state) do
    # Wait until we have an encoder
    {:ok, state}
  end

  def handle_demand(:output, size, :buffers, _ctx, state) do
    {{:ok, demand: {:input, size}}, state}
  end

  @impl true
  def handle_process(:input, %Buffer{payload: payload}, ctx, state) do
    %{encoder_ref: encoder_ref} = state

    case Native.encode(payload, encoder_ref) do
      {:ok, frames} ->
        bufs = wrap_frames(frames)
        in_caps = ctx.pads.input.caps

        caps =
          {:output,
           %H264{
             alignment: :au,
             framerate: in_caps.framerate,
             height: in_caps.height,
             width: in_caps.width,
             profile: @h264_profile,
             stream_format: :byte_stream
           }}

        # redemand is needed until the internal buffer of encoder is filled (no buffers will be
        # generated before that) but it is a noop if the demand has been fulfilled
        actions = [{:caps, caps} | bufs] ++ [redemand: :output]
        {{:ok, actions}, state}

      {:error, reason} ->
        {{:error, reason}, state}

      _ ->
        {{:error, "Invalid native encode response"}, state}
    end
  end

  @impl true
  def handle_caps(:input, %Raw{} = caps, _ctx, state) do
    {framerate_num, framerate_denom} = caps.framerate

    case Native.create(
           caps.width,
           caps.height,
           caps.format,
           state.bitrate,
           state.target_usage,
           framerate_num,
           framerate_denom
         ) do
      {:ok, encoder_ref} ->
        {{:ok, redemand: :output}, %{state | encoder_ref: encoder_ref}}

      {:error, reason} ->
        {{:error, reason}, state}

      _ ->
        {{:error, "Invalid native create response"}, state}
    end
  end

  @impl true
  def handle_end_of_stream(:input, _ctx, state) do
    with {:ok, frames} <- Native.flush(state.encoder_ref),
         bufs <- wrap_frames(frames) do
      actions = bufs ++ [end_of_stream: :output, notify: {:end_of_stream, :input}]
      {{:ok, actions}, state}
    else
      {:error, reason} -> {{:error, reason}, state}
    end
  end

  @impl true
  def handle_prepared_to_stopped(_ctx, state) do
    {:ok, %{state | encoder_ref: nil}}
  end

  defp wrap_frames([]), do: []

  defp wrap_frames(frames) do
    frames |> Enum.map(fn frame -> %Buffer{payload: frame} end) ~> [buffer: {:output, &1}]
  end
end
