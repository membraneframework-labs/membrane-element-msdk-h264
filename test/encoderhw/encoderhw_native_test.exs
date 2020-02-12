defmodule Encoderhw.NativeTest do
  use ExUnit.Case, async: true
  alias Membrane.Element.Msdk.H264.Encoderhw.Native, as: Enc

  @tag :dev
  test "Encode 1 240p frame" do
    # in_path = "../fixtures/reference-100-240p.raw" |> Path.expand(__DIR__)
    # assert {:ok, file} = File.read(in_path)
    # assert {:ok, ref} = Enc.create(320, 240, :I420, 1000, 30, 1)
    # assert <<frame::bytes-size(115_200), tail::binary>> = file
    # assert {:ok, frames} = Enc.encode(frame, ref)

    in_path = "/home/swm/Downloads/test_stream_176x96.yuv"
    assert {:ok, file0} = File.read(in_path)
    assert {:ok, file1} = File.read(in_path)
    assert {:ok, file2} = File.read(in_path)

    IO.puts "\n\nEnc.create ---------"
    assert {:ok, ref} = Enc.create(176, 96, :I420, 1000, :medium, 30, 1)

    :timer.sleep(1000)
  
    #assert <<frame::bytes-size(25_344), tail::binary>> = file

    IO.puts "\n\nEnc.encode --------- 0 -----------"
    IO.inspect Enc.encode(file0, ref)
    #assert {:ok, frames0} = Enc.encode(file0, ref)

    #IO.puts "\n\nEnc.encode --------- 1 -----------"
    #assert {:ok, frames1} = Enc.encode(file1, ref)

    #IO.puts "\n\nEnc.encode --------- 2 -----------"
    #assert {:ok, frames2} = Enc.encode(file2, ref)

    #assert {:ok, [frame]} = Enc.flush(ref)
  end
end
