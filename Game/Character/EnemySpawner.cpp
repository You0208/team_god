#include "EnemySpawner.h"
#include "EnemyManager.h"
#include "Enemy_ABCD.h"
#include "Game/Stage/StageManager.h"
#include "Lemur/Input/Input.h"

void EnemySpawner::Initialize()
{
	stage_pos = {
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStagePos().x,
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStagePos().z
	};
	stage_width = {
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().x,
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y
	};
	// ���̈ʒu����
	stage_to_shaft = 1.0f;
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};
	// �^�C�}�[�̏�����
	timer = 0.0f;

	// ���x���P�̏�����
	InitializeLevel1();
	InitializeLevel2();
	InitializeLevel3();
	InitializeLevel4();
	InitializeLevel5();

	// Imgui�p
	 
	//TODO ���ˁ@�G�X�e�[�^�X
	{
		// Enemy_A
		enemy_A.attack_power = 3;		// �U����
		enemy_A.attack_interval = 3.0f; // �U���Ԋu
		enemy_A.speed_power = -0.75f;    // �ړ����x
		enemy_A.radius = 0.5f;			// ���a
		enemy_A.health = 3;				// �̗�

		// Enemy_B
		enemy_B.attack_power = 6;		 // �U����
		enemy_B.attack_interval = 3.0f;	 // �U���Ԋu
		enemy_B.speed_power = -1.0f;	 // �ړ����x
		enemy_B.radius = 0.5f;			 // ���a
		enemy_B.health = 5;				 // �̗�
		mover_timer_max_B = 1.0f;		 // �Ƃǂ܂��Ă��鎞��
		dis_B = 3.0f;					 // ��C�ɐi�ދ���

		// Enemy_C
		enemy_C.attack_power = 1;			// �U����
		enemy_C.attack_interval = 3.0f;		// �U���Ԋu
		enemy_C.speed_power = -1.0f;		// �ړ����x
		enemy_C.radius = 0.5f;				// ���a
		enemy_C.health = 5;					// �̗�
		speed_power_Y_D = -1.0f;			// �΂߈ړ��̎��̂�����̑��x

		// Enemy_D
		enemy_D.attack_power = 1;			 // �U����
		enemy_D.attack_interval = 3.0f;		 // �U���Ԋu
		enemy_D.speed_power = -1.0f;		 // �ړ����x
		enemy_D.radius = 0.5f;				 // ���a
		enemy_D.health = 5;					 // �̗�
		dis_D = 1.0f;						 // �ړ����鋗��


		// Enemy_A_2
		enemy_A_2.attack_power = 1;		// �U����
		enemy_A_2.attack_interval = 3.0f; // �U���Ԋu
		enemy_A_2.speed_power = -0.75f;    // �ړ����x
		enemy_A_2.radius = 0.5f;			// ���a
		enemy_A_2.health = 3;				// �̗�

		// Enemy_B_2
		enemy_B_2.attack_power = 6;		 // �U����
		enemy_B_2.attack_interval = 3.0f;	 // �U���Ԋu
		enemy_B_2.speed_power = -1.0f;	 // �ړ����x
		enemy_B_2.radius = 0.5f;			 // ���a
		enemy_B_2.health = 5;				 // �̗�
		mover_timer_max_B_2 = 1.0f;		 // �Ƃǂ܂��Ă��鎞��
		dis_B_2 = 3.0f;					 // ��C�ɐi�ދ���

		// Enemy_C_2
		enemy_C_2.attack_power = 1;			// �U����
		enemy_C_2.attack_interval = 3.0f;		// �U���Ԋu
		enemy_C_2.speed_power = -1.0f;		// �ړ����x
		enemy_C_2.radius = 0.5f;				// ���a
		enemy_C_2.health = 5;					// �̗�
		speed_power_Y_D_2 = -1.0f;			// �΂߈ړ��̎��̂�����̑��x

		// Enemy_D
		enemy_D_2.attack_power = 1;			 // �U����
		enemy_D_2.attack_interval = 3.0f;		 // �U���Ԋu
		enemy_D_2.speed_power = -1.0f;		 // �ړ����x
		enemy_D_2.radius = 0.5f;				 // ���a
		enemy_D_2.health = 5;					 // �̗�
		dis_D_2 = 1.0f;						 // �ړ����鋗��

		// �f�o�b�O�p
		script_enemy_A = SetEnemy(4.0f, EnemyType::A, Shaft::Side, 4.0f);
		script_enemy_B = SetEnemy(4.0f, EnemyType::B, Shaft::Side, 4.0f);
		script_enemy_C = SetEnemy(4.0f, EnemyType::C, Shaft::Side, 4.0f);
		script_enemy_D = SetEnemy(4.0f, EnemyType::D, Shaft::Side, 4.0f);
		script_enemy_A_2 = SetEnemy(4.0f, EnemyType::A_2, Shaft::Side, 4.0f);
		script_enemy_B_2 = SetEnemy(4.0f, EnemyType::B_2, Shaft::Side, 4.0f);
		script_enemy_C_2 = SetEnemy(4.0f, EnemyType::C_2, Shaft::Side, 4.0f);
		script_enemy_D_2 = SetEnemy(4.0f, EnemyType::D_2, Shaft::Side, 4.0f);
	}
}

