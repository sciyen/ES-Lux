#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

#include "core.h"
#include "config.h"

void PrintColorSch(ValueParam* v);
void PrintMode(Mode* m);

class Communication{
private:

    void OTA();

    /* Helper function to feed color parameter
     */
    static void feed_color_param(ValueParam* p, String s);

    /* Split the http get message into parameters
     * The accepted format: 
     * [Mode Type],[Start Time],[p1],[p2]....;
     * End with `;`
     * The limitaion length of parameter is defined as `META_PARAMETER_BUF_SIZE`
     * If the checksum failed, it will return 0.
     * 
     * @Param:
     *      m: the address of memory that requested data will be storage
     *      s: the string to split
     * @Output:
     *      len: the number of parameter received.
     */
    int feed_data(Mode* m, String s);

public:
    Communication();

    void init();
    
    /* Try to connect to the local AP
     * Block util connected.
     */
    void connect();

    /* Send request to server and try to parse the message.
     * Return false if it failed.
     */
    bool receive(Mode* m, int current_id);

    void updateOTA();

    /* Obtain current music time stamp and return current playing effect*/
    time_t check_start_time(uint8_t id, MODES mode);

    void WifiErrorHandle();
};
/*
void wifi_connect();
void wifi_get_data();
String wifi_receive_message();
void OTA();*/

#endif
