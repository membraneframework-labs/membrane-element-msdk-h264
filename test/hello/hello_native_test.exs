defmodule Hello.NativeTest do
  use ExUnit.Case, async: true
  alias Membrane.Element.Msdk.H264.Hello.Native, as: Hello

  test "Test foo" do
    {:ok, state} = Hello.init()

    {:ok, resp} = Hello.foo(self(), state)

    assert resp == 42
  end
end