void EnemySpawner::Update(float elapsed_time)
{
	DebugImGui();

	stage_pos = {
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStagePos().x,
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStagePos().z
	};
	stage_width = {
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().x,
		StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y
	};
	// ���ʒu�X�V
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};


	if (EnemyManager::Instance().GetEnemyCount() == 0)timer += elapsed_time * timer_speed;
	else timer += elapsed_time;

	switch (StageManager::Instance().GetStageLevel())
	{
	case 0:
	{
		int count = level_1.size();
		if (index < count)
		{
			EnemyScript& script = level_1.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 1:
	{
		int count = level_2.size();
		if (index < count)
		{
			EnemyScript& script = level_2.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 2:
	{
		int count = level_3.size();
		if (index < count)
		{
			EnemyScript& script = level_3.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 3:
	{
		int count = level_4.size();
		if (index < count)
		{
			EnemyScript& script = level_4.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 4:
	{
		int count = level_5.size();
		if (index < count)
		{
			EnemyScript& script = level_5.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	}

}

void EnemySpawner::EnemySpawn(EnemyScript script)
{
	Enemy* enemy = nullptr;
	switch (script.enemy_type)
	{
	case EnemyType::A:
	{
		enemy = new Enemy_A;
		SetBasicEnemyStatus(enemy, enemy_A);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::B:
	{
		enemy = new Enemy_B;
		SetBasicEnemyStatus(enemy, enemy_B);
		enemy->SetMoveTimerMax(mover_timer_max_B);
		enemy->SetDis(dis_B);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::C:
	{
		enemy = new Enemy_C;
		SetBasicEnemyStatus(enemy, enemy_C);
		enemy->SetSpeedPowerY(speed_power_Y_D);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
	}
	break;
	case EnemyType::D:
		enemy = new Enemy_D;
		SetBasicEnemyStatus(enemy, enemy_D);
		enemy->SetDis(dis_D);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	case EnemyType::A_2:
	{
		enemy = new Enemy_A;
		SetBasicEnemyStatus(enemy, enemy_A_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::B_2:
	{
		enemy = new Enemy_B;
		SetBasicEnemyStatus(enemy, enemy_B_2);
		enemy->SetMoveTimerMax(mover_timer_max_B_2);
		enemy->SetDis(dis_B_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::C_2:
	{
		enemy = new Enemy_C;
		SetBasicEnemyStatus(enemy, enemy_C_2);
		enemy->SetSpeedPowerY(speed_power_Y_D_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
	}
	break;
	case EnemyType::D_2:
		enemy = new Enemy_D;
		SetBasicEnemyStatus(enemy, enemy_D_2);
		enemy->SetDis(dis_D_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ���X�g�ɒǉ�
		EnemyManager::Instance().Register(enemy);
		break;
	}
}

void EnemySpawner::DebugImGui()
{
	ImGui::Begin(u8"�G");

	ImGui::DragFloat(u8"�G�o���p�̃^�C�}�[", &timer, 0.0f, 5.0f);
	ImGui::SliderFloat(u8"�G���Ȃ����̔{��", &timer_speed, 0.0f, 5.0f);


	if (ImGui::TreeNode("enemy1"))
	{
		EnemyImGui(enemy_A);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy2"))
	{
		EnemyImGui(enemy_B);
		ImGui::SliderFloat(u8"��C�ɐi�ނ܂ł̎���", &mover_timer_max_B, 0.0f, 5.0f);
		ImGui::SliderFloat(u8"��C�ɐi�ދ���", &dis_B, 0.0f, 10.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy3"))
	{
		EnemyImGui(enemy_C);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy4"))
	{
		EnemyImGui(enemy_D);
		ImGui::SliderFloat(u8"�΂߂̎��̒ǉ����x", &speed_power_Y_D, 0.0f, 10.0f);
		ImGui::SliderFloat(u8"�i�ދ���", &dis_D, 0.0f, 10.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy1_2"))
	{
		EnemyImGui(enemy_A_2);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy2_2"))
	{
		EnemyImGui(enemy_B_2);
		ImGui::SliderFloat(u8"��C�ɐi�ނ܂ł̎���_2", &mover_timer_max_B, 0.0f, 5.0f);
		ImGui::SliderFloat(u8"��C�ɐi�ދ���_2", &dis_B, 0.0f, 10.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy3_2"))
	{
		EnemyImGui(enemy_C_2);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy4_2"))
	{
		EnemyImGui(enemy_D_2);
		ImGui::SliderFloat(u8"�΂߂̎��̒ǉ����x_2", &speed_power_Y_D, 0.0f, 10.0f);
		ImGui::SliderFloat(u8"�i�ދ���_2", &dis_D, 0.0f, 10.0f);
		ImGui::TreePop();
	}


	ImGui::End();

	ImGui::Begin(u8"�G����");

	if (ImGui::Button("enemy1"))
	{
		EnemySpawn(script_enemy_A);
	}
	if (ImGui::Button("enemy2"))
	{
		EnemySpawn(script_enemy_B);
	}

	if (ImGui::Button("enemy3"))
	{
		EnemySpawn(script_enemy_C);
	}
	if (ImGui::Button("enemy4"))
	{
		EnemySpawn(script_enemy_D);
	}
	if (ImGui::Button("enemy1_2"))
	{
		EnemySpawn(script_enemy_A_2);
	}
	if (ImGui::Button("enemy2_2"))
	{
		EnemySpawn(script_enemy_B_2);
	}

	if (ImGui::Button("enemy3_2"))
	{
		EnemySpawn(script_enemy_C_2);
	}
	if (ImGui::Button("enemy4_2"))
	{
		EnemySpawn(script_enemy_D_2);
	}
	ImGui::End();
}

void EnemySpawner::SetBasicEnemyStatus(Enemy* enemy, EnemyStatus status)
{
	enemy->SetAttackPower(status.attack_power);
	enemy->SetHealth(status.health);
	enemy->SetAttackInterval(status.attack_interval);
	enemy->SetRadius(status.radius);
	enemy->SetSpeedPower(status.speed_power);
}

void EnemySpawner::EnemyImGui(EnemyStatus& status)
{
	ImGui::SliderInt("HP", &status.health, 0, 10);
	ImGui::SliderInt(u8"�U����", &status.attack_power, 0, 10);
	ImGui::SliderFloat(u8"�U���Ԋu", &status.attack_interval, 0.0f, 10.0f);
	ImGui::SliderFloat(u8"���x", &status.speed_power, -10.0f, 0.0f);
	ImGui::SliderFloat(u8"���a", &status.radius, 0.0f, 1.0f);
}

EnemyScript EnemySpawner::SetEnemy(float second, int enemy_type, int shaft, float pos)
{	
	// �����ʒu
	DirectX::XMFLOAT3 spawn_pos = {};

	// ���𔻒�
	switch (shaft)
	{
	case Shaft::Side:// �����̂ɐ���
		spawn_pos = { shaft_pos.x,0.0f,(stage_pos.y + stage_width.y) - pos };
		break;
	case Shaft::Vertical:// �c���̐���
		spawn_pos = { (stage_pos.x + stage_width.x) - pos,0.0f,shaft_pos.y };
		break;
	}

	return { second ,enemy_type,shaft,pos,spawn_pos };
}

void EnemySpawner::InitializeLevel1()
{
	//TODO ���ˁ@�G�o��1�]1
	// �J�n����̕b�E�G�l�~�[�̃^�C�v�E���iSide:�c���@Vertical:�����j�E���ɑ΂��钷��
	level_1.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 1.0f));
	level_1.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 17.5f));//A1

	level_1.emplace_back(SetEnemy(15.0f, EnemyType::A, Shaft::Vertical, 1.0f));
	level_1.emplace_back(SetEnemy(15.0f, EnemyType::A, Shaft::Vertical, 14.0f));//A2

	level_1.emplace_back(SetEnemy(24.0f, EnemyType::A, Shaft::Side, 6.0f));
	level_1.emplace_back(SetEnemy(24.0f, EnemyType::A, Shaft::Vertical, 17.5f));//A3

	level_1.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Side, 15.0f));
	level_1.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Side, 17.0f));
	level_1.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Vertical, 17.5f));//A4

	level_1.emplace_back(SetEnemy(40.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_1.emplace_back(SetEnemy(40.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A5

	level_1.emplace_back(SetEnemy(46.0f, EnemyType::A, Shaft::Side, 1.0f));
	level_1.emplace_back(SetEnemy(46.0f, EnemyType::A, Shaft::Side, 17.5f));//A1

	level_1.emplace_back(SetEnemy(55.0f, EnemyType::A, Shaft::Vertical, 1.0f));
	level_1.emplace_back(SetEnemy(55.0f, EnemyType::A, Shaft::Vertical, 14.0f));//A2

	level_1.emplace_back(SetEnemy(61.0f, EnemyType::A, Shaft::Side, 6.0f));
	level_1.emplace_back(SetEnemy(61.0f, EnemyType::A, Shaft::Vertical, 17.5f));//A3

	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Side, 15.0f));
	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Side, 17.0f));
	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Vertical, 17.5f));//A4

	//level_1.emplace_back(SetEnemy(73.0f, EnemyType::A, Shaft::Side, 9.0f));
	//level_1.emplace_back(SetEnemy(73.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A5


}

void EnemySpawner::InitializeLevel2()
{
	// level_2������
	
	level_2.emplace_back(SetEnemy(3.0f, EnemyType::A, Shaft::Side, 16.0f));
	level_2.emplace_back(SetEnemy(3.0f, EnemyType::A, Shaft::Vertical, 15.0f));//A1

	level_2.emplace_back(SetEnemy(10.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_2.emplace_back(SetEnemy(10.0f, EnemyType::A, Shaft::Vertical, 11.0f));//A2

	level_2.emplace_back(SetEnemy(16.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_2.emplace_back(SetEnemy(16.0f, EnemyType::A, Shaft::Side, 4.0f));
	level_2.emplace_back(SetEnemy(16.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A3

	level_2.emplace_back(SetEnemy(25.0f, EnemyType::A, Shaft::Vertical, 6.0f));
	level_2.emplace_back(SetEnemy(25.0f, EnemyType::A, Shaft::Vertical, 12.0f));
	level_2.emplace_back(SetEnemy(25.0f, EnemyType::A, Shaft::Side, 16.0f));//A4

	level_2.emplace_back(SetEnemy(34.0f, EnemyType::A, Shaft::Side, 17.0f));
	level_2.emplace_back(SetEnemy(34.0f, EnemyType::B, Shaft::Vertical, 17.0f));//B5

	level_2.emplace_back(SetEnemy(34.0f, EnemyType::B, Shaft::Side, 1.0f));
	level_2.emplace_back(SetEnemy(34.0f, EnemyType::B, Shaft::Vertical, 1.0f));//B6
}

void EnemySpawner::InitializeLevel3()
{
	// level_3������
	//level_3.emplace_back(SetEnemy(15.0f, EnemyType::A_2, Shaft::Side, 1.0f));
	level_3.emplace_back(SetEnemy(15.0f, EnemyType::B, Shaft::Side, 1.0f));
	level_3.emplace_back(SetEnemy(15.0f, EnemyType::C, Shaft::Side, 1.0f));
	level_3.emplace_back(SetEnemy(15.0f, EnemyType::D, Shaft::Side, 1.0f));
}

void EnemySpawner::InitializeLevel4()
{
	// level_4������
	level_4.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 1.0f));
}

void EnemySpawner::InitializeLevel5()
{
	// level_5������
	level_5.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 1.0f));
}
