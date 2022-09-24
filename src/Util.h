#pragma once
#include <cstdint>

bool GetCurrentModuleInfo(uint8_t*& codeBase, size_t& codeSectionSize);
uint8_t* SearchPattern(const uint16_t* pattern, size_t patternSize, uint8_t* ptr, size_t start, size_t size);
