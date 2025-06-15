#include<iostream>
#include<chrono>
#include<thread>
#include "kernel/Kernel.h"
#include "kernel/DeviceRegistry.h"

using namespace std;

int main(int argc, char* argv[]){
    cout<<"=== vOS - Virtual Operating system ==="<<endl;
    cout<<"Runnning in: "<< (argc>1 ? argv[1] : "local")<<"environment"<<endl;
    cout<<endl;

    auto& kernel = Kernel::getInstance();

    if (!kernel.initialize()) {
        cerr<<"Failed to initialise kernel!"<<endl;
        return 1;
    }
    cout<<endl;
    cout<<"===System Status==="<<endl;
    cout<<"Kernel Initialised : " <<(kernel.isInitialized() ? "YES":"NO")<<endl;
    cout << "Device count: " << kernel.getDeviceRegistry().getDeviceCount() << endl;
    cout << "Current tick count: " << Kernel::getTicks() << endl;

    cout <<endl;
    cout << "Simulating system ticks..." <<endl;
    for (int i = 0; i < 5; ++i) {
        Kernel::incrementTicks();
        cout << "Tick: " << Kernel::getTicks() <<endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    cout << endl;
    cout << "Shutting down system..." << endl;
    kernel.shutdown();
    
    cout << "System halted." << endl;
    return 0;
}