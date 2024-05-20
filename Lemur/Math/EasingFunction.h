#pragma once
#include"Easing.h"

struct EasingFunction
{
public:
    enum EasingType
    {
        InSine,
        OutSine,
        InOutSine,
        OutBounce
    };
    float value = 0.0f;
    bool is_easing = false;

public:
    void EasingValue(float elapsed_time);
    void CallValueEasing(float target_scale_, float start_scale_, EasingType easing_type_, float t = 0.2f)
    {
        timer = 0.0f;
        time_max = t;
        target_value = target_scale_;
        start_value = start_scale_;
        is_easing = true;
        is_continue_easing = false;
        easing_type = easing_type_;
    }

    void ContinueEasing(float elapsed_time);
    void CallValueContinue(float continue_min_, float continue_max_, float start_scale_, int easing_type_up_, int easing_type_down_, float t = 0.5f)
    {
        timer = 0.0f;
        continue_time_max = t;
        continue_max = continue_max_;
        continue_min = continue_min_;
        continue_start_scale = start_scale_;
        start_value = start_scale_;

        if (start_value >= continue_min)
        {
            continue_state = 0;
            continue_target_value = continue_max;
        }
        else if (start_value < continue_max)
        {
            continue_state = 1;
            continue_target_value = continue_min;
        }

        is_continue_easing = true;
        easing_type_up = easing_type_up_;
        easing_type_down = easing_type_down_;
    }


private:
    float timer = 0.0f;
    float time_max = 0.2f;
    float target_value = 0.0f;
    float start_value = 0.0f;

    int loop_count = 0;
    bool is_loop = true;
    bool is_continue_easing = false;
    float continue_time_max = 0.2f;
    float continue_max = 0.0f;
    float continue_min = 0.0f;
    float continue_target_value = 0.0f;
    float continue_start_scale = 0.0f;
    int easing_type = 0;
    int easing_type_up = 0;
    int easing_type_down = 0;
    int continue_state = 0;// 0:Up,1:Down
};