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


#define NUMBER "1"
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

void Communication::connect(){
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i){
        if (WiFi.SSID(i) == WIFI_SSID1){
            WiFi.begin(WIFI_SSID1, WIFI_PASS1); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID1);
            break;
        }
        if (WiFi.SSID(i) == WIFI_SSID2){
            WiFi.begin(WIFI_SSID2, WIFI_PASS2); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID2);
            break;
        }
        if (WiFi.SSID(i) == WIFI_SSID3){
            WiFi.begin(WIFI_SSID3, WIFI_PASS3); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID3);
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
        if (connection_times >= WIFI_CONNECT_RETRY)
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

int Communication::feed_data(Mode* m, String s){
    int start=0, len=0;
    uint32_t checksum = 0;
    for (int i=0; i<s.length(); i++){
        if (s[i] >= '0' && s[i] <= '9') continue;
        if (s[i] == ';'){
            // Perform checksum and break
            String meta = web_data.substring(start, i);
            if (checksum & 0xff == meta.toInt())
                return len-2;
            else    // Checksum failed
                return 0;
        }

        // If there is multi times non-number charactor appearing, it
        // will skip them till next number comes
        if (start != i){
            // Length of string must larger than 0
            String meta = web_data.substring(start, i);

            // If it failed to convert, it will return 0.
            if (len == 0)       // Mode Type
                (*m).mode = meta.toInt();
            else if (len == 1)  // Start time
                (*m).start_time = meta.toInt();
            else{               // Other parameter
                (*m).param[len-2] = meta.toInt();
                checksum += (*m).param[len-2];
            }
            len++;
        }
        start = i+1;
    }
    // End without checksum, unusual request
    return 0;
}

bool Communication::receive(Mode* m){
    if (WiFi.status() == WL_CONNECTED){
        /* Request data from server */
        String url = WIFI_REQUEST_URL + String(number) + "-light.txt";
        http.begin(url);
        httpCode = http.GET();
        web_data = http.getString();

        if (feed_data(m, web_data) == 0){
            // Message error, Report it
        }
        http.end();

        /* Report connection time */ 
        url = WIFI_RESPOND_URL + String(number);
        http.begin(url);
        httpCode = http.GET();
        http.end();
    }
    else{
        #ifdef DEBUGGER
        Serial.println("Connection Failed! Rebooting...");
        #endif
        delay(1000);
        ESP.restart();
    }
    return true;
}