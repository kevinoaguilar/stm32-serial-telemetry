#pragma onces

#include "Parser.h"

class PacketStats {
public:
	void record(const ParsedPacket& p) {
		if (!p.valid) {
			++invalidCount;
		}
		else {
			++validCount;
			totalPayloadBytes += p.length;
		}
    }

    	double validRate() const {
		if (total() == 0) return 0.0;
		return (double)validCount / total();
	}

	int valid() const { return validCount; }
	int invalid() const { return invalidCount; }
	int total() const { return validCount + invalidCount; }
	int payloadBytes() const { return totalPayloadBytes; }


private:
	int validCount = 0;
	int invalidCount = 0;
	int totalPayloadBytes = 0;
};

void printStats(const PacketStats& s);