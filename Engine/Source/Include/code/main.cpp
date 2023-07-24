#include "z_engine.h"

using namespace zengine;
using namespace std;


#include <iomanip>
#include <vector>
#include <queue>
#include <deque>
#include "z_core/z_vector.h"


struct DataNode {
public:
    DataNode* next_node_ptr;
    DataNode* previous_node_ptr;
    IndexType capacity;
    IndexType size;
    IndexType a[1];
    IndexType b[10];
};

constexpr auto init_function = [](ZArray<Int32, 10>* array_ptr) {
    for (IndexType index = 0; index < array_ptr->size(); ++index) {
        (*array_ptr)[index] = 1;
    }
};
int main(){
    ZVector<int> a;
    ZVector<int> b;
    ZVector<ZVector<int>> c;
    ZVector<ZVector<int>> d;
    for (int i = 0; i < 6; i++)
    {
        a.PushBack(i);
        c.PushBack(a);
    }
    d = c;
    b = a;
    auto it = b.Erases(b.ReverseEnd() - 2, 2);
    it = b.Erase(it);

    //cout << (-4) % 3;
    //ZArray<Int32, 19> test_1;
    //ZArray<Int32, 19> test_2(test_1);

    //ZVector<Int32> vector_1;
    //for (IndexType index = 1; index < 13; ++index) {
    //    vector_1.PushBacks(-10,index);
    //}
    //ZVector<ZVector<Int32>> vector_2;
    //ZVector<ZVector<Int32>> vector_3;
    //for (IndexType index = 1; index < 13; ++index) {
    //    vector_1.PushBack(index);
    //    vector_2.PushBack(vector_1);
    //}
    //auto begin = vector_2.ReverseBegin();
    //auto end = vector_2.ReverseEnd();
    //vector_2.Assign(begin, end);
    //vector_3 = vector_2;
    //for (IndexType i = 0; i < vector_3.size(); ++i) {
    //    for (IndexType j = 0; j < vector_3[i].size(); ++j) {
    //        cout << vector_3[i][j] << " ";
    //    }
    //    cout << endl;
    //}

    //Int32* test_1 = new Int32[1024 * 1024 * 100];
    //Int32* test_2 = new Int32[1024 * 1024 * 100];

    //for (IndexType index = 0; index < 1024 * 1024; index++) {
    //    test_1[index] = index;
    //}
    //TIME_TEST(memmove(test_1, test_2, 4LL * 1024LL * 1024LL * 100 - 10));
    //TIME_TEST(memcpy(test_1, test_2, 4LL * 1024LL * 1024LL * 100 - 10););
  
    /*求2到100000000的所有质数*/
    //int n = 100000000;
    //bool flag;
    //for (int i = 2; i <= n; i++) {
    //    flag = false;
    //    for (int j = 2; j < i; j++) {
    //        if (i % j == 0) {
    //            flag = true;
    //        }
    //    }
    //    if (flag == true) {
    //        cout << i << endl;
    //    }
    //}

    //std::cout << std::setprecision(20);  // 设置输出的精度为18位有效数字
    ////Float64 b = 0.01;
    ////for (Int32 index = 1; index <= 200; ++index) {
    ////    if (zengine::math::internal::LnCalculateA(0.01F * index) != 
    ////        zengine::math::internal::LnCalculateB(0.01F * index)) {
    ////        cout << index <<"  " << zengine::math::internal::LnCalculateA(0.01F * index) << "  " << zengine::math::internal::LnCalculateB(0.01F * index) << endl;
    ////    }
    ////}
    //Float64 a = 1.0;
    //for (Int32 index = 1; index <= 200; ++index) {
    //    cout << index << "  " << FactorialReciprocal<Float64>(index) * pow(LnA(2.0), index) << endl;
    //}
    //ZArray<Int32> array(1000);
    //ZVector<ZVector<Int32>> vector1;
    //ZVector<Int32> ele;
    //vector1.PushBack(std::move(ele));
    //auto itarator = vector1.ConstBegin();
    //test<Int32>(1,
    //    [](Int32 a, Int32 b) -> Bool {
    //    return a > b;
    //    });
    //Size32Type test_number(0xff800000U);
    //Float32 asd = test_number.float_32;
    //if (asd == test_number.float_32) {
    //    cout << 1;s
    //}
    //const int a = 1;
    //test(a);
    //Float64 sample_frquency = 2500.0;
    //IndexType sample_size = 8192;
    //Float64 signal_frquency = 77.0;
    //Float64 pole_frequency = 100.0;
    //Float64 pole_damping = 0.7;
    //Float64 signal_amplitude = 4;

    //Float64 sample_period = 1.0 / sample_frquency;
    //Float64 time = 0;

    ////for (IndexType index = 0; index < sample_size; index++) {
    ////    printf("%6d  %10f  \n", index, Sin(time * k2PI64 * signal_frquency * 1.0));
    ////    time += sample_period;
    ////}
    //ZArray<Float64> time_domain_pre(sample_size);
    //for (IndexType index = 0; index < sample_size; index++) {
    //    Float64 value = 0;
    //    value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 0.5);
    //    value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 1.0);
    //    value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 2);
    //    time_domain_pre.Push(value);
    //    time += sample_period;
    //}



    //ZArray<Float64> time_domain_temp(sample_size);
    //DigitalSignalLPF2(&time_domain_temp, time_domain_pre, sample_period, pole_frequency, pole_damping);

    //ZArray<ZComplex<Float64>> time_domain(sample_size);
    //ZArray<ZComplex<Float64>> frquency_domain;
    //for (IndexType index = 0; index < sample_size; ++index) {
    //    time_domain.Push(ZComplex<Float64>(time_domain_temp(index), 0.0));
    //}
    ////for (IndexType index = 0; index < time_domain.size(); ++index) {
    ////    printf("%4d  ", index);
    ////    cout << time_domain(index).real << "  ";
    ////    cout << time_domain(index).imag << endl;
    ////}
    //FourierFFT(&frquency_domain, time_domain);
    //for (IndexType index = 0; index < frquency_domain.size() / 2; ++index) {
    //    printf("%10f  ", index * (sample_frquency / frquency_domain.size()));
    //    //cout << frquency_domain(index).real / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(index).imag / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(frquency_domain.size() - 1 - index).real / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(frquency_domain.size() - 1 - index).imag / frquency_domain.size() * 2 << endl;
    //    cout << frquency_domain(index).magnitude() / frquency_domain.size() * 2 << "  ";
    //    cout << frquency_domain(frquency_domain.size() - index - 1).magnitude() / frquency_domain.size() * 2 << endl;
    //}

    //frquency_domain.Clear();
    //frquency_domain.PushEmpty(sample_size);
    //memset(&frquency_domain(0), 0, frquency_domain.size() * sizeof(ZComplex<Float64>));
    //frquency_domain(10) = ZComplex<Float64>(0, -1);
    //FourierIFFT(&time_domain, frquency_domain);

    //for (IndexType index = 0; index < time_domain.size(); ++index) {
    //    printf("%10f  ", sample_distance * index);
    //    cout << time_domain(index).real << "  ";
    //    cout << time_domain(index).imag << endl;
    //}

    //Float64 sample_frquency = 2500.0F;
    //IndexType sample_size = 8192;
    //Float64 signal_frquency = 77;
    //Float64 signal_amplitude = 4;
    ////    Float32 frquency = (rand() % 40) + 60;
    ////    Float32 amplitude = rand() % 30;

    //Float64 sample_distance = 1.0 / sample_frquency;
    //Float64 time = 0;
    //ZArray<ZComplex<Float64>> time_domain(sample_size);
    //ZArray<ZComplex<Float64>> frquency_domain;
    //for (IndexType index = 0; index < sample_size; index++) {
    //    time_domain.Push(ZComplex<Float64>(signal_amplitude * Sin(time * k2PI64 * signal_frquency) + 1, 0.0));
    //    time += sample_distance;
    //}
    ////for (IndexType index = 0; index < time_domain.size(); ++index) {
    ////    printf("%4d  ", index);
    ////    cout << time_domain(index).real << "  ";
    ////    cout << time_domain(index).imag << endl;
    ////}
    //FourierFFT(&frquency_domain, time_domain);
    //for (IndexType index = 0; index < frquency_domain.size() / 2; ++index) {
    //    printf("%10f  ", index * (sample_frquency / frquency_domain.size()));
    //    //cout << frquency_domain(index).real / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(index).imag / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(frquency_domain.size() - 1 - index).real / frquency_domain.size() * 2 << "  ";
    //    //cout << frquency_domain(frquency_domain.size() - 1 - index).imag / frquency_domain.size() * 2 << endl;
    //    cout << frquency_domain(index).magnitude() / frquency_domain.size() * 2 << "  ";
    //    cout << frquency_domain(frquency_domain.size() - index - 1).magnitude() / frquency_domain.size() * 2 << endl;
    //}

    //frquency_domain.Clear();
    //frquency_domain.PushEmpty(sample_size);
    //memset(&frquency_domain(0), 0, frquency_domain.size() * sizeof(ZComplex<Float64>));
    //frquency_domain(10) = ZComplex<Float64>(0, -1);
    //FourierIFFT(&time_domain, frquency_domain);

    //for (IndexType index = 0; index < time_domain.size(); ++index) {
    //    printf("%10f  ", sample_distance * index);
    //    cout << time_domain(index).real << "  ";
    //    cout << time_domain(index).imag << endl;
    //}

    return 0;
}
