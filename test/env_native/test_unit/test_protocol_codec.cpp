#include <unity.h>
#include "../../../include/Protocol.h"
#include "../../../lib/Network/ProtocolCodec.h"

void test_serialize_deserialize_round_trip() {
    ImpactPayload original;
    original.peakDeceleration = 12.5f;
    original.seqNum = 42;
    original.footSide = FootSide::RIGHT;

    uint8_t buffer[IMPACT_PAYLOAD_WIRE_SIZE];
    const size_t written = serializeImpactPayload(original, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(IMPACT_PAYLOAD_WIRE_SIZE, written);

    ImpactPayload decoded;
    TEST_ASSERT_TRUE(deserializeImpactPayload(buffer, written, decoded));
    TEST_ASSERT_EQUAL_FLOAT(original.peakDeceleration, decoded.peakDeceleration);
    TEST_ASSERT_EQUAL(original.seqNum, decoded.seqNum);
    TEST_ASSERT_EQUAL(original.footSide, decoded.footSide);
}

void test_deserialize_rejects_short_buffer() {
    uint8_t buffer[IMPACT_PAYLOAD_WIRE_SIZE] = {};
    ImpactPayload out;
    TEST_ASSERT_FALSE(deserializeImpactPayload(buffer, IMPACT_PAYLOAD_WIRE_SIZE - 1, out));
}

void test_deserialize_rejects_unknown_protocol_version() {
    ImpactPayload original{12.5f, FootSide::LEFT, 42};
    uint8_t buffer[IMPACT_PAYLOAD_WIRE_SIZE];
    TEST_ASSERT_EQUAL(IMPACT_PAYLOAD_WIRE_SIZE,
                      serializeImpactPayload(original, buffer, sizeof(buffer)));
    buffer[0] = static_cast<uint8_t>(PROTOCOL_VERSION + 1);

    ImpactPayload out{};
    TEST_ASSERT_FALSE(deserializeImpactPayload(buffer, sizeof(buffer), out));
}

void test_serialize_deserialize_heartbeat_round_trip() {
    HeartbeatPayload original{DeviceRole::ANKLE_LEFT, 87};
    uint8_t buffer[HEARTBEAT_PAYLOAD_WIRE_SIZE] = {};
    const size_t written = serializeHeartbeatPayload(original, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(HEARTBEAT_PAYLOAD_WIRE_SIZE, written);

    HeartbeatPayload decoded{};
    TEST_ASSERT_TRUE(deserializeHeartbeatPayload(buffer, written, decoded));
    TEST_ASSERT_EQUAL(static_cast<int>(original.deviceRole), static_cast<int>(decoded.deviceRole));
    TEST_ASSERT_EQUAL(original.batteryLevel, decoded.batteryLevel);
}
