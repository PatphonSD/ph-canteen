#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <PZEM004Tv30.h>

#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
#define NUM_PZEMS 5

const char *ssid = "PowerHub";
const char *password = "powerhubX";
const char *mqtt_broker = "g8f66080.ala.asia-southeast1.emqxsl.com";
const char *mqtt_username = "PowerHubSwitch";
const char *mqtt_password = "test";
const int mqtt_port = 8883;

float voltage[NUM_PZEMS];
float current[NUM_PZEMS];
float power[NUM_PZEMS];
float energy[NUM_PZEMS];
float frequency[NUM_PZEMS];
float pf[NUM_PZEMS];
float overWatt[NUM_PZEMS];

struct Device
{
    const char *topic;
};

Device devices[] = {
    {"main/1111111111111"},
    {"main/1111111111112"},
    {"main/1111111111113"},
    {"main/1111111111114"},
    {"main/1111111111115"},
};

unsigned long pzemReadInterval = 2000;    // 2000ms สำหรับ PZEM004T
unsigned long mqttPublishInterval = 4000; // 4000ms สำหรับ MQTT publish
unsigned long pub_time_now = 0;
unsigned long time_now = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);
PZEM004Tv30 pzems[NUM_PZEMS];
WiFiManager wm;

void setupHardware();
void setupNetwork();
void connectToMQTTBroker();
void checkWifiConnection();
void checkMQTTBrokerConnection();

void setupHardware()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void setupNetwork()
{

    espClient.setInsecure();

    bool res;
    res = wm.autoConnect(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    if (!res)
    {
        Serial.println("Failed to connect");
        ESP.restart();
    }
    else
    {
        Serial.println("Connected to the Wi-Fi network");
    }
}

void connectToMQTTBroker()
{
    client.setServer(mqtt_broker, mqtt_port);
    while (!client.connected())
    {
        String client_id = "powerhub-canteen-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println("Public EMQX MQTT broker connected");
            for (const auto &device : devices)
            {
                if (client.connected())
                {
                    String subscribeStr = device.topic + String("/#");
                    client.subscribe(subscribeStr.c_str());
                    Serial.println("Subscribed to MQTT topic: " + subscribeStr);
                }
            }
        }
        else
        {
            Serial.print("Failed to connect to MQTT broker, state: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void checkWifiConnection()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi connection lost!");
        wm.reboot();
    }
}

void checkMQTTBrokerConnection()
{
    if (!client.connected())
    {
        connectToMQTTBroker();
    }
}

void setup()
{
    Serial.begin(115200);
    setupHardware();
    setupNetwork();
    connectToMQTTBroker();
}

void readPZEMValues()
{
    unsigned long currentMillis = millis();
    for (const auto &device : devices)
    {
        if (currentMillis - time_now >= pzemReadInterval)
        {
            time_now = currentMillis;
            for (int i = 0; i < NUM_PZEMS; i++)
            {
                voltage[i] = pzems[i].voltage();

                current[i] = pzems[i].current();

                power[i] = pzems[i].power();

                energy[i] = pzems[i].energy();

                frequency[i] = pzems[i].frequency();

                pf[i] = pzems[i].pf();
            }
        }
    }
}

void publishDataToMQTTBroker()
{
    unsigned long currentMillis = millis();
    if (currentMillis - pub_time_now >= mqttPublishInterval)
    {
        pub_time_now = millis();
        while (!client.connected())
        {
            /*       Serial.println("Lost connection to MQTT broker. Reconnecting..."); */
            connectToMQTTBroker();
        }
        if (client.connected())
        {
            for (int i = 0; i < NUM_PZEMS; i++)
            {
                String voltageStr = String(voltage[i], 2);
                String topicWithVoltage = devices[i].topic + String("/voltage");
                client.publish(topicWithVoltage.c_str(), voltageStr.c_str());

                String currentStr = String(current[i], 2);
                String topicWithCurrent = devices[i].topic + String("/current");
                client.publish(topicWithCurrent.c_str(), currentStr.c_str());

                String powerStr = String(power[i], 2);
                String topicWithPower = devices[i].topic + String("/power");
                client.publish(topicWithPower.c_str(), powerStr.c_str());

                String energyStr = String(energy[i], 2);
                String topicWithEnergy = devices[i].topic + String("/energy");
                client.publish(topicWithEnergy.c_str(), energyStr.c_str());

                String frequencyStr = String(frequency[i], 2);
                String topicWithFrequency = devices[i].topic + String("/frequency");
                client.publish(topicWithFrequency.c_str(), frequencyStr.c_str());

                String pfStr = String(pf[i], 2);
                String topicWithPf = devices[i].topic + String("/pf");
                client.publish(topicWithPf.c_str(), pfStr.c_str());
            }
        }
    }
}

void loop()
{
    client.loop();
    checkWifiConnection();
    checkMQTTBrokerConnection();
    readPZEMValues();
}