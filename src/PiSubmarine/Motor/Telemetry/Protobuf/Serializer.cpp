#include "PiSubmarine/Motor/Telemetry/Protobuf/Serializer.h"

#include <string>

#include "Motor.pb.h"
#include "PiSubmarine/Motor/Telemetry/Protobuf/ErrorCode.h"
#include "PiSubmarine/Error/Api/MakeError.h"

namespace PiSubmarine::Motor::Telemetry::Protobuf
{
    Serializer::Serializer(const Api::IProvider& provider)
        : m_Provider(provider)
    {
    }

    Error::Api::Result<std::vector<std::byte>> Serializer::GetRaw() const
    {
        const auto stateResult = m_Provider.GetState();
        if (!stateResult.has_value())
        {
            return std::unexpected(stateResult.error());
        }

        ::pisubmarine::motor::telemetry::protobuf::State protoState;
        protoState.set_operational(static_cast<int32_t>(stateResult->Operational));
        protoState.set_active_faults(static_cast<uint32_t>(stateResult->ActiveFaults));
        protoState.set_active_warnings(static_cast<uint32_t>(stateResult->ActiveWarnings));
        protoState.set_direction(static_cast<int32_t>(stateResult->Direction));
        protoState.set_drive_effort(static_cast<double>(stateResult->DriveEffort));

        std::string serialized;
        if (!protoState.SerializeToString(&serialized))
        {
            return std::unexpected(Error::Api::MakeError(
                Error::Api::ErrorCondition::DeviceError,
                make_error_code(ErrorCode::SerializationFailed)));
        }

        std::vector<std::byte> bytes;
        bytes.reserve(serialized.size());
        for (const char character : serialized)
        {
            bytes.push_back(static_cast<std::byte>(character));
        }

        return bytes;
    }
}
