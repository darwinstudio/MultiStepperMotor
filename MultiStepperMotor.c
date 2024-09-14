#include "MultiStepperMotor.h"

static struct STEPPER_MOTOR *stepper_motor_group_head[STEPPER_MOTOR_GROUPS_NUM] = {NULL};

/**
 * @brief 电机结构体初始化
 *
 * @param motor 电机结构体指针
 * @param clk_control_t CLK引脚电平翻转函数
 * @param dir_control_t DIR引脚电平控制函数
 * @param forwrd_pin_level_t 前进方向的电平状态
 * @param callback_t 电机停止后的回调函数
 */
void stepper_motor_init(struct STEPPER_MOTOR *motor, void (*clk_toggle_t)(uint8_t), void (*dir_control_t)(uint8_t), uint8_t forwrd_pin_level_t, void (*callback_t)(void))
{
    motor->clk_toggle_pin = clk_toggle_t;
    motor->dir_control = dir_control_t;
    motor->forward_pin_level = forwrd_pin_level_t;
    motor->step_counter = 0;
    motor->sw_control = STEPPER_MOTOR_OFF;
    motor->target_steps = 0;
    motor->state = SMOTOR_STATE_IDLE;
    motor->stopcallback = callback_t;
}

/**
 * @brief 注册电机
 *
 * @param motor 电机结构体指针
 * @param group_id 注册的电机组号
 * @return int 0 注册成功，-1 注册失败
 */
int stepper_motor_register(struct STEPPER_MOTOR *motor, uint8_t group_id)
{
    struct STEPPER_MOTOR *head_handle = NULL;
    if (group_id >= STEPPER_MOTOR_GROUPS_NUM)
    {
        return -1;
    }
    head_handle = stepper_motor_group_head[group_id];
    struct STEPPER_MOTOR *target = head_handle;
    while (target)
    {
        if (target == motor)
        {
            return -1;
        }
        target = target->next;
    }
    motor->next = head_handle;
    head_handle = motor;
    return 0;
}

/**
 * @brief 电机组周期运行函数
 *
 * @param group_id 要运行的电机组号
 */
void stepper_motor_group_ticks(uint8_t group_id)
{
    struct STEPPER_MOTOR *group_head_handle;
    ;
    struct STEPPER_MOTOR *target;
    if (group_id >= STEPPER_MOTOR_GROUPS_NUM)
    {
        return;
    }
    group_head_handle = stepper_motor_group_head[group_id];
    for (target = group_head_handle; target; target = target->next)
    {
        if (target->sw_control == STEPPER_MOTOR_ON)
        {
            target->clk_toggle_pin();
        }
    }
}
