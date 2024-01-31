#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <PZEM004Tv30.h>

const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEqjCCA5KgAwIBAgIQAnmsRYvBskWr+YBTzSybsTANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0xNzExMjcxMjQ2MTBaFw0yNzExMjcxMjQ2MTBaMG4xCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xLTArBgNVBAMTJEVuY3J5cHRpb24gRXZlcnl3aGVyZSBEViBUTFMgQ0EgLSBH\n"
    "MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALPeP6wkab41dyQh6mKc\n"
    "oHqt3jRIxW5MDvf9QyiOR7VfFwK656es0UFiIb74N9pRntzF1UgYzDGu3ppZVMdo\n"
    "lbxhm6dWS9OK/lFehKNT0OYI9aqk6F+U7cA6jxSC+iDBPXwdF4rs3KRyp3aQn6pj\n"
    "pp1yr7IB6Y4zv72Ee/PlZ/6rK6InC6WpK0nPVOYR7n9iDuPe1E4IxUMBH/T33+3h\n"
    "yuH3dvfgiWUOUkjdpMbyxX+XNle5uEIiyBsi4IvbcTCh8ruifCIi5mDXkZrnMT8n\n"
    "wfYCV6v6kDdXkbgGRLKsR4pucbJtbKqIkUGxuZI2t7pfewKRc5nWecvDBZf3+p1M\n"
    "pA8CAwEAAaOCAU8wggFLMB0GA1UdDgQWBBRVdE+yck/1YLpQ0dfmUVyaAYca1zAf\n"
    "BgNVHSMEGDAWgBQD3lA1VtFMu2bwo+IbG8OXsj3RVTAOBgNVHQ8BAf8EBAMCAYYw\n"
    "HQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8C\n"
    "AQAwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdp\n"
    "Y2VydC5jb20wQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQu\n"
    "Y29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDBMBgNVHSAERTBDMDcGCWCGSAGG\n"
    "/WwBAjAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BT\n"
    "MAgGBmeBDAECATANBgkqhkiG9w0BAQsFAAOCAQEAK3Gp6/aGq7aBZsxf/oQ+TD/B\n"
    "SwW3AU4ETK+GQf2kFzYZkby5SFrHdPomunx2HBzViUchGoofGgg7gHW0W3MlQAXW\n"
    "M0r5LUvStcr82QDWYNPaUy4taCQmyaJ+VB+6wxHstSigOlSNF2a6vg4rgexixeiV\n"
    "4YSB03Yqp2t3TeZHM9ESfkus74nQyW7pRGezj+TC44xCagCQQOzzNmzEAP2SnCrJ\n"
    "sNE2DpRVMnL8J6xBRdjmOsC3N6cQuKuRXbzByVBjCqAA8t1L0I+9wXJerLPyErjy\n"
    "rMKWaBFLmfK/AHNF4ZihwPGOc7w6UHczBZXH5RFzJNnww+WnKuTPI0HfnVH8lg==\n"
    "-----END CERTIFICATE-----\n";

#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
#define NUM_PZEMS 5

const char *ssid = "PowerHub Canteen 01";
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
    {"main/canteen000001"},
    {"main/canteen000002"},
    {"main/canteen000003"},
    {"main/canteen000004"},
    {"main/canteen000005"},
};

unsigned long pzemReadInterval = 2000;    // 2000ms สำหรับ PZEM004T
unsigned long mqttPublishInterval = 4000; // 4000ms สำหรับ MQTT publish
unsigned long pub_time_now = 0;
unsigned long read_pzem_values_time_now = 0;
unsigned long print_pzem_values_time_now = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);
PZEM004Tv30 pzems[NUM_PZEMS];
WiFiManager wm;

void setupHardware();
void setupNetwork();
void connectToMQTTBroker();
void checkWifiConnection();
void readPZEMValues();
void checkMQTTBrokerConnection();

void setupHardware()
{
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < NUM_PZEMS; i++)
  {
    pzems[i] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x1 + i);
  }
}

void setupNetwork()
{
  espClient.setCACert(ca_cert);
  wm.setConnectTimeout(20);
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
    digitalWrite(LED_BUILTIN , HIGH);
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

void readPZEMValues()
{
  unsigned long currentMillis = millis();
  for (const auto &device : devices)
  {
    if (currentMillis - read_pzem_values_time_now >= pzemReadInterval)
    {
      read_pzem_values_time_now = currentMillis;
      for (int i = 0; i < NUM_PZEMS; i++)
      {
        voltage[i] = pzems[i].voltage();

        current[i] = pzems[i].current();

        power[i] = pzems[i].power();

        energy[i] = pzems[i].energy();

        frequency[i] = pzems[i].frequency();

        pf[i] = pzems[i].pf();

        Serial.print("Device : " + String(devices[i].topic) + "\t" + String(voltage[i]) + "V\t" + String(current[i]) + "A\t" + String(power[i]) + "W\t" + String(energy[i]) + "kWh\t" + String(frequency[i]) + "Hz\n");
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
      Serial.println("Lost connection to MQTT broker. Reconnecting...");
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

void setup()
{
  Serial.begin(115200);
  setupHardware();
  setupNetwork();
  connectToMQTTBroker();
}

void loop()
{
  client.loop();
  checkWifiConnection();
  checkMQTTBrokerConnection();
  readPZEMValues();
  publishDataToMQTTBroker();
}