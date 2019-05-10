#include <iostream>
#include <future>
#include <vector>
#include <fstream>
#include <array>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <cstdio>

using namespace std;

static constexpr size_t Nthreads = 4;
static constexpr size_t MEMORY = 8 * 1024 * 1024;
static constexpr size_t MAX_ELEMENTS = static_cast<size_t>(MEMORY / sizeof (uint64_t));
static atomic<size_t> ended(0);
static condition_variable waiter;
static string thread_filename = "thread_";
static const string input_filename = "input5.bar";
static const string output_filename = "output.bar";
static mutex mut_read;
static mutex mut_write;

void inplace_sort(ifstream& ifs, ofstream& ofs) {
    vector<uint64_t> values;
    values.reserve(static_cast<size_t>(MAX_ELEMENTS / Nthreads));
    while (true) {
        {
            lock_guard<mutex> lock(mut_read);
            size_t counter = 0;
            uint64_t value;
            values.clear();
            while (ifs && counter < values.capacity()) {
                ifs.read(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                if (ifs) {
                    values.push_back(value);
                    ++counter;
                }
            }
        }
        if (!values.empty()) {
            sort(values.begin(), values.end());
            {
                unique_lock<mutex> lock(mut_write);
                if (values.size() != static_cast<size_t>(MAX_ELEMENTS / Nthreads)) {
                    while(ended != Nthreads - 1) waiter.wait(lock);
                }
                ofs.write(reinterpret_cast<char *>(values.data()), sizeof (uint64_t) * values.size());
            }
        } else {
            break;
        }
    }
    ended += 1;
    if (ended == Nthreads - 1) waiter.notify_one();
}

void divide_file() {
    ifstream ifs("output.bar", fstream::binary);
    size_t thread_counter = 0;
    while (ifs) {
        string filename = thread_filename;
        filename = filename + to_string(thread_counter % Nthreads);
        ofstream ofs(filename, fstream::app | fstream::binary);
        ++thread_counter;
        uint64_t value = 0;
        size_t counter = 0;
        while (ifs && counter < static_cast<size_t>(MAX_ELEMENTS / Nthreads)) {
            ifs.read(reinterpret_cast<char *>(&value), sizeof (uint64_t));
            if (ifs) {
                ofs.write(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                ++counter;
            }
        }
    }
}

void remove_thread_files() {
    for (size_t th = 0; th < Nthreads; ++th) {
        string filename = thread_filename;
        filename = filename + to_string(th);
        remove(filename.c_str());
    }
}


template<size_t N>
void external_merge(const string& output_fname, const array<string, N>& input_fnames)
{
    ofstream ofilestream(output_fname, fstream::binary);
    array<ifstream, N> ifilestreams;
    array<uint64_t, N> values;
    vector<pair<size_t, uint64_t>> to_compare;
    for (size_t i = 0; i < N; ++i)
    {
        ifilestreams[i] = ifstream(input_fnames[i], fstream::binary);
        ifilestreams[i].read(reinterpret_cast<char *>(&values[i]), sizeof (uint64_t));
    }
    while (true) {
        to_compare.clear();
        for (size_t i = 0; i < N; ++i)
        {
            if (ifilestreams[i]) {
                to_compare.push_back(make_pair(i, values[i]));
            }
        }
        if (to_compare.size() == 1) {
            ofilestream.write(reinterpret_cast<char *>(&to_compare.front().second), sizeof (uint64_t));
            ofilestream << ifilestreams[to_compare.front().first].rdbuf();
            break;
        }
        auto it = min_element(to_compare.begin(),
                              to_compare.end(),
                              [] (pair<size_t, uint64_t>& lhs, pair<size_t, uint64_t>& rhs) {
                                  return lhs.second < rhs.second;
                              });
        ofilestream.write(reinterpret_cast<char *>(&(it->second)), sizeof (uint64_t));
        ifilestreams[it->first].read(reinterpret_cast<char *>(&values[it->first]), sizeof (uint64_t));
    }
}

void external_merge_sort(string&& filename, size_t num_thread)
{
    string tmp_filename_1 = thread_filename;
    string tmp_filename_2 = thread_filename;
    tmp_filename_1 = tmp_filename_1 + to_string(num_thread) + to_string(1);
    tmp_filename_2 = tmp_filename_2 + to_string(num_thread) + to_string(2);
    uint64_t value;
    size_t count_elems = 0;
    size_t iter = 1;
    do {
        {
            ifstream ifs(filename, fstream::binary);
            ofstream tmp_ofs_1(tmp_filename_1, fstream::binary);
            ofstream tmp_ofs_2(tmp_filename_2, fstream::binary);
            while (ifs) {
                size_t counter = 0;
                while (ifs && counter < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads)) {
                    ifs.read(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                    if (ifs) {
                        tmp_ofs_1.write(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                        ++counter;
                    }
                }
                if (iter == 1) count_elems += counter;
                counter = 0;
                while (ifs && counter < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads)) {
                    ifs.read(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                    if (ifs) {
                        tmp_ofs_2.write(reinterpret_cast<char *>(&value), sizeof (uint64_t));
                        ++counter;
                    }
                }
                if (iter == 1) count_elems += counter;
            }
        }
        {
            ofstream ofs(filename, fstream::binary);
            ifstream tmp_ifs_1(tmp_filename_1, fstream::binary);
            ifstream tmp_ifs_2(tmp_filename_2, fstream::binary);
            while (tmp_ifs_1 && tmp_ifs_2) {
                size_t counter1 = 0;
                size_t counter2 = 0;
                bool need_to_read_1 = true;
                bool need_to_read_2 = true;
                uint64_t value1, value2;
                while (tmp_ifs_1 && tmp_ifs_2 &&
                       counter1 < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads) &&
                       counter2 < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads)) {
                    if (need_to_read_1) {
                        tmp_ifs_1.read(reinterpret_cast<char *>(&value1), sizeof (uint64_t));
                        if (!tmp_ifs_1) break;
                        need_to_read_1 = false;
                    }
                    if (need_to_read_2) {
                        tmp_ifs_2.read(reinterpret_cast<char *>(&value2), sizeof (uint64_t));
                        if (!tmp_ifs_2) break;
                        need_to_read_2 = false;
                    }
                    if (value1 < value2) {
                        ofs.write(reinterpret_cast<char *>(&value1), sizeof (uint64_t));
                        ++counter1;
                        need_to_read_1 = true;
                    } else {
                        ofs.write(reinterpret_cast<char *>(&value2), sizeof (uint64_t));
                        ++counter2;
                        need_to_read_2 = true;
                    }
                }
                while (counter1 < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads) && tmp_ifs_1) {
                    if (need_to_read_1) {
                        tmp_ifs_1.read(reinterpret_cast<char *>(&value1), sizeof (uint64_t));
                        if (!tmp_ifs_1) break;
                    }
                    ofs.write(reinterpret_cast<char *>(&value1), sizeof (uint64_t));
                    ++counter1;
                    need_to_read_1 = true;
                }
                while (counter2 < iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads) && tmp_ifs_2) {
                    if (need_to_read_2) {
                        tmp_ifs_2.read(reinterpret_cast<char *>(&value2), sizeof (uint64_t));
                        if (!tmp_ifs_2) break;
                    }
                    ofs.write(reinterpret_cast<char *>(&value2), sizeof (uint64_t));
                    ++counter2;
                    need_to_read_2 = true;
                }
            }
        }
        iter *= 2;
    } while(count_elems > iter * static_cast<size_t>(MAX_ELEMENTS / Nthreads));
    remove(tmp_filename_1.c_str());
    remove(tmp_filename_2.c_str());
}

int main()
{
    vector<future<void>> futures;
    ifstream ifilestream(input_filename, fstream::binary);
    ofstream ofilestream(output_filename, fstream::binary);
    for (size_t th = 0; th < Nthreads; ++th) {
        futures.push_back(async(inplace_sort, ref(ifilestream), ref(ofilestream)));
    }
    for (auto& fut: futures) {
        fut.get();
    }
    ended = 0;
    ifilestream.close();
    ofilestream.close();

    remove_thread_files();
    divide_file();

    futures.clear();

    array<string, Nthreads> filenames;


    for (size_t th = 0; th < Nthreads; ++th) {
        string filename = thread_filename;
        filename = filename + to_string(th);
        filenames[th] = filename;
        futures.push_back(async(external_merge_sort, move(filename), th));
    }
    for (auto& fut: futures) {
        fut.get();
    }

    external_merge(output_filename, filenames);

    remove_thread_files();

    return 0;
}
