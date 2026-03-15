//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include <Domain/Samples.hpp>
#include <services/Pump.h>
#include <services/Filter.h>
#include <services/Weather.h>
#include <services/WSServer.h>
#include <os/Settings.h>

namespace CE::Services
{
    static const char* TAG = "WSServer-Service";
    
    AsyncWebServer WSServer::_server(80);
    AsyncWebSocket WSServer::_ws("/ws");
    
    bool WSServer::Setup()
    {
        _ws.onEvent(onWsEvent);
        _server.addHandler(&_ws);
        
        // Optional: Add HTTP endpoint for status
        _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", "ESP32 Water Tank WebSocket Server");
        });
        
        _server.begin();
        ESP_LOGI(TAG, "WebSocket server started on port 80");

        return true;
    }
    
    void WSServer::Loop()
    {
        _ws.cleanupClients(); // Clean up disconnected clients
    }
    
    void WSServer::onWsEvent(AsyncWebSocket */*server*/, AsyncWebSocketClient *client, const AwsEventType type, void *arg, uint8_t *data, const size_t len)
    {
        switch(type)
        {
            case WS_EVT_CONNECT:
                ESP_LOGI(TAG, "Client #%u connected from %s", client->id(), client->remoteIP().toString().c_str());
                Broadcast("settings", OS::Settings::Get());
                break;
                
            case WS_EVT_DISCONNECT:
                ESP_LOGI(TAG, "Client #%u disconnected", client->id());
                break;
                
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
                break;
                
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    }
    
    void WSServer::handleWebSocketMessage(void *arg, uint8_t *data, const size_t len)
    {
        const auto info = static_cast<AwsFrameInfo*>(arg);
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            data[len] = 0; // Null terminate
            
            JsonDocument doc;
            const DeserializationError error = deserializeJson(doc, reinterpret_cast<char *>(data));
            
            if (error)
            {
                ESP_LOGE(TAG, "JSON parse error: %s", error.c_str());
                return;
            }
            
            const char* command = doc["command"];
            
            if (strcmp(command, "getPumpStatus") == 0)
            {
                Broadcast("pumpStatus", Pump::GetState());
            }
            else if (strcmp(command, "setPump") == 0)
            {
                Pump::Switch(doc["state"]);
                Broadcast("pumpStatus", Pump::GetState());
            }
            else if (strcmp(command, "getStatus") == 0)
            {
                // Send all current statuses
                Domain::WeatherSample weatherSample;
                if (Weather::ReadLast(weatherSample))
                {
                    Broadcast("weather", weatherSample);
                }
                
                Domain::RadarSample radarSample;
                if (Filter::TryGetLatestFilteredCm(radarSample))
                {
                    radarSample.distanceCm /= 100.0f;
                    Broadcast("distance", radarSample);
                }
                
                Broadcast("pumpStatus", Pump::GetState());
            }
            else if (strcmp(command, "getSettings") == 0)
            {
                Broadcast("settings", OS::Settings::Get());
            }
            else if (strcmp(command, "setSettings") == 0)
            {
                ESP_LOGI(TAG, "Processing command setSettings");
                OS::Settings::Get().fromJson(doc.as<JsonObject>());
                if (OS::Settings::Save())
                {
                    if (OS::Settings::Load())
                    {
                        Broadcast("settings", OS::Settings::Get());
                    }
                }
            }
        }
    }

    template <typename T>
    void WSServer::Broadcast(const char* header, const T& sample)
    {
        JsonDocument doc;
        doc["header"]   =   header;
        sample.toJson(doc.as<JsonVariant>(), "payload");

        String buffer;
        const size_t len = serializeJson(doc, buffer);
        _ws.textAll(buffer.c_str(), len);
    }
}
