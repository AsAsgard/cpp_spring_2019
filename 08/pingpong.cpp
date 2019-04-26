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
size_t N = 500000;

void ping()
{
    unique_lock<mutex> lock(m);
    while (N != 0)
    {
        cout << "ping" << '\n';
        --N;
        pp = PingPong::PONG;
        cond.notify_one();
        while (pp != PingPong::PING) cond.wait(lock);
    }
}

void pong()
{
    unique_lock<mutex> lock(m);
    while (N != 0)
    {
        while (pp != PingPong::PONG) cond.wait(lock);
        cout << "pong" << '\n';
        pp = PingPong::PING;
        cond.notify_one();
    }
}

int main()
{
    pp = PingPong::PING;
    thread second_thread(pong);
    ping();
    second_thread.join();
}
