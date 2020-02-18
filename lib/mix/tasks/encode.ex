defmodule Mix.Tasks.Encode do
  import Membrane.Testing.Assertions
  alias Membrane.Element
  alias Membrane.Testing.Pipeline
  use Mix.Task

  @shortdoc "Encodes a raw I420 input file to a h264 encoded output file."
  def run([width_param, height_param, bitrate_param, in_path_param, out_path_param]) do
    Mix.Task.run("app.start")

    {width, _rest} = Integer.parse(width_param)
    {height, _rest} = Integer.parse(height_param)
    {bitrate, _rest} = Integer.parse(bitrate_param)
    in_path = in_path_param |> Path.expand(__DIR__)
    out_path = out_path_param |> Path.expand(__DIR__)
    format = :I420
    target_usage = :medium

    start_time = DateTime.utc_now() |> DateTime.to_unix()

    {:ok, pid} = Pipeline.start_link(%Pipeline.Options{
      elements: [
        file_src: %Element.File.Source{chunk_size: 40_960, location: in_path},
        parser: %Element.RawVideo.Parser{width: width, height: height, format: format},
        encoder: %Element.Msdk.H264.Encoder{bitrate: bitrate, target_usage: target_usage},
        sink: %Element.File.Sink{location: out_path}
      ]
    })
    :ok = Pipeline.play(pid)
    IO.puts "Starting encoding..."

    assert_end_of_stream(pid, :sink, :input, 3000)

    took_time = (DateTime.utc_now() |> DateTime.to_unix()) - start_time
    IO.puts "Done! Took #{took_time}s"
  end

  def run(args) do
    IO.puts "Args:"
    IO.inspect args
    IO.puts "Usage:"
    IO.puts "mix hello [width] [height] [bitrate] [in_file] [out_file]"
  end
end
