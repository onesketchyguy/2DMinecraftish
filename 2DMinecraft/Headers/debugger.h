// Forrest Lowe 2021

#pragma once

#ifndef DEBUGGER
#define DEBUGGER

#include "fileManager.h"

inline uint64_t debug_prints;

inline void clearDebugFile() {
	debug_prints = 0;

	std::remove("Data/output_log.txt");
}

inline void printError(std::string text) {
	// Print to console.
	std::string outputText = std::to_string(debug_prints) + ":" + "-ERROR- " + text + "\n";
	std::cout << outputText;

	std::string outPut = debug_prints == 0 ? "\n\n" : "";
	outPut += outputText;

	// We may also want to save this data to a text file.
	FILESYSTEM::WriteFile("Data/output_log.txt", outPut);
	debug_prints++;
}

inline void print(std::string text) {
	// Print to console.
	std::string outputText = std::to_string(debug_prints) + ":" + text + "\n";
	std::cout << outputText;

	std::string outPut = debug_prints == 0 ? "\n\n" : "";
	outPut += outputText;

	// We may also want to save this data to a text file.
	FILESYSTEM::WriteFile("Data/output_log.txt", outPut);
	debug_prints++;
}

#endif // !DEBUGGER