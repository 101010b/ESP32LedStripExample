/* LED Hexagon (from LED Football Design)
 *  20 x Hexagon a 13 LEDs
 * ----
 *  20 x PCBs
 *                 13 LEDs
 *                
 *  LED Type APA102C
 *  
 *  (C) 2020 Thomas Buck
 *      Thomas.Buck@101010b.de
 */
#include <stdint.h>
#include <string.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp32-hal-timer.h>

#include "config.h"
#include "MultiServiceIO.h"
#include "main.h"
#include "ledctrl.h"
#include "effect.h"

/************************** Global State *************************************/
ledctrl *led;

unsigned long lastexec = 0;

MultiServiceIO *mio = NULL;
UI32Var_t state;
UI32Var_t global;
UI32Var_t current;

void effectLoop()
{
  if (millis() - lastexec >= 20)
  {
    lastexec = millis();
    if (state.val)
      led->tick();
    else
      led->allOff();
    led->progLeds();
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  Serial.println("Initializing Device...");

  // Try to initialize SPIFFS 
  Serial.println("SPIFFS initialization");
  bool spiffsok = false;
  try {
    if (SPIFFS.begin())
      spiffsok = true;
  }
  catch (void *) {
  }
  if (!spiffsok) {
    Serial.println("An Error occured initializing the SPIFFS System");
    Serial.print("Waiting timout before formatting SPIFFS File.");
    for (int i = 0; i < 30; i++)
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Trying to format the SPIFFS File System");
    Serial.println("This may take some time - please be patient");
    SPIFFS.format();
    // Retry SPIFFS again
    if (!SPIFFS.begin())
    {
      Serial.println("An Error occured initializing the SPIFFS System --> Stopping");
      while (1)
      {
        Serial.print(".");
        delay(1000);
      }
    }
  }
  
  // Initialize MultiServiceIO Object
  Serial.println("MultiServiceIO Object");
  mio = new MultiServiceIO();
  
  Serial.println("LED Control Object");
  led = new ledctrl();
  X32Set(state, 0, 0, 1);
  X32Set(global, 1, 1, 31);
  X32Set(current, 0, 0, led->list.size()-1);

  // Create Variables and corresponding callbacks
  mio->addNewConfigValue("state", &state);
  mio->addNewConfigValue("global", &global);
  mio->addNewConfigValue("effect", &current);

  for (std::list<parametric *>::iterator it = led->globals.begin(); it != led->globals.end(); ++it)
  {
    param_t *p = (*it)->params;
    while (p)
    {
      char pl[33];
      if ((*it)->formatparam(p, pl, 32))
      {
        char nm[64];
        sprintf(nm, "%s_%s", (*it)->name, p->name);
        Serial.print("Global Parameter = "); Serial.println(nm);
        switch (p->type) {
          case PARAMTYPE::INT:
            mio->addNewConfigValue(nm, &p->d.i.val);
            break;
          case PARAMTYPE::COL:
            mio->addNewConfigValue(nm, &p->d.c.val, true);
            break;
          case PARAMTYPE::BOOL:
            mio->addNewConfigValue(nm, &p->d.b.val);
            break;
          case PARAMTYPE::STRING:
            mio->addNewConfigValue(nm, &p->d.s.val);
            break;
        }
      }
      p = p->next;
    }
  }
  for (std::list<effect *>::iterator it = led->list.begin(); it != led->list.end(); ++it)
  {
    param_t *p = (*it)->params;
    while (p)
    {
      char pl[33];
      if ((*it)->formatparam(p, pl, 32))
      {
        char nm[64];
        sprintf(nm, "%s_%s", (*it)->name, p->name);
        Serial.print("Effect Parameter = ");
        Serial.println(nm);
        switch (p->type)
        {
        case PARAMTYPE::INT:
          mio->addNewConfigValue(nm, &p->d.i.val);
          break;
        case PARAMTYPE::COL:
          mio->addNewConfigValue(nm, &p->d.c.val, true);
          break;
        case PARAMTYPE::BOOL:
          mio->addNewConfigValue(nm, &p->d.b.val);
          break;
        case PARAMTYPE::STRING:
          mio->addNewConfigValue(nm, &p->d.s.val);
          break;
        }
      }
      p = p->next;
    }
  }

  Serial.println("Setting Up MIO");
  mio->setup();
  Serial.println("Setup finished --> Loop Mode now");
}

void loop()
{
  mio->loop();
  led->setGlobal(global.val);
  led->selectedEffect = current.val;
  effectLoop();
  // delay(100);
  /*if (mio->BLEConnected)
    Serial.print("B");
  else
    Serial.print(".");*/
}


  // EOF
