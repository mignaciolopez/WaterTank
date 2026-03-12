//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once
#include <ESPAsyncWebServer.h>

namespace CE::Services
{
    class WSServer
    {
    public:
        static bool Setup();
        static void Loop();
        
        // Send data to all connected clients
        static void BroadcastDistance(float distanceCm);
        static void BroadcastWeather(const Domain::WeatherSample& sample);
        static void BroadcastPumpStatus(const Domain::States::Pump& sample);

        static void BroadcastSettings();

    private:
        static AsyncWebServer _server;
        static AsyncWebSocket _ws;
        
        static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                            AwsEventType type, void *arg, uint8_t *data, size_t len);
        static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    };
}
