defmodule Membrane.Element.Msdk.H264.BundlexProject do
  use Bundlex.Project

  def project() do
    [
      nifs: nifs(Bundlex.platform())
    ]
  end

  def nifs(_platform) do
    [
      parser: [
        deps: [unifex: :unifex],
        sources: ["_generated/parser.c", "parser.c"],
        language: "c",
        pkg_configs: ["libavcodec", "libavutil"]
      ],
      decoder: [
        deps: [unifex: :unifex],
        sources: ["_generated/decoder.c", "decoder.c"],
        language: "c",
        pkg_configs: ["libavcodec", "libavutil"]
      ],
      encoder: [
        deps: [unifex: :unifex],
        sources: ["_generated/encoder.c", "encoder.c"],
        language: "c",
        pkg_configs: ["libavcodec", "libavutil"]
      ],
      hello: [
        deps: [unifex: :unifex],
        sources: ["_generated/hello.c", "hello.c"],
        language: "c",
        pkg_configs: ["libavcodec", "libavutil"]
      ],
      encoderhw: [
        deps: [unifex: :unifex],
        sources: [
          "_generated/encoderhw.cpp",
          "encoderhw.cpp",
          "msdk/common_utils.cpp",
          "msdk/common_utils_linux.cpp",
          "msdk/common_vaapi.cpp",
          "msdk_membrane/membrane_utils.cpp"
          ],
        language: "cpp",
        pkg_configs: ["libavcodec", "libavutil", "libva", "libva-drm", "libmfx", "libdrm"]
      ]
    ]
  end
end
