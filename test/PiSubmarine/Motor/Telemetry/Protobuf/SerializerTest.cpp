#include <gtest/gtest.h>

#include "PiSubmarine/Depth/Telemetry/Api/IProviderMock.h"
#include "PiSubmarine/Depth/Telemetry/Protobuf/Deserializer.h"
#include "PiSubmarine/Depth/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Telemetry/Api/IRawSourceMock.h"

namespace PiSubmarine::Depth::Telemetry::Protobuf
{
    TEST(SerializerTest, RoundTripsDepthState)
    {
        Api::IProviderMock providerMock;
        const Api::State expectedState{.Depth = 3.5_m};

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
