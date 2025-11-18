/*
 *******************************************************************************
 * USB-MIDI dump utility
 * Copyright (C) 2013-2021 Yuuichi Akagawa
 *
 * for use with USB Host Shield 2.0 from Circuitsathome.com
 * https://github.com/felis/USB_Host_Shield_2.0
 *
 * This is sample program. Do not expect perfect behavior.
 *******************************************************************************
 */

#include <usbh_midi.h>
#include <usbhub.h>

USB Usb;
USBHub Hub(&Usb);
USBH_MIDI Midi(&Usb);
USBH_MIDI Midi1(&Usb);

void MIDI_poll();

void onInit() {
  char buf[40];
  uint16_t vid = Midi.idVendor();
  uint16_t pid = Midi.idProduct();
  uint16_t add = Midi.GetAddress();
  sprintf(buf, "VID:%04X, PID:%04X, ADDR:%02X", vid, pid, add);
  Serial.println(buf);
  Serial.println("Midi_____________________________");
}

void onInit1() {
  char buf[40];
  uint16_t vid = Midi1.idVendor();
  uint16_t pid = Midi1.idProduct();
  uint16_t add = Midi1.GetAddress();
  sprintf(buf, "VID:%04X, PID:%04X, ADDR:%02X", vid, pid, add);
  Serial.println(buf);
  Serial.println("Midi1_____________________________");
}

void setup() {
  Serial.begin(115200);
  //pinMode(LED_BUILTIN, OUTPUT);
  if (Usb.Init() == -1) {
    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(1000)
    //digitalWrite(LED_BUILTIN, LOW);
    Serial.println("USB init() Error!");
    while (1)
      ;  //halt
  }      //if (Usb.Init() == -1...
  delay(200);

  // Register onInit() function
  Midi.attachOnInit(onInit);
  Midi1.attachOnInit(onInit1);
  Serial.println("Going to loop()");
}

void loop() {
  Usb.Task();
  if (Midi) {
    MIDI_poll();
  }
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll() {
  char buf[16];
  uint8_t bufMidi[MIDI_EVENT_PACKET_SIZE];
  uint16_t rcvd;
  byte message[3];
  //check what midi instance is in the sender device
  if (Midi.GetAddress() == 0x0A) {
    if (Midi.RecvData(&rcvd, bufMidi) == 0) {
      uint32_t time = (uint32_t)millis();
      sprintf(buf, "%04X%04X:%3d:", (uint16_t)(time >> 16), (uint16_t)(time & 0xFFFF), rcvd);  // Split variable to prevent warnings on the ESP8266 platform
      Serial.print(buf);
      Serial.println("");
      for (int i = 0; i < 8; i++) {
        //for (int i = 0; i < 8; i++) {
        /*sprintf(buf, " %02X", bufMidi[i]);
      Serial.print(buf);*/
        Serial.print(bufMidi[i], HEX);
        Serial.print(" ");
        if (((bufMidi[i] >> 4) == 0xC) || ((bufMidi[i] >> 4) == 0xB)) {
          message[0] = bufMidi[i];
          message[1] = bufMidi[i + 1];
          if ((bufMidi[i] >> 4) == 0xB) {
            message[2] = bufMidi[i + 2];
          }

          if (Midi1) {
            Midi1.SendData(message);
          }
        }
      }


      if (!Midi1) {
        Serial.println("No Midi1 Connected!");
      } else {
        Serial.println("Sender is Midi");
      }
    }
  } else if (Midi1.GetAddress() == 0x0A) {
    if (Midi1.RecvData(&rcvd, bufMidi) == 0) {
      uint32_t time = (uint32_t)millis();
      sprintf(buf, "%04X%04X:%3d:", (uint16_t)(time >> 16), (uint16_t)(time & 0xFFFF), rcvd);  // Split variable to prevent warnings on the ESP8266 platform
      Serial.print(buf);

      for (int i = 0; i < 8; i++) {
        //for (int i = 0; i < 8; i++) {
        /*sprintf(buf, " %02X", bufMidi[i]);
      Serial.print(buf);*/
        Serial.print(bufMidi[i], HEX);
        Serial.print(" ");
        if (((bufMidi[i] >> 4) == 0xC) || ((bufMidi[i] >> 4) == 0xB)) {
          message[0] = bufMidi[i];
          message[1] = bufMidi[i + 1];
          if ((bufMidi[i] >> 4) == 0xB) {
            message[2] = bufMidi[i + 2];
          }

          if (Midi) {
            Midi.SendData(message);
          }
        }
      }

      Serial.println("");
      if (!Midi) {
        Serial.println("No Midi Connected!");
      } else {
        Serial.println("Sender is Midi1");
      }
    }
  } else {
    Serial.println("Main Midi no Connected");
  }
}
