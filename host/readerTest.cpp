#include "PacketReader.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

// Feeds one chunk to the reader and prints what came back.
void feedAndShow(PacketReader& reader, const std::vector<uint8_t>& chunk) {
	std::vector<ParsedPacket> packets = reader.feed(chunk);
	std::cout << "  fed " << chunk.size() << " byte(s) -> "
	          << packets.size() << " packet(s)\n";
	for (const ParsedPacket& p : packets) {
		printResult("    packet", p);
	}
}

int main() {
	const std::vector<uint8_t> good{0xAA, 0x03, 0x10, 0x20, 0x30, 0x60};

	{
		std::cout << "Test 1: one whole packet\n";
		PacketReader reader;
		feedAndShow(reader, good);
	}
	{
		std::cout << "Test 2: split in two\n";
		PacketReader reader;
		feedAndShow(reader, {0xAA, 0x03, 0x10});
		feedAndShow(reader, {0x20, 0x30, 0x60});
	}
	{
		std::cout << "Test 3: two in one chunk\n";
		PacketReader reader;
		feedAndShow(reader, {0xAA, 0x03, 0x10, 0x20, 0x30, 0x60, 0xAA, 0x00, 0x00});
	}
	{
		std::cout << "Test 4: junk first\n";
		PacketReader reader;
		feedAndShow(reader, {0x13, 0x37, 0xAA, 0x03, 0x10, 0x20, 0x30, 0x60});
	}
	{
		std::cout << "Test 5: one byte at a time\n";
		PacketReader reader;
		for (uint8_t b : good) {
			feedAndShow(reader, {b});
		}
	}
	{
		std::cout << "Test 6: false header hiding a real packet\n";
		PacketReader reader;
		feedAndShow(reader, {0xAA, 0x04, 0xAA, 0x00, 0x00, 0x00, 0x00});
	}

	return 0;
}