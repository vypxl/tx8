#include "tx8/core/util.hpp"

#include "tx8/core/instruction.hpp"
#include "tx8/core/log.hpp"

#include <istream>

using namespace tx;

std::optional<RomInfo> tx::parse_header(std::istream& stream) {
    RomInfo info;
    uint32  magic;
    stream.read((char*) &magic, sizeof(uint32));
    if (magic != MAGIC) {
        tx::log_debug("[header_parse] Invalid magic bytes: {:x} instead of {:x}\n", magic, MAGIC);
        return std::nullopt;
    }

    uint8  name_len;
    uint16 description_len;
    stream.read((char*) &name_len, sizeof(uint8));
    stream.read((char*) &description_len, sizeof(uint16));
    stream.read((char*) &info.size, sizeof(uint32));

    if (info.size > tx::MAX_ROM_SIZE) {
        tx::log_debug("[header_parse] Invalid rom size: {}, maximum allowed is {}\n", info.size, tx::MAX_ROM_SIZE);
        return std::nullopt;
    }

    uint8 header_checksum;
    stream.read((char*) &header_checksum, sizeof(uint8));

    // Skip unused bytes
    stream.seekg(UNUSED_BYTES_COUNT, std::ios::cur);

    info.name.resize(name_len);
    info.description.resize(description_len);
    stream.read(info.name.data(), name_len);
    stream.read(info.description.data(), description_len);

    stream.seekg(0);

    uint8 byte;
    uint8 checksum = 0;
    for (uint32 i = 0; i < BASIC_ROM_INFO_LEN + name_len + description_len; ++i) {
        stream.read((char*) &byte, sizeof(uint8));
        if (i == CHECKSUM_BYTE_INDEX) continue;
        checksum ^= byte;
    }

    if (checksum != header_checksum) {
        tx::log_debug("[header_parse] Invalid checksum: {:x} instead of {:x}\n", checksum, header_checksum);
        return std::nullopt;
    }
    if (stream.fail()) {
        tx::log_debug("[header_parse] Failed to parse header: read failure\n");
        return std::nullopt;
    }
    return info;
}

std::vector<uint8> tx::build_header(const RomInfo& info) {
    std::vector<uint8> header(BASIC_ROM_INFO_LEN + info.name.size() + info.description.size(), 0);

    uint8* ptr = header.data();

    uint32 magic = MAGIC;
    memcpy(ptr, &magic, sizeof(uint32));
    ptr += sizeof(uint32);

    uint8 name_len = info.name.size();
    memcpy(ptr, &name_len, sizeof(uint8));
    ptr += sizeof(uint8);

    uint16 description_len = info.description.size();
    memcpy(ptr, &description_len, sizeof(uint16));
    ptr += sizeof(uint16);

    memcpy(ptr, &info.size, sizeof(uint32));
    ptr += sizeof(uint32);

    // Skip checksum byte
    ptr += sizeof(uint8);

    // Skip unused bytes
    ptr += UNUSED_BYTES_COUNT;

    memcpy(ptr, info.name.data(), info.name.size()); // NOLINT
    ptr += info.name.size();

    memcpy(ptr, info.description.data(), info.description.size()); // NOLINT
    ptr += info.description.size();

    uint8 checksum = 0;
    for (uint32 i = 0; i < BASIC_ROM_INFO_LEN + info.name.size() + info.description.size(); ++i) {
        if (i == CHECKSUM_BYTE_INDEX) continue;
        checksum ^= header[i];
    }
    header[CHECKSUM_BYTE_INDEX] = checksum;

    return header;
}
