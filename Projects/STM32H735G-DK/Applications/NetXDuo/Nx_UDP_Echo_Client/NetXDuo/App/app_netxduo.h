/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.h
  * @author  MCD Application Team
  * @brief   NetXDuo applicative header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* Private includes ----------------------------------------------------------*/

#include "nx_stm32_eth_driver.h"

/* USER CODE BEGIN Includes */
#include "nxd_dhcp_client.h"
#include "main.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define PRINT_IP_ADDRESS(addr)         do {                                         \
                                            printf("%s: %lu.%lu.%lu.%lu \n", #addr, \
                                            (addr >> 24) & 0xff,                    \
                                            (addr >> 16) & 0xff,                    \
                                            (addr >> 8) & 0xff,                     \
                                            (addr & 0xff));                         \
                                       } while(0)

#define PRINT_DATA(addr, port, data)   do {                                           \
                                            printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \n", \
                                            (addr >> 24) & 0xff,                      \
                                            (addr >> 16) & 0xff,                      \
                                            (addr >> 8) & 0xff,                       \
                                            (addr & 0xff), port, data);               \
                                       } while(0)
/* USER CODE END PM */

/* Exported functions prototypes ---------------------------------------------*/
UINT App_NetXDuo_Init(VOID *memory_ptr);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

#define PAYLOAD_SIZE             1536
#define NX_PACKET_POOL_SIZE      (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 20)

#define DEFAULT_MEMORY_SIZE      1024
#define DEFAULT_PRIORITY         10

#define NULL_ADDRESS             0

#define DEFAULT_PORT             6000

#define UDP_SERVER_ADDRESS       IP_ADDRESS(192, 168, 1, 1)
#define UDP_SERVER_PORT          6001

#define MAX_PACKET_COUNT         10
#define DEFAULT_MESSAGE          "NetXDuo UDP Client on STM32H735-DK"
/* USER CODE END Private defines */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
 }
#endif
#endif /* __APP_NETXDUO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
