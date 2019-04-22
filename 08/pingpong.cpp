#include <mutex>
#include <iostream>
#include <condition_variable>
#include <thread>

using namespace std;

enum class PingPong {
    PING = 0,
    PONG = 1
};

static mutex m;
static condition_variable cond;
static PingPong pp;
static bool ready = false;

void ping(size_t N)
{
    while (N != 0)
    {
        while (pp != PingPong::PING) this_thread::yield();
        unique_lock<mutex> lock(m);
        cout << "ping" << '\n';
        --N;
        pp = PingPong::PONG;
        if (N == 0) ready = true;
        cond.notify_one();
    }
}

void pong()
{
    unique_lock<mutex> lock(m);
    while (!ready)
    {
        pp = PingPong::PING;
        cond.wait(lock);
        cout << "pong" << '\n';
    }
}

int main()
{
    pp = PingPong::PONG;
    size_t N = 500000;
    thread second_thread(pong);
    ping(N);
    second_thread.join();
}
