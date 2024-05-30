// /**
//  * @file      Arduino_NetworkTest.ino
//  * @author    Lewis He (lewishe@outlook.com)
//  * @license   MIT
//  * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
//  * @date      2023-02-01
//  * @note      This example function is the SIMCom A7670E network test to
//  *            determine whether the module can access the network and obtain some access parameters
//  */

// // Set serial for debug console (to the Serial Monitor, default speed 115200)
// #define SerialMon Serial

// // Set serial for AT commands (to the module)
// #define SerialAT Serial1

// #define TINY_GSM_MODEM_SIM7000  // This line can remain the same for simplicity, but ensure you use correct library for A7670E
// #define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// // See all AT commands, if wanted
// // #define DUMP_AT_COMMANDS

// #include <TinyGsmClient.h>
// #include <SPI.h>
// #include <SD.h>
// #include <Ticker.h>

// #ifdef DUMP_AT_COMMANDS
// #include <StreamDebugger.h>
// StreamDebugger debugger(SerialAT, SerialMon);
// TinyGsm modem(debugger);
// #else
// TinyGsm modem(SerialAT);
// #endif

// #define UART_BAUD           9600
// #define PIN_DTR             25
// #define PIN_TX              26
// #define PIN_RX              27
// #define PWR_PIN             4

// #define SD_MISO             2
// #define SD_MOSI             15
// #define SD_SCLK             14
// #define SD_CS               13
// #define LED_PIN             12

// void modemPowerOn()
// {
//     pinMode(PWR_PIN, OUTPUT);
//     digitalWrite(PWR_PIN, LOW);
//     delay(1000);    // Datasheet Ton minimum = 1S
//     digitalWrite(PWR_PIN, HIGH);
// }

// void modemPowerOff()
// {
//     pinMode(PWR_PIN, OUTPUT);
//     digitalWrite(PWR_PIN, LOW);
//     delay(1500);    // Datasheet Ton minimum = 1.2S
//     digitalWrite(PWR_PIN, HIGH);
// }

// void setup()
// {
//     // Set console baud rate
//     SerialMon.begin(115200);

//     // Set LED OFF
//     pinMode(LED_PIN, OUTPUT);
//     digitalWrite(LED_PIN, HIGH);

//     modemPowerOn();

//     SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
//     if (!SD.begin(SD_CS)) {
//         Serial.println("> It looks like you haven't inserted the SD card..");
//     } else {
//         uint32_t cardSize = SD.cardSize() / (1024 * 1024);
//         String str = "> SDCard Size: " + String(cardSize) + "MB";
//         Serial.println(str);
//     }

//     SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

//     Serial.println("> Check whether Modem is online");
//     // Test modem is online
//     uint32_t timeout = millis();
//     while (!modem.testAT()) {
//         Serial.print(".");
//         if (millis() - timeout > 60000 ) {
//             Serial.println("> It looks like the modem is not responding, trying to restart");
//             modemPowerOff();
//             delay(5000);
//             modemPowerOn();
//             timeout = millis();
//         }
//     }
//     Serial.println("\nModem is online");

//     // Test SIM card status
//     timeout = millis();
//     Serial.print("> Get SIM card status");
//     while (modem.getSimStatus() != SIM_READY) {
//         Serial.print(".");
//         if (millis() - timeout > 60000 ) {
//             Serial.println("It seems that your SIM card has not been detected. Has it been inserted?");
//             Serial.println("If you have inserted the SIM card, please remove the power supply again and try again!");
//             return;
//         }
//     }
//     Serial.println();
//     Serial.println("> SIM card exists");

//     Serial.println("> /**********************************************************/");
//     Serial.println("> Please make sure that the location has 4G/LTE signal");
//     Serial.println("> /**********************************************************/");

//     String res = modem.getIMEI();
//     Serial.print("IMEI:");
//     Serial.println(res);
//     Serial.println();

//     // Set network mode to LTE only
//     modem.setNetworkMode(38); // 38 for LTE only

//     // Configure APN settings
//     modem.sendAT("+CGDCONT=1,\"IP\",\"internet\"");
//     modem.waitResponse();

//     modem.sendAT("+CGAUTH=1,1,\"true\",\"true\"");
//     modem.waitResponse();

//     // Check network signal and registration information
//     Serial.println("> Accessing the network. The access speed may be slow. Please wait patiently");
//     RegStatus status;
//     timeout = millis();
//     do {
//         int16_t sq = modem.getSignalQuality();

//         status = modem.getRegistrationStatus();

//         if (status == REG_DENIED) {
//             Serial.println("> The SIM card you use has been rejected by the network operator. Please check that the card you use is not bound to a device!");
//             return;
//         } else {
//             Serial.print("Signal:");
//             Serial.println(sq);
//         }

//         if (millis() - timeout > 360000 ) {
//             if (sq == 99) {
//                 Serial.println("> It seems that there is no signal. Please check whether the LTE antenna is connected.");
//                 return;
//             }
//             timeout = millis();
//         }

