/****************************************************************************************************************************
  UDPSendReceive.ino - Simple Arduino web server sample for ESP8266/ESP32 AT-command shield
  
  For Ethernet shields using WT32_ETH01 (ESP32 + LAN8720)

  WebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run WebServer

  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/WebServer_WT32_ETH01
  Licensed under MIT license
 *****************************************************************************************************************************/

#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_       3

#include <WebServer_WT32_ETH01.h>

// Select the IP address according to your local network
IPAddress myIP(192, 168, 2, 232);
IPAddress myGW(192, 168, 2, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

bool eth_connected = false;

unsigned int localPort = 1883;    //10002;  // local port to listen on

char packetBuffer[255];          // buffer to hold incoming packet
byte ReplyBuffer[] = "ACK";      // a string to send back

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("\nETH Started");
      //set eth hostname here
      ETH.setHostname("WT32-ETH01");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;

    case SYSTEM_EVENT_ETH_GOT_IP:
      if (!eth_connected)
      {
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());

        if (ETH.fullDuplex())
        {
          Serial.print(", FULL_DUPLEX");
        }

        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
      }

      break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;

    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("\nETH Stopped");
      eth_connected = false;
      break;

    default:
      break;
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  // Using this if Serial debugging is not necessary or not using Serial port
  //while (!Serial && (millis() < 3000));

  Serial.print("\nStarting UDPSendReceive on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO, 
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);

  WiFi.onEvent(WiFiEvent);

  while (!eth_connected)
    delay(100);

  Serial.println(F("\nStarting connection to server..."));
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

  Serial.print(F("Listening on port "));
  Serial.println(localPort);
}

void loop()
{
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();

  if (packetSize)
  {
    Serial.print(F("Received packet of size "));
    Serial.println(packetSize);
    Serial.print(F("From "));
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(F(", port "));
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    
    if (len > 0)
    {
      packetBuffer[len] = 0;
    }

    Serial.println(F("Contents:"));
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer, sizeof(ReplyBuffer));
    Udp.endPacket();
  }
}
