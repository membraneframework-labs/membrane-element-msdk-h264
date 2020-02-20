defmodule Membrane.Element.Msdk.H264.BundlexProject do
  use Bundlex.Project

  def project() do
    [nifs: nifs(Bundlex.platform())]
  end

  def nifs(_platform) do
    [
      encoder: [
        deps: [unifex: :unifex],
        sources: [
          "_generated/encoder.cpp",
          "encoder.cpp",
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
