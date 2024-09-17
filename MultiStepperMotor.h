#ifndef __MULTISTEPPERMOTOR_H_
#define __MULTISTEPPERMOTOR_H_

#include <stdint.h>
#include <stddef.h>

#define STEPPER_MOTOR_GROUPS_NUM 5 // 最多支持5个电机组

#define STEPPER_MOTOR_ON 1
#define STEPPER_MOTOR_OFF 0

#define SMOTOR_STATE_IDLE 0       // 空闲
#define SMOTOR_STATE_MOVING 1     // 运动中
#define SMOTOR_STATE_STEPS_STOP 2 // 步数停止
#define SMOTOR_STATE_USER_STOP 3  // 用户停止

#define SMOTOR_DIR_FORWARD 0  // 向前运动
#define SMOTOR_DIR_BACKWARD 1 // 向后运动

typedef struct STEPPER_MOTOR
{
    uint8_t sw_control;                   // 控制开关
    void (*clk_toggle_pin)(void);         // CLK引脚电平翻转函数接口
    void (*dir_control)(uint8_t level_t); // DIR引脚电平控制函数接口
    uint8_t forward_pin_level;            // 正转引脚电平
    uint32_t step_counter;                // 步数
    uint32_t target_steps;                // 目标步数
    uint8_t state;                        // 状态
    void (*stopcallback)(void);           // 停止回调函数
    struct STEPPER_MOTOR *next;
} STEPPER_MOTOR;

void stepper_motor_init(struct STEPPER_MOTOR *motor, void (*clk_toggle_t)(uint8_t), void (*dir_control_t)(uint8_t), uint8_t forwrd_pin_level_t, void (*callback_t)(void));
int stepper_motor_register(struct STEPPER_MOTOR *motor, uint8_t group_id);
int8_t stepper_motor_run(struct STEPPER_MOTOR *motor, uint8_t dir_t, uint32_t steps);
int8_t stepper_motor_stop(struct STEPPER_MOTOR *motor);
void stepper_motor_callback_ticks(uint8_t group_id);
void stepper_motor_group_it_ticks(uint8_t group_id);

#endif
