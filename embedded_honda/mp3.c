#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "hal/uart_types.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "soc/uart_struct.h"
#include "sdkconfig.h"

#define CMD_PLAY_NEXT 0x01
#define CMD_PLAY_PREV 0x02
#define CMD_PLAY_W_INDEX 0x03
#define CMD_SET_VOLUME 0x06
#define CMD_SEL_DEV 0x09
#define CMD_PLAY_W_VOL 0x22
#define CMD_PLAY 0x0D
#define CMD_PAUSE 0x0E
#define CMD_SINGLE_CYCLE 0x19

#define DEV_TF 0x02
#define SINGLE_CYCLE_ON 0x00
#define SINGLE_CYCLE_OFF 0x01


//control pins
#define VOLUME_GPIO GPIO_NUM_33
#define PLAY_BUTTON GPIO_NUM_32
#define NEXT_BUTTON GPIO_NUM_35
#define PREV_BUTTON GPIO_NUM_34

#define PLAY_BUTTON_BIT_MASK (1ULL << PLAY_BUTTON)
#define NEXT_BUTTON_BIT_MASK (1ULL << NEXT_BUTTON)
#define PREV_BUTTON_BIT_MASK (1ULL << PREV_BUTTON)

#define BUF_SIZE 1024


//static esp_adc_cal_characteristics_t adc1_chars;


static void mp3_command(int8_t command, int16_t dat) {

  int8_t frame[BUF_SIZE] = { 0 };
  frame[0] = 0x7e;                // starting byte
  frame[1] = 0xff;                // version
  frame[2] = 0x06;                // the number of bytes of the command without starting byte and ending byte
  frame[3] = command;             //
  frame[4] = 0x00;                // 0x00 = no feedback, 0x01 = feedback
  frame[5] = (int8_t)(dat >> 8);  // data high byte
  frame[6] = (int8_t)(dat);       // data low byte
  frame[7] = 0xef;                // ending byte

  //send data
  uart_write_bytes(UART_NUM_2, (const char*)frame, 8);
}


void app_main(void) {
  
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    
    .rx_flow_ctrl_thresh = 122,
  };

  uart_param_config(UART_NUM_2, &uart_config);
  uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  
  //rx buffer length must be >= 128
  uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);


  vTaskDelay(500 / portTICK_PERIOD_MS); // wait chip initialization is complete
  
  mp3_command(CMD_SEL_DEV, DEV_TF);  // select the TF card
  vTaskDelay(200 / portTICK_PERIOD_MS);  // wait again



  //gpio_set_direction(VOLUME_GPIO, GPIO_MODE_INPUT);

  //buttons
  gpio_set_direction(PLAY_BUTTON, GPIO_MODE_INPUT);
  gpio_set_direction(NEXT_BUTTON, GPIO_MODE_INPUT);
  gpio_set_direction(PREV_BUTTON, GPIO_MODE_INPUT);

  
  //analog read
  //esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &read_raw);
  //esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
  adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);




  //control mp3
  float vol = 0;
  float lastVol = 0;
  bool play = false;
  
  //play first file on card
  mp3_command(CMD_PLAY, 0x0000);       // Play mp3
  

  vol = (((float)adc1_get_raw(ADC1_CHANNEL_5) / 4096.0) - 0.42) / 0.57;
  mp3_command(CMD_SET_VOLUME, (int)(vol*100.0));
  printf("Setting volume at: %d\n", (int)(vol*100.0));

  
  //mp3_command(CMD_PAUSE, 0x0000);      // Pause mp3
  //mp3_command(CMD_PLAY_NEXT, 0x0000);  // Play next mp3
  //mp3_command(CMD_PLAY_PREV, 0x0000);  // Play previous mp3
  //mp3_command(CMD_SET_VOLUME, 30);     // Change volume to 30

  while (1) {

    //read adc and calculate volume
    vol = (((float)adc1_get_raw(ADC1_CHANNEL_5) / 4096.0) - 42.0) / 57.0;
    //mp3_command(CMD_SET_VOLUME, (int)(vol*100.0));
    
    //printf("%d\n", (int)(vol*100.0));
    /*
    //adjust volume
    mp3_command(CMD_SET_VOLUME, (int)(vol*100.0));
    */


    //buttons for different commands
    if (gpio_get_level(PLAY_BUTTON) == 1) {
    
      if (play) { mp3_command(CMD_PLAY, 0x0000);
      } else {    mp3_command(CMD_PAUSE, 0x0000); }

      play ^= 1;
    }
    
    if (gpio_get_level(NEXT_BUTTON) == 1) {
    
      mp3_command(CMD_PLAY_NEXT, 0x0000);
    }

    if (gpio_get_level(PREV_BUTTON) == 1) {
    
      mp3_command(CMD_PLAY_PREV, 0x0000);
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}