/**
 * @file ADS1256_Config.h
 * @author TITH (1023515576@qq.com)
 * @brief ADS1256的驱动程序配置文件，不需要include此文件
 * @version 0.2
 * @date 2022-01-11
<<<<<<< HEAD
 * 
 * @copyright Copyright (c) 2022
 * 
=======
 *
 * @copyright Copyright (c) 2022
 *
>>>>>>> 4b84313b340da237f4021acaf5f1efda636c46d5
 */

#pragma once

#include "ADS1256.h"

// 引脚配置

<<<<<<< HEAD
#define ADS1256_DRDY_GPIOx GPIOA
#define ADS1256_DRDY_GPIO_Pin GPIO_PIN_4

#define ADS1256_Reset_GPIOx GPIOB
=======
#define ADS1256_DRDY_GPIOx     GPIOA
#define ADS1256_DRDY_GPIO_Pin  GPIO_PIN_4

#define ADS1256_Reset_GPIOx    GPIOB
>>>>>>> 4b84313b340da237f4021acaf5f1efda636c46d5
#define ADS1256_Reset_GPIO_Pin GPIO_PIN_2

// 控制是否启用NSS片选

// #define ADS1256_NSS_Enable
#ifdef ADS1256_NSS_Enable
<<<<<<< HEAD
	#define ADS1256_NSS_GPIOx 
	#define ADS1256_NSS_GPIO_Pin
=======
#define ADS1256_NSS_GPIOx
#define ADS1256_NSS_GPIO_Pin
>>>>>>> 4b84313b340da237f4021acaf5f1efda636c46d5
#endif // 有片选引脚可以打开
