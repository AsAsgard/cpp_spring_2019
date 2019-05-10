#include <iostream>
#include <fstream>
#include <random>
#include <memory>

using namespace std;
/*
int main()
{
    cout << "Enter filename: ";
    string filename;
    cin >> filename;
    cout << "Enter logfilename: ";
    string logfilename;
    cin >> logfilename;
    ofstream log(logfilename);
    ifstream filestream(filename, fstream::binary);
    uint64_t value;
    size_t counter = 0;
    while (filestream) {
        filestream.read(reinterpret_cast<char *>(&value), sizeof (uint64_t));
        if (filestream) {
            log << value << '\n';
            ++counter;
        }
    }
    cout << counter;
    return 0;
}
*/
