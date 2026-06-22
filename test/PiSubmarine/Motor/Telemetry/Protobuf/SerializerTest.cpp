#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "PiSubmarine/Motor/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Motor/Telemetry/Protobuf/Deserializer.h"
#include "PiSubmarine/Motor/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Telemetry/Api/IRawSourceMock.h"

namespace PiSubmarine::Motor::Telemetry::Protobuf
{
    class ProviderMock final : public Api::IProvider
    {
    public:
        MOCK_METHOD((Error::Api::Result<Api::State>), GetState, (), (const, override));
    };

    TEST(SerializerTest, RoundTripsMotorState)
    {
        ProviderMock providerMock;
        const Api::State expectedState{
            .Operational = Api::OperationalState::Degraded,
            .ActiveFaults = static_cast<Api::Faults>(
                static_cast<uint32_t>(Api::Faults::Overcurrent)
                | static_cast<uint32_t>(Api::Faults::OpenLoad)),
            .ActiveWarnings = Api::Warnings::Temperature,
            .Direction = Api::DriveDirection::Forward,
            .DriveEffort = NormalizedFraction{0.55}};

        EXPECT_CALL(providerMock, GetState())
            .WillOnce(testing::Return(Error::Api::Result<Api::State>(expectedState)));

        Serializer serializer(providerMock);
        const auto rawResult = serializer.GetRaw();

        ASSERT_TRUE(rawResult.has_value());
        EXPECT_FALSE(rawResult->empty());

        ::PiSubmarine::Telemetry::Api::IRawSourceMock rawSourceMock;
        EXPECT_CALL(rawSourceMock, GetRaw())
            .WillOnce(testing::Return(Error::Api::Result<std::vector<std::byte>>(*rawResult)));

        Deserializer deserializer(rawSourceMock);
        const auto stateResult = deserializer.GetState();

        ASSERT_TRUE(stateResult.has_value());
        EXPECT_EQ(*stateResult, expectedState);
    }
}
