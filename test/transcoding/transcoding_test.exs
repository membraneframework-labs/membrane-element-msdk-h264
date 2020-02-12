defmodule TranscodingTest do
  import Membrane.Testing.Assertions
  alias Membrane.Element
  alias Membrane.Testing.Pipeline
  use ExUnit.Case

  def prepare_paths(filename) do
    in_path = "../fixtures/input-#{filename}.h264" |> Path.expand(__DIR__)
    out_path = "/tmp/output-transcode-#{filename}.h264"
    File.rm(out_path)
    on_exit(fn -> File.rm(out_path) end)
    {in_path, out_path}
  end

  def make_pipeline(in_path, out_path) do
    Pipeline.start_link(%Pipeline.Options{
      elements: [
        file_src: %Element.File.Source{chunk_size: 40_960, location: in_path},
        parser: Element.Msdk.H264.Parser,
        decoder: Element.Msdk.H264.Decoder,
        encoder: %Element.Msdk.H264.Encoder{preset: :fast, crf: 30},
        sink: %Element.File.Sink{location: out_path}
      ]
    })
  end

  def perform_test(filename, timeout) do
    {in_path, out_path} = prepare_paths(filename)

    assert {:ok, pid} = make_pipeline(in_path, out_path)
    assert Pipeline.play(pid) == :ok
    assert_end_of_stream(pid, :sink, :input, timeout)
  end

  describe "TranscodingPipeline should" do
    test "transcode 10 720p frames" do
      perform_test("10-720p", 1000)
    end

    test "transcode 100 240p frames" do
      perform_test("100-240p", 2000)
    end

    test "transcode 20 360p frames with 422 subsampling" do
      perform_test("20-360p-I422", 2000)
    end
  end
end
