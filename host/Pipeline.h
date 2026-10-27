#pragma once

#include "DataSource.h"
#include "PacketReader.h"


void runPipeline(DataSource& source, PacketReader& reader, int maxReads);
