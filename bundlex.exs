defmodule Membrane.Element.Msdk.H264.BundlexProject do
  use Bundlex.Project

  def project() do
    [nifs: nifs(Bundlex.platform())]
  end

  def nifs(_platform) do
    [
      encoder_vmem: [
        deps: [unifex: :unifex],
        sources: [
          "_generated/encoder_vmem.cpp",
          "encoder_vmem.cpp",
          "msdk/common_utils.cpp",
          "msdk/common_utils_linux.cpp",
          "msdk/common_vaapi.cpp",
          "msdk_membrane/membrane_utils.cpp"
        ],
        language: "cpp",
        pkg_configs: ["libavcodec", "libavutil", "libva", "libva-drm", "libmfx", "libdrm"]
      ],
      encoder_sysmem: [
        deps: [unifex: :unifex],
        sources: [
          "_generated/encoder_sysmem.cpp",
          "encoder_sysmem.cpp",
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
