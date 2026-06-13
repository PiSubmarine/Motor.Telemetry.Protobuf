#pragma once

#include <system_error>

namespace PiSubmarine::Depth::Telemetry::Protobuf
{
    enum class ErrorCode
    {
        SerializationFailed = 1,
        DeserializationFailed,
        InvalidPayload
    };

    [[nodiscard]] std::error_code make_error_code(ErrorCode errorCode) noexcept;
}

namespace std
{
    template<>
    struct is_error_code_enum<PiSubmarine::Depth::Telemetry::Protobuf::ErrorCode> : true_type
    {
    };
}
