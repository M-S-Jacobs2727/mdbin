#include "mdbin.h"

#include <cstdint>
#include <fstream>
#include <sstream>

namespace MDBIN
{
Status write(const std::filesystem::path& binfile, const HeaderInfo& info, const std::vector<double>& data)
{
    std::ofstream ofs(binfile, std::fstream::binary);
    // TODO: Checks for info.info.size(), info.columnLabels.size(), stepRange vs numFrames

    if (!ofs.good())
        return Status::ioError;
    
    ofs.write("\xa0\xb1\xb5mdbin", 8);
    
    uint32_t versionMajor = MDBIN_VERSION_MAJOR;
    uint32_t versionMinor = MDBIN_VERSION_MINOR;
    ofs.write(reinterpret_cast<const char*>(&versionMajor), sizeof(uint32_t));
    ofs.write(reinterpret_cast<const char*>(&versionMinor), sizeof(uint32_t));

    ofs.write(reinterpret_cast<const char*>(&info.initStep), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&info.endStep), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&info.deltaStep), sizeof(uint64_t));

    std::ostringstream oss(info.columnLabels[0]);
    for (auto it = info.columnLabels.begin() + 1; it != info.columnLabels.end(); ++it)
        oss << '\n' << *it;
    
    std::string columns = oss.str();
    uint64_t size = columns.size();

    ofs.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
    ofs.write(columns.c_str(), columns.size());

    ofs.write(reinterpret_cast<const char*>(&info.numFrames), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&info.numAtoms), sizeof(uint64_t));
    ofs.write(reinterpret_cast<const char*>(&info.numCols), sizeof(uint64_t));

    ofs.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(double));

    ofs.close();

    return Status::good;
}

Status append(const std::filesystem::path& binfile, const std::vector<double>& data)
{
    std::fstream iofs(binfile, std::fstream::binary | std::fstream::in | std::fstream::out);
    
    std::string word = "12345678";
    iofs.read(word.data(), 8);
    if (word != "\xa0\xb1\xb5mdbin")
        return Status::syntaxError;
    
    iofs.seekp(0, std::fstream::end);
    iofs.write(reinterpret_cast<const char*>(data.data()), data.size()*sizeof(double));
    iofs.close();

    return Status::good;
}

Status read(const std::filesystem::path& binfile, HeaderInfo& info, std::vector<double>& data)
{
    std::ifstream ifs(binfile, std::ios::binary);

    std::string word = "12345678";

    ifs.read(word.data(), 8);
    if (word != "\xa0\xb1\xb5mdbin")
        return Status::syntaxError;

    uint32_t ver_maj = 0, ver_min = 0;
    ifs.read(reinterpret_cast<char*>(&ver_maj), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&ver_min), sizeof(uint32_t));

    ifs.read(reinterpret_cast<char*>(&info.initStep), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&info.endStep), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&info.deltaStep), sizeof(uint64_t));

    uint32_t nchars = 0;
    ifs.read(reinterpret_cast<char*>(&nchars), sizeof(uint32_t));
    word.reserve(nchars);
    ifs.read(word.data(), nchars);

    ifs.read(reinterpret_cast<char*>(&info.numFrames), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&info.numAtoms), sizeof(uint64_t));
    ifs.read(reinterpret_cast<char*>(&info.numCols), sizeof(uint64_t));

    data.resize(info.numFrames * info.numAtoms * info.numCols);
    ifs.read(reinterpret_cast<char*>(data.data()), data.size());
    ifs.close();
    

    return Status::good;
}
}