defmodule Encodersysmem.NativeTest do
  use ExUnit.Case, async: true
  alias Membrane.Element.Msdk.H264.Encodersysmem.Native, as: Enc

  @tag :dev
  test "Encode 1 240p frame" do
    in_path = "../fixtures/reference-100-240p.raw" |> Path.expand(__DIR__)
    assert {:ok, file} = File.read(in_path)
    IO.puts("create")
    assert {:ok, ref} = Enc.create(320, 240, :I420, 1000, :medium, 30, 1)
    assert <<frame::bytes-size(115_200), tail::binary>> = file
    IO.puts("encode")
    assert {:ok, frames} = Enc.encode(frame, ref)
    IO.puts("encode")
    assert {:ok, frames} = Enc.encode(frame, ref)
    IO.puts("flush")
    assert {:ok, frames} = Enc.flush(ref)
  end
end
