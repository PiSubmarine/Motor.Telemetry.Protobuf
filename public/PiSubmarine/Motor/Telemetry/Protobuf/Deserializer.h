#pragma once

#include "PiSubmarine/Depth/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Telemetry/Api/IRawSource.h"

namespace PiSubmarine::Depth::Telemetry::Protobuf
{
    class Deserializer final : public Api::IProvider
    {
    public:
        explicit Deserializer(const ::PiSubmarine::Telemetry::Api::IRawSource& rawSource);

        [[nodiscard]] Error::Api::Result<Api::State> GetState() const override;

    private:
        const ::PiSubmarine::Telemetry::Api::IRawSource& m_RawSource;
    };
}
