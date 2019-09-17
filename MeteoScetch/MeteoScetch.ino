#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BME280.h>
#include <MHZ19.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <FS.h>
#include "Deque.h"
#include "Debug.html.h"
#include "Definitions.h"

Adafruit_BME280 bme;
MHZ19 mhz19(MHZ19_RX_pin, MHZ19_TX_pin);
QueueArray<readings> readingsQueue(QueueSize);
ESP8266WebServer server(ServerPort);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void SpiffsInitialize()
{
	while (!SPIFFS.begin())
	{		
		Serial.println(Message::SpiffsErr);
		server.send(200, ContentType::Text, Message::SpiffsErr);
		delay(1000);
	}
	Serial.println(Message::SpiffsGood);
	server.send(200, ContentType::Text, Message::SpiffsGood);
	Serial.println();
}

void BmeInitialize()
{
	while (!bme.begin(BME_Address)) {
		Serial.println(Message::BmeErr);
		server.send(200, ContentType::Text, Message::BmeErr);
		delay(1000);
	}
	Serial.println(Message::BmeGood);
	Serial.println();
	server.send(200, ContentType::Text, Message::BmeGood);
}

void Mzh19Initialize()
{
	mhz19.setAutoCalibration(false);
	while (mhz19.getStatus() == -1) {
		Serial.println(Message::MzhErr);
		delay(1000);
	}
	Serial.println(Message::MzhGood);
	Serial.println();
	/*while ( mhz19.isWarming() ) {
	  Serial.println("MH-Z19 now warming up...");
	  delay(1000);
	}
	Serial.println("MH-Z19 was warmed up!");
	Serial.println();//*/
}

void ConnectToWiFi()
{
	Serial.print(F("Connecting to ")); Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.println(F("Try connect to WiFi..."));
		delay(1000);
	}
	Serial.println(F("WiFi connected!"));
	Serial.print(F("Got IP: "));  Serial.println(WiFi.localIP());
	Serial.println();
	/*//Initialize AP Mode
	WiFi.softAP(ssid);  //Password not used
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("Web Server IP:");
	Serial.println(myIP);//*/
}

void TimeSetup()
{
	while (!timeClient.update())
	{
		Serial.println(Message::TimeErr);
		Serial.println(timeClient.getFormattedDate());
		delay(1000);
	}
	timeClient.setTimeOffset(TimeOffset);
	Serial.println(Message::TimeGood);
	Serial.println();
}

bool loadFromSpiffs(String path) {
	String dataType;
	if (path.endsWith("/")) path = MainPageAddress;
	if (!SPIFFS.exists(path.c_str()))
	{
		return false;
	}
	if (path.endsWith(".html")) dataType = ContentType::Page;
	else if (path.endsWith(".css")) dataType = ContentType::Style;
	else if (path.endsWith(".js")) dataType = ContentType::Script;
	File dataFile = SPIFFS.open(path.c_str(), "r");
	if (server.streamFile(dataFile, dataType) != dataFile.size()) {
		dataFile.close();
		return false;
	}
	dataFile.close();
	return true;
}

String RequestInfo()
{
	String message = F("URI: ");
	message += server.uri();
	message += F("\nMethod: ");
	message += ((server.method() == HTTP_GET) ? F("GET") : F("POST"));
	message += F("\nArguments: ");
	message += server.args();
	message += F("\n");
	for (uint8_t i = 0; i < server.args(); i++) {
		message += F(" NAME:");
		message += server.argName(i);
		message += F("\n VALUE:");
		message += server.arg(i);
		message += F("\n");
	}
	message += F("\n");
	return message;
}

String hexStr(char *data, std::size_t len)
{
	String s;
	for (std::size_t i = 0; i < len; ++i) {
		s += hexMap[(static_cast<int>(data[i]) & 0xF0) >> 4];
		s += hexMap[static_cast<int>(data[i]) & 0x0F];
	}
	return s;
}

String hexStr(byte *data, std::size_t len)
{
	String s;
	for (std::size_t i = 0; i < len; ++i) {
		s += hexMap[(static_cast<int>(data[i]) & 0xF0) >> 4];
		s += hexMap[static_cast<int>(data[i]) & 0x0F];
	}
	return s;
}

String hexStr(String data)
{
	String s;
	for (std::size_t i = 0; i < data.length(); ++i) {
		s += hexMap[(static_cast<int>(data[i]) & 0xF0) >> 4];
		s += hexMap[static_cast<int>(data[i]) & 0x0F];
	}
	return s;
}

void handleNotFound() {
	if (server.method() == HTTP_OPTIONS)
	{
		server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
		server.sendHeader(F("Access-Control-Max-Age"), F("10000"));
		server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
		server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
		server.send(204);
		return;
	}
	if (loadFromSpiffs(server.uri())) return;
	String message = F("File Not Detected\n\n");
	message += RequestInfo();
	//server.send(404, ContentType::Text, message);
	Serial.println(message);
}

void PostDebug()
{
	String message = F("PostDebug\n");
	message += RequestInfo();
	Serial.println(message);
	std::size_t len = static_cast<std::size_t>(sizeof(readings)) * static_cast<std::size_t>(readingsQueue.Length());
	byte* data = new byte[len];
	for (std::size_t i = 0; i < readingsQueue.Length(); ++i)
	{
		memcpy(data + static_cast<std::size_t>(i) * sizeof(readings), &readingsQueue[i], sizeof(readings));
	}
	String request = "";
	for (std::size_t i = 0; i < len; ++i)
	{
		request += static_cast<char>(data[i]);
	}
	server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
	server.sendHeader(F("Access-Control-Max-Age"), F("10000"));
	server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
	server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
	//server.send(200, ContentType::Text, request);
	//Serial.println(hexStr(data, len));
	/*WiFiClient client = server.client();
	client.write(data, len);//*/
	//server.send(200, ContentType::Binary, data, len);
	Serial.println(ESP.getFreeHeap()); // if not ESP use MemoryFree library
	Serial.println();
	delete[]data;
}

void handleDebug() 
{
	String message = F("handleDebug\n");
	message += RequestInfo();
	Serial.println(message);
	server.send(200, ContentType::Page, DebugPage);
}

void StartServer()
{
	server.on("/debug", handleDebug);
	server.on("/getDebugData", HTTP_GET, PostDebug);
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println(F("HTTP server started"));
	Serial.println();
}

void setup() {
	Serial.begin(SerialSpeed);
	delay(100);
	SpiffsInitialize();
	BmeInitialize();
	Mzh19Initialize();
	ConnectToWiFi();
	TimeSetup();
	StartServer();
}

void loop() {
	readings last;
	last.date = timeClient.getEpochTime();
	last.temperature = bme.readTemperature();
	last.humidity = bme.readHumidity();
	last.pressure = bme.readPressure() * PascalsToMmHg;
	last.CO2 = mhz19.getPPM(MHZ19_POTOCOL::UART);
	readingsQueue.PushBack(last);
	delay(RefreshRate);
	server.handleClient();
}
