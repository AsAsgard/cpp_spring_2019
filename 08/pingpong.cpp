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
    unique_lock<mutex> lock(m);
    while (N != 0)
    {
        cout << "ping" << '\n';
        --N;
        if (N == 0) ready = true;
        pp = PingPong::PONG;
        cond.notify_one();
        while (pp != PingPong::PING) cond.wait(lock);
    }
}

void pong()
{
    unique_lock<mutex> lock(m);
    while (!ready)
    {
        while (pp != PingPong::PONG) cond.wait(lock);
        cout << "pong" << '\n';
        pp = PingPong::PING;
        cond.notify_one();
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
