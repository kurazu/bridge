#include <iostream>
#include <include/v8.h>
#include <include/libplatform/libplatform.h>
#include <boost/python.hpp>

using namespace v8;

void run_in_v8(const char* code) {
  V8::InitializeICU();
  Platform* platform = platform::CreateDefaultPlatform();
  V8::InitializePlatform(platform);
  V8::Initialize();

  // Create a new Isolate and make it the current one.
  Isolate* isolate = Isolate::New();
  {
    Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Create a new context.
    Local<Context> context = Context::New(isolate);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(isolate, code);

    // Compile the source code.
    Local<Script> script = Script::Compile(source);

    // Run the script to get the result.
    Local<Value> result = script->Run();

    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(result);
    std::cout << "Result: " << *utf8 << "\n";
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
}
// void run_in_v8(const char* code) {
//     std::cout << "Result: " << code << "\n";
// }

using namespace boost::python;

BOOST_PYTHON_MODULE(runjs)
{
    def("run", run_in_v8);
}
