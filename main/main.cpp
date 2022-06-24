/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <cstdlib>
#include <thread>
#include "esp_log.h"
#include "gpio_cxx.hpp"
#include "lib/tcpip/tcplistener.h"
#include <list>
#include <string.h>
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "trace.h"

#define SSID "Koole Controls"
#define PSWD "K@u5tGD!8Ug&X!rc"

using namespace idf;
using namespace std;

TraceDebugger tracer;

esp_err_t event_handler(void* ctx, system_event_t* event)
{
	return ESP_OK;
}
void StartWIFI()
{
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t sta_config = { };
	memcpy(sta_config.sta.ssid, SSID, strlen(SSID));
	memcpy(sta_config.sta.password, PSWD, strlen(PSWD));
	sta_config.sta.bssid_set = false;
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());

	setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
	tzset();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}


extern "C" void app_main(void)
{
	StartWIFI();
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	tracer.Start();
	int i = 0;
	while (true)
	{
		double timeSinceStart = esp_timer_get_time();
		timeSinceStart /= 1000000;
		tracer.SendMeasurementTest(timeSinceStart, i, 1);
		i++;
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}



