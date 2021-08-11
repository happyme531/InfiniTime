
#include "Console.h"
#include "systemtask/SystemTask.h"
#include "components/ble/NimbleController.h"
#include "components/ble/BleNus.h"

using namespace Pinetime::Components;

Console::Console(Pinetime::System::SystemTask& systemTask,
                Pinetime::Controllers::NimbleController& nimbleController,
                Pinetime::Controllers::FS& fs,
                Pinetime::Components::LittleVgl& lvgl,
                Pinetime::Controllers::MotorController& motorController,
                Pinetime::Drivers::Cst816S& touchPanel,
                Pinetime::Drivers::SpiNorFlash& spiNorFlash,
                Pinetime::Drivers::TwiMaster& twiMaster,
                Pinetime::Controllers::MotionController& motionController):
                systemTask {systemTask},
                nimbleController{nimbleController},
                fs{fs},
                lvgl{lvgl},
                motorController{motorController},
                touchPanel{touchPanel},
                spiNorFlash{spiNorFlash},
                twiMaster{twiMaster},
                motionController{motionController}
{
}

void Console::Init()
{
    auto rxCallback = [this](char *str, int length) {
        this->Received(str, length);
    };

    nimbleController.bleNus().ConsoleRegister(rxCallback);
}

void Console::Print(char *str)
{
    nimbleController.bleNus().Print(str);
}

void Console::Process()
{
    static uint32_t accCount = 0;

    // Simple stupid comparison, later would be nice to add commands lookup table with argument parsing
    if(hasCommandFlag)
    {
        hasCommandFlag = false;

        // This AT > OK needs to be there, because https://terminal.hardwario.com/ waits for the answer
        // When we use or create better webpage terminal, this can go out
        if(strncmp(rxBuffer, "AT", 2) == 0)
        {
            nimbleController.bleNus().Print((char*)"OK\r\n");
        }
        else if(strncmp(rxBuffer, "LVGL", 4) == 0)
        {
            // TODO: list of objects, changing position, size & color would be great
            char lvbuf[128];
            lv_mem_monitor_t mon;
            lv_mem_monitor(&mon);
            snprintf(lvbuf, sizeof(lvbuf), "used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)(mon.total_size - mon.free_size),
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);
            nimbleController.bleNus().Print(lvbuf);

            // List active screen objects
            lv_obj_t* actStrc = lv_scr_act();
            uint16_t childCount = lv_obj_count_children(actStrc);
            snprintf(lvbuf, sizeof(lvbuf), "children: %d\n", childCount);
            nimbleController.bleNus().Print(lvbuf);

            lv_obj_t * child;
            uint16_t i = 0;
            child = lv_obj_get_child(actStrc, NULL);
            while(child) {
                snprintf(lvbuf, sizeof(lvbuf), "#%d, x: %d, y: %d, w: %d, h: %d\n", i++, lv_obj_get_x(child), lv_obj_get_y(child), lv_obj_get_width(child), lv_obj_get_height(child));
                nimbleController.bleNus().Print(lvbuf);
                vTaskDelay(50); // Add small delay for each item, so the print buffer has time to be send over BLE
                
                child = lv_obj_get_child(actStrc, child);
            }
        }
        else if(strncmp(rxBuffer, "VIBRATE", 7) == 0)
        {
            motorController.SetDuration(100);
        }
        else if(strncmp(rxBuffer, "FS", 2) == 0)
        {
            // TODO: add directory listings etc.
        }
        else if(strncmp(rxBuffer, "WKUP", 4) == 0)
        {
            systemTask.PushMessage(Pinetime::System::Messages::GoToRunning);
        }
        else if(strncmp(rxBuffer, "SLEEP", 5) == 0)
        {
            systemTask.PushMessage(Pinetime::System::Messages::GoToSleep);
        }
        else if(strncmp(rxBuffer, "SPINOR", 6) == 0)
        {
            // TODO: print RAW data from FLASH
        }
        else if(strncmp(rxBuffer, "ACC", 3) == 0)
        {
            // Print 50 accelerometer measurements
            accCount = 50;
        }
    }

    // Debug print accelerometer values
    if(accCount)
    {
        accCount--;
        char accBuf[32];

        snprintf(accBuf, sizeof(accBuf), "%d, %d, %d\n", motionController.X(), motionController.Y(), motionController.Z());
        nimbleController.bleNus().Print(accBuf);
    }
}

void Console::Received(char* str, int length)
{
    //char b[128];

    // Wrap if input is too long without CR/LN
    if(rxPos == bufferSize - 1)
    {
        rxPos = 0;
    }

    for(int i = 0; i < length; i++)
    {
        rxBuffer[rxPos++] = str[i];
        rxBuffer[rxPos] = '\0'; // terminate for debug print 

        if(str[i] == 13 || str[i] == 10)
        {
            rxPos = 0;
            hasCommandFlag = true;
            break;
        }
    }

    //sprintf(b, "rx: %s, len: %d, buffer: %s\r\n", str, length, rxBuffer);
    //nimbleController.bleNus().Print(b);
}