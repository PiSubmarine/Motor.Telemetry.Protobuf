#include "PiSubmarine/Depth/Telemetry/Protobuf/Serializer.h"

#include <string>

#include "Depth.pb.h"
#include "PiSubmarine/Depth/Telemetry/Protobuf/ErrorCode.h"
#include "PiSubmarine/Error/Api/MakeError.h"

namespace PiSubmarine::Depth::Telemetry::Protobuf
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

        ::pisubmarine::depth::telemetry::protobuf::State protoState;
        if (stateResult->Depth.has_value())
        {
            protoState.set_depth_meters(stateResult->Depth->Value);
        }

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
