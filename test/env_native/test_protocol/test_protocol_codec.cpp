#include <unity.h>
#include <cstring>
#include "../../../include/Protocol.h"
#include "../../../lib/network/ProtocolCodec.h"

void setUp(void) {}
void tearDown(void) {}

void test_impact_payload_roundtrip(void) {
    ImpactPayload original;
    original.peakDeceleration = 4.25f;
    original.footSide = static_cast<uint8_t>(1);
    original.seqNum = 42u;

    uint8_t wire[IMPACT_PAYLOAD_WIRE_SIZE];
    size_t written = serializeImpactPayload(original, wire, sizeof(wire));
    TEST_ASSERT_EQUAL_UINT32(IMPACT_PAYLOAD_WIRE_SIZE, written);

    ImpactPayload decoded;
    TEST_ASSERT_TRUE(deserializeImpactPayload(wire, written, decoded));

    TEST_ASSERT_EQUAL_FLOAT(original.peakDeceleration, decoded.peakDeceleration);
    TEST_ASSERT_EQUAL_UINT8(original.footSide, decoded.footSide);
    TEST_ASSERT_EQUAL_UINT32(original.seqNum, decoded.seqNum);
}

void test_impact_payload_wire_size_guard(void) {
    ImpactPayload payload;
    uint8_t smallBuffer[4];
    TEST_ASSERT_EQUAL_UINT32(0, serializeImpactPayload(payload, smallBuffer, sizeof(smallBuffer)));
    TEST_ASSERT_FALSE(deserializeImpactPayload(smallBuffer, sizeof(smallBuffer), payload));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_impact_payload_roundtrip);
    RUN_TEST(test_impact_payload_wire_size_guard);
    return UNITY_END();
}
