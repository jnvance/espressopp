#ifndef _HELLO_HELLOWORLD_HPP
#define _HELLO_HELLOWORLD_HPP
#include "acconfig.hpp"
#include "pmi/pmi.hpp"
#include <string>
#include <vector>

namespace hello {
  // Worker class
  class HelloWorld {
    vector<std::string> allMessages;
  public:
    std::string getMessages();
    void createMessage();
  };

  // Proxy class
  class PHelloWorld : 
    pmi::ParallelObject<hello::HelloWorld> {
  public:
    PMI_PARALLEL_PROXY_METHOD(createMessage);

    std::string getMessages() {
      createMessage();
      return getLocalInstance().getMessages();
    }

    // expose the python registration
    static void registerPython();
  };
}

#endif