#include<iostream>
#include "kernel/Kernel.h"
#include "kernel/DeviceRegistry.h"
#include "kernel/Clock.h"

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
    cout<<"Clock running: "<<(kernel.getClock().isRunning() ? "YES" : "NO")<<endl;
    cout << "Current tick count: " << Kernel::getTicks() << endl;

    cout << endl;
    cout << "Real-time clock running... System will auto-tick every 100ms" << endl;
    cout << "Press Enter to shutdown after observing ticks..." << endl;
    cout << endl;

    cin.get();
    
    cout << endl;
    cout << "Final tick count: " << Kernel::getTicks() << endl;
    cout << "Shutting down system..." << endl;
    kernel.shutdown();
    
    cout << "System halted." << endl;
    return 0;
}