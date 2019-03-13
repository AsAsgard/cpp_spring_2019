#include "numbers.dat"
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);
    if (argc < 3 || argc % 2 == 0) return -1;
    for (int i = 1; i < argc; i += 2)
    {
        int left_boundary_value = std::atoi(argv[i]);
        int right_boundary_value = std::atoi(argv[i+1]);
        if (left_boundary_value > right_boundary_value)
        {
            cout << 0 << '\n';
            continue;
        }

        auto left_boundary_ptr = std::lower_bound(Data, Data + Size, left_boundary_value);
        if (left_boundary_ptr == Data + Size)
        {
            cout << 0 << '\n';
            continue;
        }
        auto right_boundary_ptr = std::upper_bound(Data, Data + Size, right_boundary_value);
        if (right_boundary_ptr == Data + Size && right_boundary_value != Data[Size - 1])
        {
            cout << 0 << '\n';
            continue;
        }

        vector<int> Primes(right_boundary_value + 1);
        Primes[0] = 1;
        Primes[1] = 1;
        for (int k = 2; k*k <= right_boundary_value; ++k)
        {
            if (Primes[k] == 0)
            {
                for (int j = k*k; j <= right_boundary_value; j += k)
                {
                    Primes[j] = 1;
                }
            }
        }

        int primesCount = 0;
        int tmpCounter = 0;
        while (left_boundary_ptr != right_boundary_ptr)
        {
            tmpCounter = 0;
            left_boundary_value = *left_boundary_ptr;
            while(left_boundary_ptr != right_boundary_ptr &&
                  left_boundary_value == *left_boundary_ptr)
            {
                ++tmpCounter;
                ++left_boundary_ptr;
            }
            if (Primes[left_boundary_value] == 0) primesCount += tmpCounter;
        }

        cout << primesCount << '\n';
    }
    return 0;
}
