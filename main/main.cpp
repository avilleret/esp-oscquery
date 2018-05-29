#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "http_server.h"
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include "ossia.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  return ESP_OK;
}

static const char* TAG = "http_server";

ossia::node root;

static void cb_GET_root_method(http_context_t http_ctx, void* ctx)
{
  const char* json = root.to_string();
  size_t response_size = strlen(json);
  http_response_begin(http_ctx, 200, "text/html", response_size);
  http_buffer_t http_index_html = { .data = json };
  http_response_write(http_ctx, &http_index_html);
  http_response_end(http_ctx);
}


extern "C" {
void app_main(void)
{
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  wifi_config_t sta_config = {
    {
      "dlink-4240",
      "ejjeb48364",
      WIFI_FAST_SCAN,
      false
    }
  };
  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
  ESP_ERROR_CHECK( esp_wifi_start() );
  ESP_ERROR_CHECK( esp_wifi_connect() );

  // create HTTP server
  http_server_t server;
#ifdef HTTPS_SERVER
  http_server_options_t http_options = HTTPS_SERVER_OPTIONS_DEFAULT();
#else
  http_server_options_t http_options = HTTP_SERVER_OPTIONS_DEFAULT();
#endif
  esp_err_t res;

  ESP_LOGI(TAG, "Creating Example Server!");
  ESP_ERROR_CHECK( res =  http_server_start(&http_options, &server) );
  if (res != ESP_OK) {
    return;
  }
  ESP_LOGV(TAG, "OK");

  ESP_LOGI(TAG, "Registering Handler!");
  ESP_ERROR_CHECK( res = http_register_handler(server, "/", HTTP_GET, HTTP_HANDLE_RESPONSE, &cb_GET_root_method, NULL) );
  if (res != ESP_OK) {
    return;
  }
  ESP_LOGV(TAG, "OK");

  // create some Ossia node
  root.add_child("foo").add_parameter().set_value(-3.1415);
  auto bar = root.add_child("bar").add_parameter();
  bar.set_value(123.456);

  while(true)
  {
    int v = rand();
    bar.set_value(v);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGV("OSSIA", "New value generated: %d\n", v);
  }
}
}
