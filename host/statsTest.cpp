#include "Parser.h"
#include "PacketStats.h"
#include <iostream>
#include <vector>
#include <cstdint>

int main() {
	std::vector<uint8_t> good{0xAA, 0x03, 0x10, 0x20, 0x30, 0x60};
	std::vector<uint8_t> badHeader{0xBB, 0x03, 0x10, 0x20, 0x30, 0x60};
	std::vector<uint8_t> badChecksum{0xAA, 0x03, 0x10, 0x20, 0x30, 0xFF};
	std::vector<uint8_t> tooShort{0xAA, 0x03};
	std::vector<uint8_t> lyingLength{0xAA, 0xC8, 0x10, 0x20};   // claims 200 bytes
	std::vector<uint8_t> emptyPayload{0xAA, 0x00, 0x00};        // valid, no data

	printResult("good", parsePacket(good));
	printResult("badHeader", parsePacket(badHeader));
	printResult("badChecksum", parsePacket(badChecksum));
	printResult("tooShort", parsePacket(tooShort));
	printResult("lyingLength", parsePacket(lyingLength));
	printResult("emptyPayload", parsePacket(emptyPayload));

	PacketStats ps;
	ps.record(parsePacket(good));
	ps.record(parsePacket(badHeader));
	ps.record(parsePacket(badChecksum));
	ps.record(parsePacket(tooShort));
	ps.record(parsePacket(lyingLength));
	ps.record(parsePacket(emptyPayload));

	printStats(ps);

	return 0;
}