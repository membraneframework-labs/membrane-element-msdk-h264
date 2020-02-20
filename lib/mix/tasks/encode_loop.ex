defmodule Mix.Tasks.EncodeLoop do
  import Membrane.Testing.Assertions
  alias Membrane.Element
  alias Membrane.Testing.Pipeline
  use Mix.Task

  @shortdoc "Infinitely loops a task: Encodes a raw I420 input file to a h264 encoded output file."
  def run([width_param, height_param, bitrate_param, in_path_param, out_path_param]) do
    Mix.Task.run("app.start")

    {width, _rest} = Integer.parse(width_param)
    {height, _rest} = Integer.parse(height_param)
    {bitrate, _rest} = Integer.parse(bitrate_param)
    in_path = in_path_param |> Path.expand(__DIR__)
    out_path = out_path_param |> Path.expand(__DIR__)
    format = :I420
    target_usage = :medium

    task_start_time = DateTime.utc_now()

    Enum.each(1..999_999_999, fn i ->
      start_time = DateTime.utc_now()

      {:ok, pid} =
        Pipeline.start(%Pipeline.Options{
          elements: [
            file_src: %Element.File.Source{chunk_size: 40_960, location: in_path},
            parser: %Element.RawVideo.Parser{width: width, height: height, format: format},
            encoder: %Element.Msdk.H264.Encoder{bitrate: bitrate, target_usage: target_usage},
            sink: %Element.File.Sink{location: out_path}
          ]
        })

      :ok = Pipeline.play(pid)
      IO.puts("Started encoding... #{i}")

      timeout = 60_000 * 60
      assert_end_of_stream(pid, :sink, :input, timeout)
      :ok = Pipeline.stop_and_terminate(pid)
      Process.sleep(10)

      took_time = DateTime.diff(DateTime.utc_now(), start_time, :millisecond) / 1000
      total_time = DateTime.diff(DateTime.utc_now(), task_start_time, :millisecond) / 1000
      IO.puts("Done! Took #{took_time}s (total #{total_time}s)")
    end)
  end

  def run(args) do
    IO.puts("Args:")
    IO.inspect(args)
    IO.puts("Usage:")
    IO.puts("mix hello [width] [height] [bitrate] [in_file] [out_file]")
  end
end
