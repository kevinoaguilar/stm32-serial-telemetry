#include "PacketStats.h"
#include <iostream>s

void printStats(const PacketStats& s) {
	std::cout << "Valid packets: " << s.valid() << "\n";
	std::cout << "Invalid packets: " << s.invalid() << "\n";
	std::cout << "Total packets: " << s.total() << "\n";
	std::cout << "Valid rate of packets: " << s.validRate() << "\n";
	std::cout << "Total payload bytes in the packets: " << s.payloadBytes() << "\n";
}