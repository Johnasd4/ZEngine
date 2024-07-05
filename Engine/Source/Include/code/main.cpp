/*
    Copyright (c) YuLin Zhu (朱雨林)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (朱雨林)
    Contact: 1152325286@qq.com
*/
#define PROJECT_NAME L"Include"

#include "z_engine.h"

#include <memory>

using namespace zengine;
using namespace std;

Int32 test_func(Int32 &test) {
    test = test + 1;
    cout << test << endl;
    return test;
}

//void* operator new(size_t size) {
//    if (size == sizeof(int)) {
//        return intPool.allocate();
//    }
//    else if (size == sizeof(long)) {
//        return longPool.allocate();
//    }
//    else {
//        return ::operator new(size);
//    }
//}
//
//void operator delete(void* ptr, size_t size) noexcept {
//    if (size == sizeof(int)) {
//        intPool.deallocate(ptr);
//    }
//    else if (size == sizeof(long)) {
//        longPool.deallocate(ptr);
//    }
//    else {
//        ::operator delete(ptr);
//    }
//}
//
//void* operator new[](size_t size) {
//    return ::operator new(size);
//    }
//
//    void operator delete[](void* ptr) noexcept {
//        ::operator delete(ptr);
//        }

struct TMemoryBlockBase {
    FORCEINLINE Void Initialize(Void* _pool_ptr) noexcept {}
};

int main() {
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%x", 3, 4, 5, 6, ~7);
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_MESSAGE(L"Message...");
    Z_LOG_START(L"Start...");
    Z_LOG_PROCESS(L"Process 1...");
    Z_LOG_PROCESS(L"Process 2...");
    Z_LOG_PROCESS(L"Process 3...");
    Z_LOG_FINISH(L"Finish...");
    Z_LOG_SUCCESS(L"Success...");
    Z_LOG_FAILURE(L"Failure...");
    //std::thread prod(producer);
    //std::thread cons(consumer);
    //prod.join();
    //cons.join();
    //ZSemMutex test_mutex;
    //test_mutex.Lock();
    //ZThreadPool thread_pool(1);
    //{
    //    Int32 i = 1;
    //    //thread_pool.AddTask(test_func, Ref(i));
    //}
    //Sleep(100);
    //ZTask test_task([]() {cout<<1; });
    //auto test_lambda = [&](Int32 i) {cout << i + 1; };
    //thread_pool.AddTask(test_lambda, 1);
    ////test_task.Run();
    //TArray<Int32,10>* a = nullptr;
    //delete a;
    //sizeof(TTuple<Int32>);
    //thread_pool.LockUntilTaskDone();
    for (Int32 i = 0; i < 10; ++i) {
        auto sp = std::allocate_shared<int>(TSmartPointerAllocator<TArray<Int32, 10>>());
    }
    auto b = MakeShared<Int32>(3);
    StaticPointerCast<Int32>(b);
    ZString string("123");
    string == string;
    //sizeof(_Ref_count_base);
    //ZString temp_str_0 = "123";
    //ZString temp_str_1 = temp_str_0 + "1234";
    //cout << temp_str_1.String() << endl;
    //cout << endl << ("123" == temp_str_1) << endl;
    //std::string f = "123";
    //sizeof(f.find("1"));
    //cout << temp_str_1.StartWith(temp_str_0) << endl;
    Sleep(100);
    return 0;
}
