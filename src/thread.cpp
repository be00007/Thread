#include<thread>
#include<iostream>
#include<condition_variable>
#include<queue>

std::mutex mtx;
std::queue<int> que;
std::condition_variable cond;

void Printf(int a) {
    std::cout << "Hello from thread, ID: " << std::this_thread::get_id() << std::endl;
    a++;
}

void CProduction(){
    for (int i = 0; i < 10; i++)
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            que.push(i);
            cond.notify_one();  //   本线程并不会释放锁，等作用域结束
            std::cout<<"Process:"<<i<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void CConsumer(){
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待的时候释放锁，条件满足的时候尝试获取原来的锁
        cond.wait(lock, [](){return !que.empty();}); // false的时候等待
        int value = que.front();
        que.pop();

        std::cout<<"Consumer"<<value<<std::endl;
    }
    
}

int main(){
    int test = 0;
    // 使用 std::ref 传递引用参数
    std::thread P(CProduction);
    std::thread C(CConsumer);
    P.join();
    C.join();
    return 0;
}
