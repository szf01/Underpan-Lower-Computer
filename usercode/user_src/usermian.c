/*
 * @Author: szf
 * @Date: 2022-10-22 13:54:44
<<<<<<< HEAD
 * @LastEditTime: 2022-12-12 22:47:16
 * @LastEditors: szf
=======
 * @LastEditTime: 2022-12-07 00:31:27
 * @LastEditors: szf_8d43 2176529058@qq.com
>>>>>>> 2561b48b9297d54f81a4a2793aff16a0a8f9bca9
 * @Description:
 * @FilePath: \underpan_v3.1\usercode\user_src\usermian.c
 * @WeChat:szf13373959031
 */
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "Caculate.h"
#include "wtr_can.h"
#include "DJI.h"
#include "wtr_uart.h"
#include <math.h>
#include "main.h"
#include "usermain.h"
#include "wtr_mavlink.h"
#include "ADS1256.h"
#include <math.h>
int test = 0;
int counter = 0;
float w_speed = 0;
/**
 * @description:
 * @author: szf
 * @date:
 * @return {*}
 */
void calculate_3(double *moter_speed,
                 double v_x,
                 double v_y,
                 double v_w)
{
    moter_speed[0] = (-v_x * sin(30 * DEC) - v_y * cos(30 * DEC) + v_w * r_underpan_3) / (2 * pi * r_wheel);
    moter_speed[1] = (+v_x + v_w * r_underpan_3) / (2 * pi * r_wheel);
    moter_speed[2] = (-v_x * sin(30 * DEC) + v_y * cos(30 * DEC) + v_w * r_underpan_3) / (2 * pi * r_wheel);
}

/**
 * @description:
 * @author: szf
 * @date:
 * @return {void}
 */
void calculate_3_2(double *moter_speed,
                   double v_x,
                   double v_y,
                   double v_w)
{
    moter_speed[2] = (-v_y * sin(30 * DEC) + v_x * cos(30 * DEC) + v_w * r_underpan_3) * 60 / (2 * pi * r_wheel) * 19;
    moter_speed[1] = (+v_y + v_w * r_underpan_3) * 60 / (2 * pi * r_wheel) * 19;
    moter_speed[0] = (-v_y * sin(30 * DEC) - v_x * cos(30 * DEC) + v_w * r_underpan_3) * 60 / (2 * pi * r_wheel) * 19;
}

/**
 * @description:
 * @author: szf
 * @date:
 * @return {void}
 */