//         delay(800);
//     } while (status != REG_OK_HOME && status != REG_OK_ROAMING);

//     Serial.println("Obtain the APN issued by the network");
//     modem.sendAT("+CGNAPN");
//     if (modem.waitResponse(3000, res) == 1) {
//         res = res.substring(res.indexOf(",") + 1);
//         res.replace("\"", "");
//         res.replace("\r", "");
//         res.replace("\n", "");
//         res.replace("OK", "");
//         Serial.print("The APN issued by the network is:");
//         Serial.println(res);
//     }

//     modem.sendAT("+CNACT=1");
//     modem.waitResponse();

//     modem.sendAT("+CNACT?");
//     if (modem.waitResponse("+CNACT: ") == 1) {
//         modem.stream.read();
//         modem.stream.read();
//         res = modem.stream.readStringUntil('\n');
//         res.replace("\"", "");
//         res.replace("\r", "");
//         res.replace("\n", "");
//         modem.waitResponse();
//         Serial.print("The current network IP address is:");
//         Serial.println(res);
//     }

//     modem.sendAT("+CPSI?");
//     if (modem.waitResponse("+CPSI: ") == 1) {
//         res = modem.stream.readStringUntil('\n');
//         res.replace("\r", "");
//         res.replace("\n", "");
//         modem.waitResponse();
//         Serial.print("The current network parameter is:");
//         Serial.println(res);
//     }

//     Serial.println("/**********************************************************/");
//     Serial.println("After the network test is complete, please enter the  ");
//     Serial.println("AT command in the serial terminal.");
//     Serial.println("/**********************************************************/\n\n");
// }

// void loop()
// {
//     while (SerialAT.available()) {
//         SerialMon.write(SerialAT.read());
//     }
//     while (SerialMon.available()) {
//         SerialAT.write(SerialMon.read());
//     }
// }

#include <HardwareSerial.h>

#define MODEM_PWRKEY         4
#define MODEM_TX             26
#define MODEM_RX             27

HardwareSerial SerialAT(1);

void sendATCommand(const char* cmd);
void readResponse();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  // Configure the modem power key pin
  pinMode(MODEM_PWRKEY, OUTPUT);
  digitalWrite(MODEM_PWRKEY, LOW);

  // Start communication with the SIMCom module
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  // Power on the module
  Serial.println("Powering on the module...");
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(5000);

  // Wait for the module to power up
  delay(10000);

  // Check AT command to check communication
  sendATCommand("AT");
  delay(2000);

  // Check signal quality
  sendATCommand("AT+CSQ");
  delay(2000);

  // Check SIM card status
  sendATCommand("AT+CPIN?");
  delay(2000);

  // Scan for available networks
  sendATCommand("AT+COPS=?");
  delay(10000);  // Scanning can take a while

  // Manually register to network (use the correct operator name from the scan results)
  sendATCommand("AT+COPS=1,0,\"TRUE-H\"");
  delay(5000);

  // Check network registration
  sendATCommand("AT+CREG?");
  delay(2000);

  // Attempt to attach to GPRS
  sendATCommand("AT+CGATT=1");
  delay(5000);

  // Check GPRS attachment
  sendATCommand("AT+CGATT?");
  delay(2000);

  // Set APN (ensure these are correct)
  sendATCommand("AT+CSTT=\"internet\",\"true\",\"true\"");
  delay(2000);

  // Bring up wireless connection
  sendATCommand("AT+CIICR");
  delay(5000);

  // Get local IP address
  sendATCommand("AT+CIFSR");
  delay(2000);

  // Ping a server to check internet connection
  sendATCommand("AT+PING=\"www.google.com\"");
  delay(2000);

  // Read the final response from the module
  readResponse();
}

void loop() {
  // Nothing here
}

void sendATCommand(const char* cmd) {
  Serial.print("Sending command: ");
  Serial.println(cmd);
  SerialAT.println(cmd);

  unsigned long timeout = millis() + 10000; // 10 seconds timeout for some commands
  while (millis() < timeout) {
    if (SerialAT.available()) {
      String response = SerialAT.readString();
      Serial.print("Response: ");
      Serial.println(response);
      if (response.indexOf("OK") != -1) {
        Serial.println("AT command executed successfully.");
      } else if (response.indexOf("ERROR") != -1) {
        Serial.println("AT command returned an error.");
      } else if (response.indexOf(">") != -1) {
        Serial.println("Ready to receive input.");
      }
      return;
    }
  }
  Serial.println("No response or timeout.");
}

void readResponse() {
  unsigned long timeout = millis() + 10000; // 10 seconds timeout for network commands
  while (millis() < timeout) {
    if (SerialAT.available()) {
      String response = SerialAT.readString();
      Serial.print("Response: ");
      Serial.println(response);
      if (response.indexOf("OK") != -1) {
        Serial.println("Command executed successfully.");
      } else if (response.indexOf("ERROR") != -1) {
        Serial.println("Command returned an error.");
      }
      return;
    }
  }
  Serial.println("No response or timeout.");
}