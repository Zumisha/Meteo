#pragma once

const int MHZ19_RX_pin = 13;			// D7 NodeMCU pin
const int MHZ19_TX_pin = 15;			// D8 NodeMCU pin
const uint8_t BME_Address = 0x76;		// Usually BME address is 0x76 or 0x77
unsigned long SerialSpeed = 115200;		// Set in accordance with the default speed used by the controller 
										// (to be able to view information about low-level errors)
const int ServerPort = 80;
unsigned long  RefreshRate = 1000;		// Refresh rate can't be less than 1000ms
const int TimeOffset = 3 * 3600;		// +3.00 (Moscow time)
const float PascalsToMmHg = 0.00750062; // Pascals to millimeters of mercury conversion factor
const size_t QueueSize = 50;			// Set based on the size of RAM. If you get a hardware error, decrease this value.

const char* PROGMEM ssid = "Zunet2";
const char* PROGMEM password = "vjcrdfboss";

const char* PROGMEM MainPageAddress = "/index.html";

class Message
{
public:
	static constexpr char* PROGMEM SpiffsErr = "Error during SPIFFS initialization!";
	static constexpr char* PROGMEM SpiffsGood = "SPIFFS was initialized!";
	static constexpr char* PROGMEM BmeErr = "Could not find a valid BME280 sensor, check the correctness of the address and the correct connection of wires!";
	static constexpr char* PROGMEM BmeGood = "BME280 sensor was initialized!";
	static constexpr char* PROGMEM MzhErr = "Could not find a valid MH-Z19 sensor, check the correctness of the address and the correct connection of wires!";
	static constexpr char* PROGMEM MzhGood = "MH-Z19 sensor was initialized!";
	static constexpr char* PROGMEM TimeErr = "Failed to get time from NTP server!";
	static constexpr char* PROGMEM TimeGood = "Time successfully synchronized!";
};

class ContentType 
{
public:
	static constexpr char* PROGMEM Text = "text/plain";
	static constexpr char* PROGMEM Page = "text/html";
	static constexpr char* PROGMEM Style = "text/css";
	static constexpr char* PROGMEM Script = "application/javascript";
	static constexpr char* PROGMEM Json = "text/json";
	static constexpr char* PROGMEM Binary = "application/octet-stream";
};

constexpr char hexMap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
						   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

struct readings
{
	int date;
	float temperature;
	float humidity;
	float pressure;
	float CO2;
};