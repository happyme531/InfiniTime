#pragma once


#include <cstdint>
#include <cstddef>

//#include "systemtask/SystemTask.h"

namespace Pinetime {

  namespace System {
    class SystemTask;
  }

  namespace Controllers
  {
    class NimbleController;
    class FS;
    class MotorController;
  }

  namespace Drivers
  {
    class Cst816S;
    class SpiNorFlash;
    class TwiMaster;
  }
  

  namespace Components
  {
    class LittleVgl;

    class Console {
      public:
        Console(Pinetime::System::SystemTask& systemTask,
                Pinetime::Controllers::NimbleController& nimbleController,
                Pinetime::Controllers::FS& fs,
                Pinetime::Components::LittleVgl& lvgl,
                Pinetime::Controllers::MotorController& motorController,
                Pinetime::Drivers::Cst816S& touchPanel,
                Pinetime::Drivers::SpiNorFlash& spiNorFlash,
                Pinetime::Drivers::TwiMaster& twiMaster);

        void Init();
        void Print(char *str);
        void Received(char* str, int length);

      private:
          Pinetime::System::SystemTask& systemTask;
          Pinetime::Controllers::NimbleController& nimbleController;
          Pinetime::Controllers::FS& fs;
          Pinetime::Components::LittleVgl& lvgl;
          Pinetime::Controllers::MotorController& motorController;
          Pinetime::Drivers::Cst816S& touchPanel;
          Pinetime::Drivers::SpiNorFlash& spiNorFlash;
          Pinetime::Drivers::TwiMaster& twiMaster;

          static constexpr int bufferSize = 256;
          char rxBuffer[bufferSize];
          uint16_t rxPos;

      };
  }
}
