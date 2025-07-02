#include "DeviceRegistry.h"
#include <algorithm>
#include <cstddef>
#include <mutex>

using namespace std;

DeviceRegistry::DeviceRegistry() : initialized(false) {}

DeviceRegistry::~DeviceRegistry() { cleanup(); }

bool DeviceRegistry::initialize() {
  lock_guard<mutex> lock(registryMutex);
  if (initialized) {
    return true;
  }

  devices.emplace_back("SystemTimer", "Timer");
  devices.emplace_back("Console", "IO");
  devices.emplace_back("Logger", "System");
  devices.emplace_back("MemoryManager", "System");

  initialized = true;
  return initialized;
}

void DeviceRegistry::cleanup() {
  lock_guard<mutex> lock(registryMutex);

  for (auto &device : devices) {
    device.active = false;
  }

  devices.clear();
  initialized = false;
}

void DeviceRegistry::registerDevice(const string &name, const string &type) {
  lock_guard<mutex> lock(registryMutex);
  devices.emplace_back(name, type);
}
void DeviceRegistry::unregisterDevice(const string &name) {
  lock_guard<mutex> lock(registryMutex);
  devices.erase(
      remove_if(devices.begin(), devices.end(),
                [&name](const DeviceInfo &dev) { return dev.name == name; }),
      devices.end());
}

size_t DeviceRegistry::getDeviceCount() const{
    lock_guard<mutex> lock(registryMutex);
    return devices.size();
}

vector<DeviceInfo> DeviceRegistry::getDevices() const {
    lock_guard<mutex> lock(registryMutex);
    return devices;
}