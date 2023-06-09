#include"z_engine.h"



using namespace zengine;
using namespace std;
using namespace zengine::math;


void test(int a) {

}


int main(){
    Size32Type test_number(0xff800000U);
    Float32 asd = test_number.float_32;
    if (asd == test_number.float_32) {
        cout << 1;
    }
    
    const int a = 1;
    test(a);
    Float64 sample_frquency = 2500.0;
    IndexType sample_size = 8192;
    Float64 signal_frquency = 77.0;
    Float64 pole_frequency = 100.0;
    Float64 pole_damping = 0.7;
    Float64 signal_amplitude = 4;

    Float64 sample_period = 1.0 / sample_frquency;
    Float64 time = 0;

    //for (IndexType index = 0; index < sample_size; index++) {
    //    printf("%6d  %10f  \n", index, Sin(time * k2PI64 * signal_frquency * 1.0));
    //    time += sample_period;
    //}
    ZArray<Float64> time_domain_pre(sample_size);
    for (IndexType index = 0; index < sample_size; index++) {
        Float64 value = 0;
        value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 0.5);
        value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 1.0);
        value += signal_amplitude * Sin(time * k2PI64 * signal_frquency * 2);
        time_domain_pre.Push(value);
        time += sample_period;
    }



    ZArray<Float64> time_domain_temp(sample_size);
    DigitalSignalLPF2(&time_domain_temp, time_domain_pre, sample_period, pole_frequency, pole_damping);

    ZArray<ZComplex<Float64>> time_domain(sample_size);
    ZArray<ZComplex<Float64>> frquency_domain;
    for (IndexType index = 0; index < sample_size; ++index) {
        time_domain.Push(ZComplex<Float64>(time_domain_temp(index), 0.0));
    }
    //for (IndexType index = 0; index < time_domain.size(); ++index) {
    //    printf("%4d  ", index);
    //    cout << time_domain(index).real << "  ";
    //    cout << time_domain(index).imag << endl;
    //}
    FourierFFT(&frquency_domain, time_domain);
    for (IndexType index = 0; index < frquency_domain.size() / 2; ++index) {
        printf("%10f  ", index * (sample_frquency / frquency_domain.size()));
        //cout << frquency_domain(index).real / frquency_domain.size() * 2 << "  ";
        //cout << frquency_domain(index).imag / frquency_domain.size() * 2 << "  ";
        //cout << frquency_domain(frquency_domain.size() - 1 - index).real / frquency_domain.size() * 2 << "  ";
        //cout << frquency_domain(frquency_domain.size() - 1 - index).imag / frquency_domain.size() * 2 << endl;
        cout << frquency_domain(index).magnitude() / frquency_domain.size() * 2 << "  ";
        cout << frquency_domain(frquency_domain.size() - index - 1).magnitude() / frquency_domain.size() * 2 << endl;
    }

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
