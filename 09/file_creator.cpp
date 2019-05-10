#include <iostream>
#include <fstream>
#include <random>

using namespace std;

static size_t N = 2000000;
/*
int main()
{
    ofstream log("log.log");
    ofstream filestream("input5.bar", fstream::binary);
    random_device device;
    mt19937_64 generator(device());
    for (size_t i = 0; i < N; ++i) {
        uint64_t value = static_cast<uint64_t>(generate_canonical<double, 64>(generator) * UINT64_MAX);
        if (i<50 || i>(N-50))
            log << value << '\n';
        filestream.write(reinterpret_cast<char *>(&value), sizeof (uint64_t));
    }
    return 0;
}
*/
