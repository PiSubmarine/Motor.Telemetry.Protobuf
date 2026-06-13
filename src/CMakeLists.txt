include("${_build_cmake_local}")

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

set(DEPTH_TELEMETRY_PROTO "${CMAKE_CURRENT_SOURCE_DIR}/PiSubmarine/Depth/Telemetry/Protobuf/Depth.proto")
protobuf_generate_cpp(DEPTH_TELEMETRY_PROTO_SRCS DEPTH_TELEMETRY_PROTO_HDRS "${DEPTH_TELEMETRY_PROTO}")

message("Adding library: ${PISUBMARINE_MODULE_NAME}")
add_library(${PISUBMARINE_MODULE_NAME}
        PiSubmarine/Depth/Telemetry/Protobuf/Deserializer.cpp
        PiSubmarine/Depth/Telemetry/Protobuf/ErrorCode.cpp
        PiSubmarine/Depth/Telemetry/Protobuf/Serializer.cpp
        "../public/PiSubmarine/Depth/Telemetry/Protobuf/Deserializer.h"
        "../public/PiSubmarine/Depth/Telemetry/Protobuf/ErrorCode.h"
        "../public/PiSubmarine/Depth/Telemetry/Protobuf/Serializer.h"
        ${DEPTH_TELEMETRY_PROTO_SRCS}
        ${DEPTH_TELEMETRY_PROTO_HDRS})
target_include_directories(${PISUBMARINE_MODULE_NAME} PUBLIC "../public" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
PiSubmarineInitTarget(${PISUBMARINE_MODULE_NAME})

PiSubmarineAddDependency("https://github.com/PiSubmarine/Depth.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Depth.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Telemetry.Api)

target_link_libraries(${PISUBMARINE_MODULE_NAME} PRIVATE protobuf::libprotobuf)
