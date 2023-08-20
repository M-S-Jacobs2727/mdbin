#include "mdbin.h"

#include <cstdint>
#include <fstream>
#include <sstream>

namespace MDBIN
{
Status write(const std::filesystem::path& binfile, const Data& data)
{
    std::ofstream ofs(binfile, std::ios::binary);
    // TODO: Checks for data.data.size(), data.columnLabels.size(), stepRange vs numFrames

    if (!ofs.good())
        return Status::ioError;
    
    ofs.write("\xa0\xb1\xb5mdbin", 8);
    
    uint32_t versionMajor = MDBIN_VERSION_MAJOR;
    uint32_t versionMinor = MDBIN_VERSION_MINOR;
    ofs.write(reinterpret_cast<const char*>(&versionMajor), sizeof(uint32_t));
    ofs.write(reinterpret_cast<const char*>(&versionMinor), sizeof(uint32_t));

    ofs.write(reinterpret_cast<const char*>(&data.initStep), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&data.endStep), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&data.deltaStep), sizeof(uint64_t));

    std::ostringstream oss(data.columnLabels[0]);
    for (auto it = data.columnLabels.begin() + 1; it != data.columnLabels.end(); ++it)
        oss << '\n' << *it;
    
    std::string columns = oss.str();
    uint64_t size = columns.size();

    ofs.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
    ofs.write(columns.c_str(), columns.size());

    ofs.write(reinterpret_cast<const char*>(&data.numFrames), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&data.numAtoms), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&data.numCols), sizeof(uint64_t));

    ofs.write(reinterpret_cast<const char*>(data.data.data()), data.data.size() * sizeof(double));

    ofs.close();

    return Status::good;
}

Status read(const std::filesystem::path& binfile, Data& data)
{
    std::ifstream ifs(binfile, std::ios::binary);

    std::string word = "12345678";

    ifs.read(word.data(), 8);
    if (word != "\xa0\xb1\xb5mdbin")
        return Status::syntaxError;

    uint32_t ver_maj = 0, ver_min = 0;
    ifs.read(reinterpret_cast<char*>(&ver_maj), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&ver_min), sizeof(uint32_t));

    ifs.read(reinterpret_cast<char*>(&data.initStep), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&data.endStep), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&data.deltaStep), sizeof(uint64_t));

    uint32_t nchars = 0;
    ifs.read(reinterpret_cast<char*>(&nchars), sizeof(uint32_t));
    word.reserve(nchars);
    ifs.read(word.data(), nchars);

    ifs.read(reinterpret_cast<char*>(&data.numFrames), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&data.numAtoms), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&data.numCols), sizeof(uint64_t));

    data.data.resize(data.numFrames * data.numAtoms * data.numCols);
    ifs.read(reinterpret_cast<char*>(data.data.data()), data.data.size());
    ifs.close();
    

    return Status::good;
}
}