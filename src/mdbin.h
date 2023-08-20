#pragma once

#define MDBIN_VERSION_MAJOR 0U
#define MDBIN_VERSION_MINOR 1U

#include <filesystem>
#include <string>
#include <vector>

namespace MDBIN
{
enum class Status { good = 0, syntaxError = 1, ioError = 2 };
struct Data
{
    uint64_t initStep = 0UL;
    uint64_t endStep = 0UL;
    uint64_t deltaStep = 0UL;
    std::vector<std::string> columnLabels;
    uint64_t numFrames = 0UL;
    uint64_t numAtoms = 0UL;
    uint32_t numCols = 0UL;
    std::vector<double> data;
};
Status write(const std::filesystem::path&, const Data&);
Status read(const std::filesystem::path&, Data&);
}
