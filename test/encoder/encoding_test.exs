defmodule EncodingTest do
  import Membrane.Testing.Assertions
  alias Membrane.Element
  alias Membrane.Testing.Pipeline
  use ExUnit.Case

  def prepare_paths(filename) do
    in_path = "../fixtures/reference-#{filename}.raw" |> Path.expand(__DIR__)
    out_path = "/tmp/output-encode-#{filename}.h264"
    File.rm(out_path)
    on_exit(fn -> File.rm(out_path) end)
    {in_path, out_path}
  end

  def make_pipeline(in_path, out_path, width, height, format \\ :I420) do
    Pipeline.start_link(%Pipeline.Options{
      elements: [
        file_src: %Element.File.Source{chunk_size: 40_960, location: in_path},
        parser: %Element.RawVideo.Parser{width: width, height: height, format: format},
        encoder: %Element.Msdk.H264.Encoder{bitrate: 1_000, target_usage: :medium},
        sink: %Element.File.Sink{location: out_path}
      ]
    })
  end

  def perform_test(filename, width, height, format \\ :I420) do
    {in_path, out_path} = prepare_paths(filename)

    assert {:ok, pid} = make_pipeline(in_path, out_path, width, height, format)
    assert Pipeline.play(pid) == :ok
    assert_end_of_stream(pid, :sink, :input, 3000)
  end

  describe "EncodingPipeline should" do
    test "encode 10 720p frames" do
      perform_test("10-720p", 1280, 720)
    end

    test "encode 100 240p frames" do
      perform_test("100-240p", 340, 240)
    end

    test "encode 20 360p frames with 422 subsampling" do
      perform_test("20-360p-I422", 480, 360, :I422)
    end
  end

  @tag :skip
  @tag :large
  describe "Large file encoding" do
    test "30MB Bunny" do
      # in_path = "/home/swm/Big_Buck_Bunny_1080_10s_30MB.yuv"
      # width = 1920
      # height = 1080

      # in_path = "/home/swm/Big_Buck_Bunny_720_10s_30MB.yuv"
      # width = 1280
      # height = 720

      in_path = "/home/swm/Big_Buck_Bunny_360_10s_30MB.yuv"
      width = 640
      height = 360

      out_path = "/home/swm/bunny_test.h264"

      assert {:ok, pid} = make_pipeline(in_path, out_path, width, height)
      assert Pipeline.play(pid) == :ok
      assert_end_of_stream(pid, :sink, :input, 60_000)
    end
  end

  @tag timeout: :infinity
  @tag :skip
  @tag :infinite
  describe "Infinite create-run-delete loop" do
    test "small raw file" do
      in_path = "../fixtures/reference-20-360p-I422.raw" |> Path.expand(__DIR__)
      width = 640
      height = 360
      out_path = "/home/swm/bunny_test.h264"

      Enum.each(1..999999999, fn i ->
        if rem(i, 10) == 0, do: IO.puts "Run #{i}"
        assert {:ok, pid} = make_pipeline(in_path, out_path, width, height)
        assert Pipeline.play(pid) == :ok
        assert_end_of_stream(pid, :sink, :input, 3000)
        Pipeline.stop_and_terminate(pid)
        #Process.sleep(200)
      end)
    end
  end
end
