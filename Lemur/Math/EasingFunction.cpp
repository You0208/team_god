#include"EasingFunction.h"

void EasingFunction::EasingValue(float elapsed_time)
{
	if (is_easing)
	{
		if (timer > time_max)
		{
			timer = time_max;
			value = target_value;
			is_easing = false;
			timer = 0.0f;
			return;
		}

		if (easing_type != EasingType::OutBounce)
		{
			if (target_value >= start_value)
			{
				if (value >= target_value) {
					value = target_value;
					is_easing = false;
					timer = 0.0f;
					return;
				}
			}
			else if (target_value >= start_value)
			{
				if (value <= target_value) {
					value = target_value;
					is_easing = false;
					timer = 0.0f;
					return;
				}
			}
		}
		timer += elapsed_time;

		switch (easing_type)
		{
		case EasingType::InSine:
			value = {
			Easing::InSine(timer, time_max, target_value, start_value)
			};
			break;
		case EasingType::OutSine:
			value = {
			Easing::OutSine(timer, time_max, target_value, start_value)
			};
			break;
		case EasingType::OutBounce:
			value = {
			Easing::OutBounce(timer, time_max, target_value, start_value)
			};
			break;
		}
	}
}

void EasingFunction::ContinueEasing(float elapsed_time)
{
	if (!is_easing && is_continue_easing)
	{
		switch (continue_state)
		{
		case 0:// Up
			// 時間切れ
			if (timer > continue_time_max)
			{
				timer = continue_time_max;// 超過した時間を戻す
				value = continue_target_value;// スケールを調整
				timer = 0.0f;// タイマーを初期化

				// ダウンスケールの準備
				continue_start_scale = value;// スタートスケールの設定
				continue_target_value = continue_min;// ターゲットスケールの設定
				continue_state = 1;// ステートを次へ
				break;
			}
			if (value >= continue_target_value)
			{
				value = continue_target_value;// スケールを調整
				timer = 0.0f;// タイマーを初期化

				// ダウンスケールの準備
				continue_start_scale = value;// スタートスケールの設定
				continue_target_value = continue_min;// ターゲットスケールの設定
				continue_state = 1;// ステートを次へ
				break;
			}

			timer += elapsed_time;

			switch (easing_type_up)
			{
			case EasingType::InSine:
				value = {
				Easing::InSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::OutSine:
				value = {
				Easing::OutSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::InOutSine:
				value = {
				Easing::InOutSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::OutBounce:
				value = {
				Easing::InBounce(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			}
			break;

		case 1:// Down
			if (timer > continue_time_max)
			{
				timer = continue_time_max;
				value = continue_target_value;
				timer = 0.0f;

				// アップスケールの準備
				continue_start_scale = value;// スタートスケールの設定
				continue_target_value = continue_max;// ターゲットスケールの設定
				continue_state = 0;// ステートを前へ
				break;
			}

			if (value <= continue_target_value) 
			{
				value = continue_target_value;// スケールを調整
				timer = 0.0f;// タイマーを初期化

				// アップスケールの準備
				continue_start_scale = value;// スタートスケールの設定
				continue_target_value = continue_max;// ターゲットスケールの設定
				continue_state = 0;// ステートを前へ
				break;
			}

			timer += elapsed_time;

			switch (easing_type_down)
			{
			case EasingType::InSine:
				value = {
				Easing::InSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::OutSine:
				value = {
				Easing::OutSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::InOutSine:
				value = {
				Easing::InOutSine(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			case EasingType::OutBounce:
				value = {
				Easing::InBounce(timer, continue_time_max, continue_target_value, continue_start_scale)
				};
				break;
			}
			break;
		}
	}
}

void EasingFunction::TwiceEasing(float elapsed_time)
{
	if (!is_easing && is_twice_easing)
	{

	}

}
