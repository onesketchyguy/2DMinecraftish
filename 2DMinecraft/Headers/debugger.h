// Forrest Lowe 2021

#pragma once

#ifndef DEBUGGER
#define DEBUGGER

#include "fileManager.h"

uint64_t debug_prints;

void print(std::string text) {
	// Print to console.
	std::cout << debug_prints << ":" << text << std::endl;

	std::string outPut = debug_prints == 0 ? "\n\n" : "";
	outPut += std::to_string(debug_prints) + ":" + text + "\n";

	// We may also want to save this data to a text file.
	FILESYSTEM::WriteFile("Data/output_log.txt", outPut);
	debug_prints++;
}

#endif // !DEBUGGER