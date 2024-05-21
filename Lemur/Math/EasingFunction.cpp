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
			// ���Ԑ؂�
			if (timer > continue_time_max)
			{
				timer = continue_time_max;// ���߂������Ԃ�߂�
				value = continue_target_value;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �_�E���X�P�[���̏���
				continue_start_scale = value;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_value = continue_min;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 1;// �X�e�[�g������
				break;
			}
			if (value >= continue_target_value)
			{
				value = continue_target_value;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �_�E���X�P�[���̏���
				continue_start_scale = value;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_value = continue_min;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 1;// �X�e�[�g������
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

				// �A�b�v�X�P�[���̏���
				continue_start_scale = value;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_value = continue_max;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 0;// �X�e�[�g��O��
				break;
			}

			if (value <= continue_target_value) 
			{
				value = continue_target_value;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �A�b�v�X�P�[���̏���
				continue_start_scale = value;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_value = continue_max;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 0;// �X�e�[�g��O��
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
