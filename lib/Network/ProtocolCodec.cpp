#include "ProtocolCodec.h"
#include <cstring>

namespace
{

    static_assert(sizeof(float) == sizeof(uint32_t), "Float size mismatch");

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    uint32_t hostToNetwork32(uint32_t value)
    {
        return value;
    }
    uint32_t networkToHost32(uint32_t value)
    {
        return value;
    }
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    uint32_t hostToNetwork32(uint32_t value)
    {
#if defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap32(value);
#else
        return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) |
               ((value & 0x00FF0000u) >> 8) | ((value & 0xFF000000u) >> 24);
#endif
    }
    uint32_t networkToHost32(uint32_t value)
    {
        return hostToNetwork32(value);
    }
#else
    uint32_t hostToNetwork32(uint32_t value)
    {
#if defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap32(value);
#else
        return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) |
               ((value & 0x00FF0000u) >> 8) | ((value & 0xFF000000u) >> 24);
#endif
    }
    uint32_t networkToHost32(uint32_t value)
    {
        return hostToNetwork32(value);
    }
#endif

    void writeUint32BE(uint8_t *buffer, uint32_t value)
    {
        const uint32_t networkValue = hostToNetwork32(value);
        memcpy(buffer, &networkValue, sizeof(networkValue));
    }

    uint32_t readUint32BE(const uint8_t *buffer)
    {
        uint32_t networkValue = 0;
        memcpy(&networkValue, buffer, sizeof(networkValue));
        return networkToHost32(networkValue);
    }

    uint32_t floatToNetworkBits(float value)
    {
        uint32_t bits = 0;
        memcpy(&bits, &value, sizeof(bits));
        return hostToNetwork32(bits);
    }

    float networkBitsToFloat(uint32_t networkBits)
    {
        const uint32_t hostBits = networkToHost32(networkBits);
        float value = 0.0f;
        memcpy(&value, &hostBits, sizeof(value));
        return value;
    }

    // Outils de flux séquentiels pour éliminer le codage par index manuel
    class BufferWriter
    {
    private:
        uint8_t *buf_;
        size_t cap_;
        size_t idx_;

    public:
        BufferWriter(uint8_t *buffer, size_t capacity) : buf_(buffer), cap_(capacity), idx_(0) {}

        bool write8(uint8_t val)
        {
            if (idx_ + 1 > cap_)
                return false;
            buf_[idx_++] = val;
            return true;
        }

        bool write32BE(uint32_t val)
        {
            if (idx_ + 4 > cap_)
                return false;
            writeUint32BE(buf_ + idx_, val);
            idx_ += 4;
            return true;
        }

        size_t getLength() const { return idx_; }
    };

    class BufferReader
    {
    private:
        const uint8_t *buf_;
        size_t len_;
        size_t idx_;

    public:
        BufferReader(const uint8_t *buffer, size_t length) : buf_(buffer), len_(length), idx_(0) {}

        bool read8(uint8_t &val)
        {
            if (idx_ + 1 > len_)
                return false;
            val = buf_[idx_++];
            return true;
        }

        bool read32BE(uint32_t &val)
        {
            if (idx_ + 4 > len_)
                return false;
            val = readUint32BE(buf_ + idx_);
            idx_ += 4;
            return true;
        }
    };

} // namespace

size_t serializeImpactPayload(const ImpactPayload &payload, uint8_t *buffer, size_t capacity)
{
    if (!buffer || capacity < IMPACT_PAYLOAD_WIRE_SIZE)
    {
        return 0;
    }

    BufferWriter writer(buffer, capacity);
    if (!writer.write8(PROTOCOL_VERSION))
        return 0;
    if (!writer.write32BE(floatToNetworkBits(payload.peakDeceleration)))
        return 0;
    if (!writer.write8(static_cast<uint8_t>(payload.footSide)))
        return 0;
    if (!writer.write32BE(payload.seqNum))
        return 0;

    return writer.getLength();
}

bool deserializeImpactPayload(const uint8_t *buffer, size_t length, ImpactPayload &out)
{
    if (!buffer || length < IMPACT_PAYLOAD_WIRE_SIZE)
    {
        return false;
    }

    BufferReader reader(buffer, length);
    uint8_t version = 0;
    if (!reader.read8(version) || version != PROTOCOL_VERSION)
        return false;

    uint32_t peakBits = 0;
    if (!reader.read32BE(peakBits))
        return false;
    out.peakDeceleration = networkBitsToFloat(peakBits);

    uint8_t side = 0;
    if (!reader.read8(side))
        return false;
    out.footSide = static_cast<FootSide>(side);

    if (!reader.read32BE(out.seqNum))
        return false;

    return true;
}

size_t serializeHeartbeatPayload(const HeartbeatPayload &payload, uint8_t *buffer, size_t capacity)
{
    if (!buffer || capacity < HEARTBEAT_PAYLOAD_WIRE_SIZE)
    {
        return 0;
    }

    BufferWriter writer(buffer, capacity);
    if (!writer.write8(PROTOCOL_VERSION))
        return 0;
    if (!writer.write8(static_cast<uint8_t>(payload.deviceRole)))
        return 0;
    if (!writer.write8(payload.batteryLevel))
        return 0;

    return writer.getLength();
}

bool deserializeHeartbeatPayload(const uint8_t *buffer, size_t length, HeartbeatPayload &out)
{
    if (!buffer || length < HEARTBEAT_PAYLOAD_WIRE_SIZE)
    {
        return false;
    }

    BufferReader reader(buffer, length);
    uint8_t version = 0;
    if (!reader.read8(version) || version != PROTOCOL_VERSION)
        return false;

    uint8_t role = 0;
    if (!reader.read8(role))
        return false;
    out.deviceRole = static_cast<DeviceRole>(role);

    uint8_t battery = 0;
    if (!reader.read8(battery))
        return false;
    out.batteryLevel = battery;

    return true;
}