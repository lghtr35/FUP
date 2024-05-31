cc_library(
    name = "fup_core_service_lib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp","interface/*"]),
    copts = ["-Icore/service/interface"],
    deps = [
        "//core/entity:fup_core_entity_lib",
        "@blake3"
    ],
    visibility = ["//core:__pkg__"]
)