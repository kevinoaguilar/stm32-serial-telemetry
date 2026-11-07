#include "Parser.h"
#include "Cobs.h"
#include "Crc16.h"
#include <cstdint>
 
ParsedPacket parseFrame(const std::vector<uint8_t>& frame) {
	ParsedPacket result;
	result.valid = false;
	result.length = 0;

	std::vector<uint8_t> decodedFrame;
	if (!cobsDecode(frame.data(), frame.size(), decodedFrame)){
		return result;
	}	

	if (decodedFrame.size() < 2) {
		return result;
	}
	
	uint16_t crcSignature = ((uint16_t)decodedFrame[decodedFrame.size() - 2] << 8 | decodedFrame[decodedFrame.size() - 1]);
	decodedFrame.erase(decodedFrame.end() - 2, decodedFrame.end());

	uint16_t hostCrc = crc16(decodedFrame.data(), decodedFrame.size());
	
	if (crcSignature != hostCrc) {
		return result;
	}
	
	result.payload = decodedFrame;
	result.length = decodedFrame.size();
	result.valid = true;
	return result;
}
 