#include <nan.h>
#include <unistd.h>

#include <list>

std::list<uint32_t> clicks;

using namespace Nan;

#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hid/IOHIDManager.h>

void myHIDMouseCallback( void* context,  IOReturn result,  void* sender,  IOHIDValueRef value ) {
  IOHIDElementRef elem = IOHIDValueGetElement( value );
  uint32_t usage = IOHIDValueGetIntegerValue( value );
  uint32_t button = IOHIDElementGetUsage( elem );

  //assuming max 5 buttons on mouse, filtering the scroll events
  if (usage != 1 || button > 5) {
    return; //not a mouse click
  }

  clicks.push_back(button);
}

CFMutableDictionaryRef myCreateDeviceMatchingDictionary( UInt32 usagePage,  UInt32 usage ) {
  CFMutableDictionaryRef dict = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0
    , & kCFTypeDictionaryKeyCallBacks
    , & kCFTypeDictionaryValueCallBacks );
  if ( ! dict ) {
    return NULL;
  }
  CFNumberRef pageNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, & usagePage );
  if ( ! pageNumberRef ) {
    CFRelease( dict );
    return NULL;
  }
  CFDictionarySetValue( dict, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef );
  CFRelease( pageNumberRef );
  CFNumberRef usageNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, & usage );
  if ( ! usageNumberRef ) {
    CFRelease( dict );
    return NULL;
  }
  CFDictionarySetValue( dict, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef );
  CFRelease( usageNumberRef );
  return dict;
}

class MouseListenerWorker : public AsyncProgressWorker {
 public:
  MouseListenerWorker(Callback *callback, Callback *progress)
    : AsyncProgressWorker(callback), progress(progress) {
      this->hidManager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDOptionsTypeNone );
      CFArrayRef matches;
      {
        CFMutableDictionaryRef mouse = myCreateDeviceMatchingDictionary( 0x01, 2 );
        CFMutableDictionaryRef matchesList[] = { mouse };
        matches = CFArrayCreate( kCFAllocatorDefault, (const void **)matchesList, 1, NULL );
      }
      IOHIDManagerSetDeviceMatchingMultiple( this->hidManager, matches );
    }

  void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
    IOHIDManagerRegisterInputValueCallback( this->hidManager, myHIDMouseCallback, NULL);
    IOHIDManagerScheduleWithRunLoop( this->hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode );
    IOHIDManagerOpen( this->hidManager, kIOHIDOptionsTypeNone );

    while(true) {
      CFRunLoopRunInMode(kCFRunLoopDefaultMode, 60, TRUE);
      while (!clicks.empty()) {
        uint32_t button = clicks.front();
        progress.Send(reinterpret_cast<const char*>(&button), sizeof(uint32_t));
        clicks.pop_front();
      }
    }
  }

  void HandleProgressCallback(const char *data, size_t size) {
    Nan::HandleScope scope;
    uint32_t button = *reinterpret_cast<uint32_t*>(const_cast<char*>(data));
    v8::Local<v8::Value> argv[] = {
      New<v8::Int32>(button),
    };
    progress->Call(1, argv);
  }

 private:
  Callback *progress;
  IOHIDManagerRef hidManager;
};

NAN_METHOD(Listen) {
  Callback *progress = new Callback(info[0].As<v8::Function>());
  Callback *callback = new Callback(info[0].As<v8::Function>());
  AsyncQueueWorker(new MouseListenerWorker(callback, progress));
}

NAN_MODULE_INIT(Init) {
  Set(target
    , New<v8::String>("listen").ToLocalChecked()
    , New<v8::FunctionTemplate>(Listen)->GetFunction());
}

NODE_MODULE(my_addon, Init)
