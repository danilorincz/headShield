//#include "driver/adc.h"
//#include "esp_adc_cal.h"
void setup() {
  Serial.begin(115200);
  //adc1_ulp_enable();
  //adc1_config_width(ADC_WIDTH_12Bit);
  //adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
}

void loop() {
Serial.println(analogRead(36));
delay(20);
return;
//Serial.println(adc1_get_raw(ADC1_CHANNEL_3));
}
