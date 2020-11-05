#include "communication.h"

extern TaskHandle_t LED_UPDATE;
extern TaskHandle_t WIFI_HANDLE;

HTTPClient http;

Communication::Communication(){
    
}

void Communication::init(){
    connect();
    OTA();
}

void Communication::OTA()
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

void Communication::feed_color_param(ValueParam* p, String s){
    int comma = s.indexOf(',');
    uint32_t upper = s.substring(0, comma).toInt();
    p->func = (upper >> 16)    & 0xff;
    p->XH.range = (upper >> 8) & 0xff;
    p->XH.lower = upper        & 0xff;
    
    uint32_t lower = s.substring(comma+1).toInt();
    p->XH.p1 = (lower >> 8) & 0xff;
    p->XH.p2 = lower        & 0xff;
}

int Communication::feed_data(Mode* m, String s){
    int start=0, len=0;
    uint32_t checksum = 0;
    for (int i=0; i<s.length(); i++){
        if ((s[i] >= '0' && s[i] <= '9') || s[i] == ',') continue;
        if (s[i] == ';'){
            // Perform checksum and break
            String meta = s.substring(start, i);
            if (checksum & 0xff == meta.toInt())
                return len-2;
            else    // Checksum failed
                return 0;
        }

        // If there is multi times non-number charactor appearing, it
        // will skip them till next number comes
        if (start != i){
            // Length of string must larger than 0
            String meta = s.substring(start, i);

            /*
            // If it failed to convert, it will return 0.
            if (len == 0)       // Mode Type
                (*m).mode = (MODES)meta.toInt();
            else if (len == 1)  // Start time
                (*m).start_time = meta.toInt();
            else{               // Other parameter
                (*m).param[len-2] = meta.toInt();
                checksum += (*m).param[len-2];
            }
            len++;
            */
            char key = s[start-1];
            switch(key){
                case 'M': (*m).mode = (MODES)meta.toInt(); break;
                case 'S': (*m).start_time = meta.toInt();  break;
                case 'D': (*m).duration = meta.toInt();    break;
                case 'X': feed_color_param(&(m->XH), meta);break;
                case 'Y': feed_color_param(&(m->XS), meta);break;
                case 'Z': feed_color_param(&(m->XV), meta);break;
                case 'U': feed_color_param(&(m->YH), meta);break;
                case 'V': feed_color_param(&(m->YS), meta);break;
                case 'W': feed_color_param(&(m->YV), meta);break;
                case 'Z': 
                    int comma = meta.indexOf(',');
                    uint32_t upper = meta.substring(0, comma).toInt();
                    m->p[0] = (upper >> 8) & 0xff;
                    m->p[1] = upper        & 0xff;
                    
                    uint32_t lower = meta.substring(comma+1).toInt();
                    m->p[2] = (lower) >> 8) & 0xff;
                    m->p[3] = lower         & 0xff;
                    break;
            }
        }
        start = i+1;
    }
    // End without checksum, unusual request
    return 0;
}

bool Communication::receive(Mode* m, int request_id){
    if (WiFi.status() == WL_CONNECTED){
        /* Request data from server */
        String url = String(WIFI_REQUEST_URL) + "?id=" + request_id;
        http.begin(url);
        int httpCode = http.GET();
        String web_data = http.getString();

        if (feed_data(m, web_data) == 0){
            // Message error, Report it
        }
        http.end();

        /* Report connection time */ 
        url = WIFI_RESPOND_URL;
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

void Communication::updateOTA(){
    ArduinoOTA.handle();
}
