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
    ifstream filestream(filename, fstream::binary);
    uint64_t current, previous = 0;
    size_t counter = 0;
    do {
        filestream.read(reinterpret_cast<char *>(&current), sizeof (uint64_t));
        if (filestream) {
            ++counter;
            if (current < previous) {
                cout << counter << ' ';
                cout << "False" << '\n';
            }
            previous = current;
        }
    } while (filestream);
    cout << counter << ' ';
    cout << "END" << '\n';
    char ch;
    cin >> ch;
    return 0;
}
*/
