#include "communication.h"

extern String data;
extern String url;
extern HTTPClient http;
extern int httpCode;
extern String web_data;
extern float BRIGHTNESS;
extern int SPEED;
extern float COLOR[3];
extern int light_stat;

extern TaskHandle_t LED_UPDATE;
extern TaskHandle_t WIFI_HANDLE;

#define STASSID1 "Benson"
#define STAPSK1 "0932209548"
#define STASSID2 "2.4G"
#define STAPSK2 "12345qwert"
#define STASSID3 "NCKUES[AUTO]"
#define STAPSK3 "nckues_auto"
#define NUMBER "1"
const char *ssid1 = STASSID1;
const char *password1 = STAPSK1;
const char *ssid2 = STASSID2;
const char *password2 = STAPSK2;
const char *ssid3 = STASSID3;
const char *password3 = STAPSK3;
const char *number = NUMBER;
const int httpsPort = 443;

void OTA()
{
    ArduinoOTA.onStart([]() {
        vTaskDelete(WIFI_HANDLE);
        vTaskDelete(LED_UPDATE);
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            Serial.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            Serial.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            Serial.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}

void wifi_connect()
{
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i)
    {
        if (WiFi.SSID(i) == ssid1)
        {
            WiFi.begin(ssid1, password1); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(ssid1);
            break;
        }
        if (WiFi.SSID(i) == ssid2)
        {
            WiFi.begin(ssid2, password2); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(ssid2);
            break;
        }
        if (WiFi.SSID(i) == ssid3)
        {
            WiFi.begin(ssid3, password3); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(ssid3);
            break;
        }
    }

    Serial.print("Waiting for connection");
    // Wait for connection
    int connection_times = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        connection_times++;
        if (connection_times > 20)
        {
            Serial.println();
            Serial.println("Connection Failed! Rebooting...");
            delay(1000);
            ESP.restart();
        }
    }

    Serial.println("");
    /*Serial.print("Connected to ");
    Serial.println(ssid);*/
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void get_data()
{
    data = receivelightmessage();
    /*if (data == "")
        ESP.restart();*/
    /*Serial.print("Message Received : ");
    //Serial.println(data);
    Serial.print(data);
    Serial.print("  ");
    Serial.print(BRIGHTNESS);
    Serial.print("  ");
    Serial.print(SPEED);
    Serial.print("  ");
    Serial.print(COLOR[0]);
    Serial.print("  ");
    Serial.print(COLOR[1]);
    Serial.print("  ");
    Serial.println(COLOR[2]);*/
    light_stat = data.toInt();
}

String receivelightmessage()
{
    String message = "";
    String str_brightness = "";
    String str_speed = "";
    String str_R = "";
    String str_G = "";
    String str_B = "";
    if (WiFi.status() == WL_CONNECTED)
    {

        //catch light
        //HTTPClient http;
        url = "http://122.117.214.235/esp8266/state/" + String(number) + "-light.txt";
        //client.connect(url, httpsPort);  //for https
        //http.begin(client, url);  //for https
        http.begin(url);
        //delay(2);
        //http.addHeader("Content-Type", "text/plain");
        //delay(1);
        httpCode = http.GET();
        //delay(2);
        web_data = http.getString();
        //delay(2);
        //Serial.println(data);
        //lastmessage=getlastline(data);
        message = web_data.substring(0, 3);
        str_brightness = web_data.substring(4, 7);
        str_speed = web_data.substring(8, 11);
        str_R = web_data.substring(12, 15);
        str_G = web_data.substring(16, 19);
        str_B = web_data.substring(20, 23);
        BRIGHTNESS = str_brightness.toInt();
        SPEED = str_speed.toInt();
        /*float speed_temp = SPEED;
        speed_temp = speed_temp / 10 + 90;*/
        SPEED = 101 - SPEED;
        COLOR[0] = str_R.toInt();
        COLOR[1] = str_G.toInt();
        COLOR[2] = str_B.toInt();
        /*(COLOR[0] >= 250) ? COLOR[0] = 250 : 1;
        (COLOR[1] >= 250) ? COLOR[1] = 250 : 1;
        (COLOR[2] >= 250) ? COLOR[2] = 250 : 1;*/
        //message.replace(" ", "");
        http.end();
        //delay(2);

        //report connection time
        url = "http://122.117.214.235/esp8266/read.php?data=" + String(number);
        //client.connect(url, httpsPort);  //for https
        //http.begin(client, url);  //for https
        http.begin(url);
        //delay(2);
        //http.addHeader("Content-Type","text/plain");
        httpCode = http.GET();
        //delay(2);
        http.end();
        //delay(2);
    }
    else
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(1000);
        ESP.restart();
    }
    return message;
}