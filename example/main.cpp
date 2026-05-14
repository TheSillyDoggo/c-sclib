#include <iostream>
#include <sclib/signatures.h>
#include <sclib/decompressor.h>
#include <vector>
#include <iostream>
#include <fstream>

std::vector<std::byte> loadFile(const char* path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file)
        return {};

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        return {};

    return buffer;
}

void writeData(void* ptr, size_t size, const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);

    if (outFile.is_open()) {
        outFile.write(static_cast<const char*>(ptr), size);
        outFile.close();
    }
}

int main()
{
    auto data = loadFile("character_abilities.csv");
    auto sig = scGetSignature(data.data(), data.size());

    void* out;
    size_t outSize;

    auto res = scDecompress(data.data(), data.size(), &out, &outSize);

    fprintf(stderr, "format: %i, res: %i, outSize: %lu\n", sig, res, outSize);

    writeData(out, outSize, "character_abilities.csv.txt");

    return 0;
}