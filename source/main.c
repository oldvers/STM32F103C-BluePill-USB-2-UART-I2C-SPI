#include <stdio.h>

#include "types.h"
#include "board.h"
#include "gpio.h"
#include "uniquedevid.h"
#include "system.h"

#include "log.h"
#include "fifo.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "usb_device.h"

void vLEDTask(void * pvParameters)
{
  GPIO_Init(GPIO_C, 13, GPIO_TYPE_OUT_OD_2MHZ, 0);

  while(1)
  {
    GPIO_Lo(GPIO_C, 13);
    LOG_SetTextColorGreen();
    printf("LED On\r\n");
    vTaskDelay(500);
    GPIO_Hi(GPIO_C, 13);
    LOG_SetTextColorRed();
    printf("LED Off\r\n");
    vTaskDelay(500);
  }
  //vTaskDelete(NULL);
}

void vDualUartTask(void * pvParameters)
{
//  /* Free PB3 from JTAG */
//  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
//  AFIO->MAPR &= ~(7 << 24);
//  AFIO->MAPR |= (2 << 24);

//  /* Init PB3 */
//  GPIO_Init(SWD_SWO_PORT, SWD_SWO_PIN, GPIO_TYPE_OUT_PP_50MHZ, 0);

//  /* Enable WiFi Module */
//  GPIO_Init(WIFI_EN_PORT, WIFI_EN_PIN, GPIO_TYPE_OUT_PP_2MHZ, 1);

//  /* Test Pins */
//  GPIO_Init(GPIOA,  7, GPIO_TYPE_OUT_PP_50MHZ, 0);
//  GPIO_Init(GPIOB, 11, GPIO_TYPE_OUT_PP_50MHZ, 0);

  /* Init PB2 to OD Hi-Z - Switch-off 1k5 PullUp from USB D+ */
  GPIO_Init(USB_PUP_PORT, USB_PUP_PIN, GPIO_TYPE_OUT_OD_2MHZ, 1);

  /* Delay */
  vTaskDelay(200);

  /* Init USB. Switch-on 1k5 PullUp to USB D+ - connect USB device */
  USBD_Init();
  GPIO_Lo(USB_PUP_PORT, USB_PUP_PIN);

  vTaskDelay(5000);

  while(FW_TRUE)
  {
    vTaskDelay(5000);
  }
  //vTaskDelete(NULL);
}

int main(void)
{
  LOG_Init();
  LOG_ClearScreen();
  LOG_SetDefaultColors();

  LOG("STM32F103C8 Started!\r\n");
  LOG("Double UART Converter Task Started\r\n");
  LOG("ID0         = 0x%04X\r\n", UDID_0);
  LOG("ID1         = 0x%04X\r\n", UDID_1);
  LOG("ID2         = 0x%08X\r\n", UDID_2);
  LOG("ID2         = 0x%08X\r\n", UDID_3);
  LOG("Memory Size = %d kB\r\n", FLASH_SIZE);
  LOG("CPU clock   = %d Hz\r\n", CPUClock);
  LOG("AHB clock   = %d Hz\r\n", AHBClock);
  LOG("APB1 clock  = %d Hz\r\n", APB1Clock);
  LOG("APB2 clock  = %d Hz\r\n", APB2Clock);

  xTaskCreate
  (
    vLEDTask,
    "LED",
    configMINIMAL_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 1,
    NULL
  );

  xTaskCreate
  (
    vDualUartTask,
    "Dual UART",
    configMINIMAL_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 1,
    NULL
  );

  vTaskStartScheduler();

  while (FW_TRUE) {};
}

void on_error(void)
{
  while (FW_TRUE) {};
}
