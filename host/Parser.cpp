#include "Parser.h"
#include <iostream>

// Sums every byte in the payload. uint8_t wraps at 256, which is
// exactly the mod-256 checksum the protocol calls for.
uint8_t computeChecksum(const std::vector<uint8_t>& payload) {
	uint8_t total = 0;
	for (uint8_t byte : payload) {
		total += byte;
	}
	return total;
}
 
ParsedPacket parsePacket(const std::vector<uint8_t>& buf) {
	ParsedPacket result;
	result.valid = false;
	result.length = 0;
 
	// Guard clauses: stop at the first sign of a bad packet.
	if (buf.size() < 3) return result;                   // doesn't fit minimum size
	if (buf[0] != 0xAA) return result;                   // header does not match
 
	uint8_t length = buf[1];
	if (buf.size() < (size_t)length + 3) return result;  // length field is lying
 
	// Payload lives at indices 2 through length + 1.
	// Index length + 2 is the checksum, so it must NOT go into the payload.
	for (size_t i = 2; i < (size_t)length + 2; ++i) {
		result.payload.push_back(buf[i]);
	}
 
	uint8_t expected = buf[length + 2];
	if (computeChecksum(result.payload) != expected) return result;
 
	// If it reaches here, every check passed.
	result.length = length;
	result.valid = true;
	return result;
}
 
void printResult(const std::string& name, const ParsedPacket& p) {
	std::cout << name << ": ";
	if (!p.valid) {
		std::cout << "INVALID\n";
		return;
	}
	std::cout << "valid, length " << (int)p.length << ", payload:";
	for (uint8_t byte : p.payload) {
		std::cout << " " << (int)byte;
	}
	std::cout << "\n";
}
 