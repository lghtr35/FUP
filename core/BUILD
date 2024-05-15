cc_library(
    name = "fup_core_lib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    copts = ["-Icore/service/interface"],
    deps = [
        "//core/entity:fup_core_entity_lib",
        "//core/service:fup_core_service_lib"
    ],
    visibility = ["//facade:__pkg__", "//facade/manager:__pkg__", "//facade/helper:__pkg__"]
)