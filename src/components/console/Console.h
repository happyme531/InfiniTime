#pragma once


#include <cstdint>
#include <cstddef>

#include "systemtask/SystemTask.h"

namespace Pinetime {

  namespace System {
    class SystemTask;
  }

namespace Components
{


  class Console {
    public:
      Console(Pinetime::System::SystemTask& systemTask);

    private:
        Pinetime::System::SystemTask& systemTask;

    };
}
}
