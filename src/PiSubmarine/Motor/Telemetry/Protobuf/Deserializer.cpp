#include "PiSubmarine/Motor/Telemetry/Protobuf/Deserializer.h"

#include <optional>

#include "Motor.pb.h"
#include "PiSubmarine/Motor/Telemetry/Protobuf/ErrorCode.h"
#include "PiSubmarine/Error/Api/MakeError.h"

namespace PiSubmarine::Motor::Telemetry::Protobuf
{
    namespace
    {
        [[nodiscard]] Error::Api::Error MakeMotorTelemetryError(
            const Error::Api::ErrorCondition condition,
            const ErrorCode code)
        {
            return Error::Api::MakeError(condition, make_error_code(code));
        }

        [[nodiscard]] std::optional<Api::OperationalState> ParseOperationalState(const int32_t value)
        {
            switch (value)
            {
                case 0:
                    return Api::OperationalState::Operational;
                case 1:
                    return Api::OperationalState::Degraded;
                case 2:
                    return Api::OperationalState::Faulted;
                default:
                    return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<Api::DriveDirection> ParseDriveDirection(const int32_t value)
        {
            switch (value)
            {
                case -1:
                    return Api::DriveDirection::Reverse;
                case 0:
                    return Api::DriveDirection::Idle;
                case 1:
                    return Api::DriveDirection::Forward;
                default:
                    return std::nullopt;
            }
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

        ::pisubmarine::motor::telemetry::protobuf::State protoState;
        if (!protoState.ParseFromArray(reinterpret_cast<const char*>(rawResult->data()), static_cast<int>(rawResult->size())))
        {
            return std::unexpected(MakeMotorTelemetryError(
                Error::Api::ErrorCondition::ContractError,
                ErrorCode::DeserializationFailed));
        }

        const auto operational = ParseOperationalState(protoState.operational());
        if (!operational.has_value())
        {
            return std::unexpected(MakeMotorTelemetryError(
                Error::Api::ErrorCondition::ContractError,
                ErrorCode::InvalidPayload));
        }

        Api::State state{
            .Operational = *operational,
            .ActiveFaults = static_cast<Api::Faults>(protoState.active_faults()),
            .ActiveWarnings = static_cast<Api::Warnings>(protoState.active_warnings())};

        if (protoState.has_direction())
        {
            const auto direction = ParseDriveDirection(protoState.direction());
            if (!direction.has_value())
            {
                return std::unexpected(MakeMotorTelemetryError(
                    Error::Api::ErrorCondition::ContractError,
                    ErrorCode::InvalidPayload));
            }

            state.Direction = *direction;
        }

        if (protoState.has_drive_effort())
        {
            try
            {
                state.DriveEffort = NormalizedFraction{protoState.drive_effort()};
            }
            catch (...)
            {
                return std::unexpected(MakeMotorTelemetryError(
                    Error::Api::ErrorCondition::ContractError,
                    ErrorCode::InvalidPayload));
            }
        }

        return state;
    }
}
