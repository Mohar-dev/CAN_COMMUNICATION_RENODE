
#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>

CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x11, 0x22, 0x33, 0x44};
uint32_t TxMailbox;

extern UART_HandleTypeDef huart2; // UART2 for logs
extern CAN_HandleTypeDef hcan1;   // CAN1 handle

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();

  HAL_CAN_Start(&hcan1);

  char *msg = "MCU1: Sending CAN message...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  TxHeader.DLC = 8;            // 8 bytes data
  TxHeader.IDE = CAN_ID_STD;   // Standard ID
  TxHeader.RTR = CAN_RTR_DATA; // Data frame
  TxHeader.StdId = 0x321;      // ID 0x321
  Printf(“Sending the message from machine0”);
  while (1)
  {
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) == HAL_OK)
    {
      char *sent = "MCU1: CAN Message Sent Successfully\r\n";
      HAL_UART_Transmit(&huart2, (uint8_t*)sent, strlen(sent), HAL_MAX_DELAY);
    }
    HAL_Delay(1000); // Send every 1 second
  }
}

#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

extern UART_HandleTypeDef huart2; // UART2 for logs
extern CAN_HandleTypeDef hcan1;   // CAN1 handle

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();

  HAL_CAN_Start(&hcan1);

  // Enable interrupt when message is received
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

  char *msg = "MCU2: Waiting for CAN messages...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  while (1)
  {
    // You can also handle received messages in interrupt
    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
    {
      if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
      {
        char buffer[128];
        sprintf(buffer, "MCU2: Received CAN ID: 0x%03lX Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                RxHeader.StdId,
                RxData[0], RxData[1], RxData[2], RxData[3],
                RxData[4], RxData[5], RxData[6], RxData[7]);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
      }
    }
  }
}