void calculate_4(double *moter_speed,
                 double v_x,
                 double v_y,
                 double v_w)
{
    moter_speed[0] = (v_y + v_w * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[1] = (-v_x + v_w * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[2] = (-v_y + v_w * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[3] = (v_x + v_w * r_underpan_4) / (2 * pi * r_wheel);
}

/**
 * @description:
 * @author: szf
 * @date:
 * @return {void}
 */
void calculate_4_2(double *moter_speed,
                   double v_x,
                   double v_y,
                   double v_w)
{
    moter_speed[0] = (vx * sqrt(2) + vy * sqrt(2) + vw * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[1] = (-vx * sqrt(2) + vy * sqrt(2) + vw * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[2] = (-vx * sqrt(2) - vy * sqrt(2) + vw * r_underpan_4) / (2 * pi * r_wheel);
    moter_speed[3] = (vx * sqrt(2) - vy * sqrt(2) + vw * r_underpan_4) / (2 * pi * r_wheel);
}

/**
 * @description: ?????????PID
 * @param {PID} *vPID
 * @param {float} processValue
 * @return {*}
 */
void PID_Incremental(PID *vPID, float processValue)
{
    float thisError;
    float increment;
    float pError, dError, iError;

    thisError = vPID->setpoint - processValue; //??????????????????????????????????????????
    //???????????????????????????????????? ??????
    pError = thisError - vPID->lasterror; //??????????????????err(k)-err(k-1)
    iError = thisError;
    dError = thisError - 2 * (vPID->lasterror) + vPID->preerror;

    increment = vPID->Kp * pError + vPID->Ki * iError + vPID->Kd * dError; //????????????

    vPID->preerror = vPID->lasterror; //??????????????????????????????
    vPID->lasterror = thisError;

    vPID->result += increment; //????????????????????? ??????????????????
    vPID->result = range(vPID->result, -vPID->limit, vPID->limit);
    // if(vPID->result > 0.6)
    // {
    //     vPID->result = 0.6;
    // }
    // if (vPID->result < -0.6)
    // {
    //     vPID->result = -0.6;
    // }//????????????
}

/**
 * @description: ?????????PID
 * @param {PIDType} *p
 * @return {*}
 */
float PID_Position(PIDType *p)
{
    float pe, ie, de;
    float out = 0;

    //??????????????????
    p->e0 = p->target - p->feedback;

    //????????????
    p->eSum += p->e0;

    //????????????
    de = p->e0 - p->e1;

    pe = p->e0;
    ie = p->eSum;

    p->e1 = p->e0;

    out = pe * (p->Kp) + ie * (p->Ki) + de * (p->Kd);
    //????????????
    out = range(out, -p->limit, p->limit);
    return out;
}

/**
 * @description: ????????????????????????
 * @author: szf
 * @date:
 * @return {void}
 */
void thread_1(void const *argument)
{
    // ?????????????????????
    CANFilterInit(&hcan1);
    hDJI[0].motorType = M3508;
    hDJI[1].motorType = M3508;
    hDJI[2].motorType = M3508;
    hDJI[3].motorType = M3508;
    DJI_Init();
    wtrMavlink_BindChannel(&huart8, MAVLINK_COMM_0);

    // PID????????????
    pid_pos_x.Kp = 0;
    pid_pos_x.Ki = 0;
    pid_pos_x.Kd = 0;
    pid_pos_x.limit = 0.7;

    pid_pos_y.Kp = 0;
    pid_pos_y.Ki = 0;
    pid_pos_y.Kd = 0;
    pid_pos_y.limit = 0.7;

    pid_vel_w.Kp = -0;
    pid_vel_w.Ki = -0;
    pid_vel_w.Kd = 0;
    pid_vel_w.limit = 0.8;

    //?????????pid????????????
    pid_pos_w_pos.Kp = -120;
    pid_pos_w_pos.Ki = 0.0001;
    pid_pos_w_pos.Kd = 0;
    pid_pos_w_pos.limit = 1;

    pid_pos_x_pos.Kp = 4;
    pid_pos_x_pos.Ki = 0.0001;
    pid_pos_x_pos.Kd = 0;
    pid_pos_x_pos.limit = 1;

    pid_pos_y_pos.Kp = 4;
    pid_pos_y_pos.Ki = 0.0001;
    pid_pos_y_pos.Kd = 0;
    pid_pos_y_pos.limit = 1;

    // ??????????????????
    HAL_UART_Receive_DMA(&huart1, JoyStickReceiveData, 18); // DMA??????AS69
    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0);              //???mavlink??????
    osDelay(100);

<<<<<<< HEAD
    
    //?????????AMT102


    // ?????????????????????
=======
>>>>>>> 2561b48b9297d54f81a4a2793aff16a0a8f9bca9
    for (;;) {

        // PID????????????
        pid_pos_x.setpoint = control.x_set;
        pid_pos_y.setpoint = control.y_set;
        pid_vel_w.setpoint = control.vw_set;//??????

        pid_pos_w_pos.target = control.vw_set;
        pid_pos_w_pos.feedback = mav_posture.zangle;
        pid_pos_x_pos.target = control.x_set;
        pid_pos_x_pos.feedback = mav_posture.pos_x;
        pid_pos_y_pos.target = control.y_set;
        pid_pos_y_pos.feedback = mav_posture.pos_y;//??????
        

        PID_Incremental(&pid_pos_x, mav_posture.pos_x);
        PID_Incremental(&pid_pos_y, mav_posture.pos_y);
        PID_Incremental(&pid_vel_w, mav_posture.zangle);

        //??????????????????
        /*         calculate_3_2(moter_speed,
                              control.vx_set + pid_pos_x.result,
                              control.vy_set + pid_pos_y.result,
                              control.vw_set + pid_vel_w.result); */
        calculate_3_2(moter_speed,
                      control.vx_set + PID_Position(&pid_pos_x_pos),
                      control.vy_set + PID_Position(&pid_pos_y_pos),
                      control.vw_set + PID_Position(&pid_pos_w_pos));
/*         calculate_3_2(moter_speed,
                    control.vx_set ,
                    control.vy_set ,
                    PID_Position(&pid_pos_w_pos)); */
                    
        //????????????
        speedServo(moter_speed[0], &hDJI[0]);
        speedServo(moter_speed[1], &hDJI[1]);
        speedServo(moter_speed[2], &hDJI[2]);
        speedServo(moter_speed[3], &hDJI[3]);

        CanTransmit_DJI_1234(&hcan1, hDJI[0].speedPID.output,
                             hDJI[1].speedPID.output,
                             hDJI[2].speedPID.output,
                             hDJI[3].speedPID.output);

        static int n_ = 0;
        if (n_++ > 100) {
            n_ = 0;
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_14); // A???????????????
        }
        osDelay(1);
    }
}

/**
 * @description: ????????????????????????
 * @author: szf
 * @date:
 * @return {void}
 */
void thread_2(void const *argument)
{
    // ??????????????????????????????6????????????
    HAL_UART_Receive_IT(&huart6, (uint8_t *)&ch, 1);
<<<<<<< HEAD
    
    // DT35???????????????
    // ?????????
    ADS1256_Init();
    ADS1256_UpdateDiffData();

=======

    // mavlink_msg_posture_send_struct(MAVLINK_COMM_0,mav_posture);
    // DT35???????????????
    // ?????????
    // ADS1256_Init;
>>>>>>> 2561b48b9297d54f81a4a2793aff16a0a8f9bca9
    for (;;) {
        // ADS1256_UpdateDiffData;
        osDelay(100);
    }
}

/**
 * @description:??????????????????
 * @return {*}
 */
/* void thread_3(void const *argument)
{
    // ????????????????????????????????????????????????????????????

    for (;;) {
        v_state.vx_state = hDJI[0].FdbData.rpm;
        v_state.vy_state = hDJI[1].FdbData.rpm;
        v_state.vw_state = hDJI[2].FdbData.rpm;

        mavlink_msg_speed_control_status_send_struct(MAVLINK_COMM_0, &v_state);


        osDelay(100);
    }
} */

/**
 * @description: ????????????
 * @author: szf
 * @date:
 * @return {void}
 */
void StartDefaultTask(void const *argument)
{
    osThreadDef(speedservo, thread_1, osPriorityNormal, 0, 512);
    osThreadCreate(osThread(speedservo), NULL);

    osThreadDef(position, thread_2, osPriorityNormal, 0, 512);
    osThreadCreate(osThread(position), NULL);

    /*     osThreadDef(velocity, thread_3, osPriorityNormal, 0, 512);
        osThreadCreate(osThread(velocity), NULL); */

    for (;;) {
        osDelay(1);
    }
}

/**
 * @description: ???????????????????????????
 * @param {UART_HandleTypeDef} *huart
 * @author: szf
 * @date:
 * @return {void}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    test++;

    // ???????????????
    if (huart->Instance == UART8) {
        // UART1Decode();//AS69??????
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0); // ??????mavlink??????
    }
    // ??????????????????
    else if (huart->Instance == USART6) // ?????????????????????decode,????????????DMA??????,??????????????????????????????s
    {
        HAL_UART_Receive_IT(&huart6, (uint8_t *)&ch, 1);
        // USART_ClearITPendingBit( USART1, USART_FLAG_RXNE);
        // HAL_UART_IRQHandler(&huart6); // ?????????????????????????????????????????????????????????????????????????????????
        switch (count) // uint8_t?????????int
        {
            case 0:

                if (ch[0] == 0x0d)
                    count++;
                else
                    count = 0;
                break;

            case 1:

                if (ch[0] == 0x0a) {
                    i = 0;
                    count++;
                } else if (ch[0] == 0x0d)
                    ;
                else
                    count = 0;
                break;

            case 2:

                posture.data[i] = ch[0];
                i++;
                if (i >= 24) {
                    i = 0;
                    count++;
                }
                break;

            case 3:

                if (ch[0] == 0x0a)
                    count++;
                else
                    count = 0;
                break;

            case 4:

                if (ch[0] == 0x0d) {
                    mav_posture.zangle = posture.ActVal[0] * 0.001;
                    // mav_posture.xangle = posture.ActVal[1] * 0.001;
                    mav_posture.xangle = control.x_set;
                    mav_posture.yangle = posture.ActVal[2] * 0.001;
                    mav_posture.pos_x = posture.ActVal[3] * 0.001;
                    mav_posture.pos_y = posture.ActVal[4] * 0.001;
                    // mav_posture.w_z = posture.ActVal[5] * 0.001;
                    mav_posture.w_z = control.vx_set;
                }
                count = 0;
                break;

            default:

                count = 0;
                break;
        }
    } 
    else {
        AS69_Decode(); // AS69??????
    }
}

/**
 * @brief ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
 *
 * @param msg ??????????????????
 * @return
 */

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{

    switch (msg->msgid) {
        case 9:
            // id = 9 ??????????????????????????????(mavlink_msg_xxx_decode)
            mavlink_msg_control_set_decode(msg, &control);
            mavlink_msg_posture_send_struct(MAVLINK_COMM_0, &mav_posture); // ?????????????????????
            break;
        case 2:
            // id = 2 ??????????????????????????????(mavlink_msg_xxx_decode)
            break;
        // ......
        default:
            break;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    counter++;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  if (htim->Instance == TIM2) {
    w_speed = counter / 2048 / 0.02;
    counter = 0;
  }
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}