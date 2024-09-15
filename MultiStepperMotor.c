#include "MultiStepperMotor.h"

static struct STEPPER_MOTOR *stepper_motor_group_head[STEPPER_MOTOR_GROUPS_NUM] = {NULL};

/**
 * @brief 电机结构体初始化
 *
 * @param motor 电机结构体指针
 * @param clk_toggle_t CLK引脚电平翻转函数
 * @param dir_control_t DIR引脚电平控制函数
 * @param forwrd_pin_level_t 前进方向的电平状态
 * @param callback_t 电机停止后的回调函数
 * @return int8_t 0 初始化成功，-1 初始化失败
 */
int8_t stepper_motor_init(struct STEPPER_MOTOR *motor, void (*clk_toggle_t)(uint8_t), void (*dir_control_t)(uint8_t), uint8_t forwrd_pin_level_t, void (*callback_t)(void))
{
    if (motor == NULL)
    {
        return -1;
    }
    motor->next = NULL;
    motor->clk_toggle_pin = clk_toggle_t;
    motor->dir_control = dir_control_t;
    motor->forward_pin_level = forwrd_pin_level_t;
    motor->step_counter = 0;
    motor->sw_control = STEPPER_MOTOR_OFF;
    motor->target_steps = 0;
    motor->state = SMOTOR_STATE_IDLE;
    motor->stopcallback = callback_t;
    return 0;
}

/**
 * @brief 注册电机
 *
 * @param motor 电机结构体指针
 * @param group_id 注册的电机组号
 * @return int8_t 0 注册成功，-1 注册失败
 */
int8_t stepper_motor_register(struct STEPPER_MOTOR *motor, uint8_t group_id)
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
 * @brief 电机运行函数
 *
 * @param motor 电机结构体指针
 * @param dir_t 运动方向: SMOTOR_DIR_FORWARD-向前运动，SMOTOR_DIR_BACKWARD-向后运动
 * @param steps 要运动的步数
 * @return int8_t 0 运行成功，-1 参数非法 -2 电机已运行
 */
int8_t stepper_motor_run(struct STEPPER_MOTOR *motor, uint8_t dir_t, uint32_t steps)
{
    // 检查参数
    if (motor == NULL)
    {
        return -1;
    }
    if (motor->sw_control == STEPPER_MOTOR_ON)
    {
        return -2;
    }
    if (steps == 0)
    {
        return -1;
    }
    // 设置运动方向电平
    if (dir_t == SMOTOR_DIR_FORWARD)
    {
        motor->dir_control(motor->forward_pin_level);
    }
    else if (dir_t == SMOTOR_DIR_BACKWARD)
    {
        motor->dir_control(!motor->forward_pin_level);
    }
    else
    {
        return -1;
    }
    // 设置步数
    motor->target_steps = steps;
    motor->step_counter = 0;
    motor->state = SMOTOR_STATE_MOVING;
    motor->sw_control = STEPPER_MOTOR_ON;
    return 0;
}

/**
 * @brief 停止电机运行函数
 *
 * @param motor 电机结构体指针
 * @return int8_t 0 停止成功，-1 参数非法，-2 电机本来就是停止的
 */
int8_t stepper_motor_stop(struct STEPPER_MOTOR *motor)
{
    if (motor == NULL)
    {
        return -1;
    }
    if (motor->sw_control == STEPPER_MOTOR_OFF)
    {
        return -2;
    }
    motor->sw_control = STEPPER_MOTOR_OFF;
    motor->state = SMOTOR_STATE_USER_STOP;
    motor->stopcallback();
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
