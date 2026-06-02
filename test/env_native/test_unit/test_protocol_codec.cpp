#include <unity.h>
#include "../../../include/Protocol.h"
#include "../../../lib/network/ProtocolCodec.h"

void test_serialize_deserialize_round_trip() {
    ImpactPayload original;
    original.peakDeceleration = 12.5f;
    original.seqNum = 42;
    original.footSide = 1;

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
