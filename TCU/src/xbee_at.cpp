#include "xbee_at.h"
#include "debug.h"

#define MAX_CALLBACK_COUNT 2
#define CALLBACK_INTERVAL_US 1000 // 1 ms

/* Format constants */
#define START_TOKEN 'T'
#define END_TOKEN '\r'


static XBeeCallback callbacks[MAX_CALLBACK_COUNT];
static int num_callbacks;
static IntervalTimer callback_timer;

static HardwareSerial *port;

/**
 * @brief Run every CALLBACK_INTERVAL_US after we register a callback.
 * Checks for received data and sends it to the callback functions.
 * 
 */
static void notify_callbacks() {
    if ((*port).available() > 0) {
        uint8_t buffer[MAX_READ_LENGTH];
        int read_bytes = (*port).readBytesUntil('\n', buffer, MAX_READ_LENGTH);

        for (int i = 0; i < num_callbacks; i++) {
            callbacks[i](buffer, read_bytes);
        }
    }
}


XBEE_STATUS XBeeInit(HardwareSerial *serialPort, uint32_t baudRate) {
    port = serialPort;
    (*port).begin(baudRate);
    return XBEE_STATUS::XB_SUCCESS;
}


XBEE_STATUS XBeeRegisterCallback(XBeeCallback callback) {
    if (num_callbacks >= MAX_CALLBACK_COUNT) {
        return XBEE_STATUS::XB_ERROR_MAX_CALLBACKS;
    }
    else if (num_callbacks == 0) {
        // start the callback timer when we register first one
        callback_timer.begin(notify_callbacks, CALLBACK_INTERVAL_US);
    }

    callbacks[num_callbacks] = callback;
    num_callbacks++;

    return XBEE_STATUS::XB_SUCCESS;
}


XBEE_STATUS XBeeSendMessage(message_t *message) {
    char encodedMessage[37];
    snprintf(&encodedMessage[0], 18, "%c%.13llu%.3x%.1hu", START_TOKEN, message->timestamp, message->id, message->length);
    for (int i = 0; i < message->length; i++) {
        snprintf(&encodedMessage[19 + 2*i], 2, "%.2x", message->dataBuf[i]);
    }
    snprintf(&encodedMessage[19 + message->length*2], 2, "%c\0", END_TOKEN);

    (*port).print(encodedMessage);
    return XBEE_STATUS::XB_SUCCESS;
}


XBEE_STATUS XBeeReceiveMessage(uint8_t *buf, uint32_t maxLen, uint32_t *receivedLength) {
    if (num_callbacks > 0) {
        return XBEE_STATUS::XB_ERROR_RECEIVE_MODE;
    }
    else if ((*port).available() <= 0) {
        return XBEE_STATUS::XB_ERROR_RX_EMPTY;
    }

    // read until we get the newline character
    uint32_t num_read = (*port).readBytesUntil('\n', buf, MAX_READ_LENGTH);

    if (num_read == 0 || num_read == MAX_READ_LENGTH) {
        return XBEE_STATUS::XB_ERROR_RECEIVE_FORMAT;
    }

    return XBEE_STATUS::XB_SUCCESS;
}
