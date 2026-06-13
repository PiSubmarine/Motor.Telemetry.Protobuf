#include "PiSubmarine/Depth/Telemetry/Protobuf/Deserializer.h"

#include "Depth.pb.h"
#include "PiSubmarine/Depth/Telemetry/Protobuf/ErrorCode.h"
#include "PiSubmarine/Error/Api/MakeError.h"

namespace PiSubmarine::Depth::Telemetry::Protobuf
{
    namespace
    {
        [[nodiscard]] Error::Api::Error MakeDepthTelemetryError(
            const Error::Api::ErrorCondition condition,
            const ErrorCode code)
        {
            return Error::Api::MakeError(condition, make_error_code(code));
        }
    }

    Deserializer::Deserializer(const ::PiSubmarine::Telemetry::Api::IRawSource& rawSource)
        : m_RawSource(rawSource)
    {
    }

    Error::Api::Result<Api::State> Deserializer::GetState() const
    {
        const auto rawResult = m_RawSource.GetRaw();
        if (!rawResult.has_value())
        {
            return std::unexpected(rawResult.error());
        }

        ::pisubmarine::depth::telemetry::protobuf::State protoState;
        if (!protoState.ParseFromArray(reinterpret_cast<const char*>(rawResult->data()), static_cast<int>(rawResult->size())))
        {
            return std::unexpected(MakeDepthTelemetryError(
                Error::Api::ErrorCondition::ContractError,
                ErrorCode::DeserializationFailed));
        }

        Api::State state{};
        if (protoState.has_depth_meters())
        {
            state.Depth = Meters{protoState.depth_meters()};
        }

        return state;
    }
}
