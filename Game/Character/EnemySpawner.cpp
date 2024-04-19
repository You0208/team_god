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
	// é≤ÇÃà íuåàíË
	stage_to_shaft = 1.0f;
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};
	// É^ÉCÉ}Å[ÇÃèâä˙âª
	timer = 0.0f;

	// ÉåÉxÉãÇPÇÃèâä˙âª
	InitializeLevel1();

	// Imguióp
	 
	//TODO Ç‡ÇÀ
	{
		// Enemy_A
		enemy_A.attack_power = 1;		// çUåÇóÕ
		enemy_A.attack_interval = 3.0f; // çUåÇä‘äu
		enemy_A.speed_power = -1.0f;    // à⁄ìÆë¨ìx
		enemy_A.radius = 0.5f;			// îºåa
		enemy_A.health = 5;				// ëÃóÕ

		// Enemy_B
		enemy_B.attack_power = 1;		 // çUåÇóÕ
		enemy_B.attack_interval = 3.0f;	 // çUåÇä‘äu
		enemy_B.speed_power = -1.0f;	 // à⁄ìÆë¨ìx
		enemy_B.radius = 0.5f;			 // îºåa
		enemy_B.health = 5;				 // ëÃóÕ
		mover_timer_max_B = 1.0f;		 // Ç∆Ç«Ç‹Ç¡ÇƒÇ¢ÇÈéûä‘
		dis_B = 3.0f;					 // àÍãCÇ…êiÇﬁãóó£

		// Enemy_C
		enemy_C.attack_power = 1;			// çUåÇóÕ
		enemy_C.attack_interval = 3.0f;		// çUåÇä‘äu
		enemy_C.speed_power = -1.0f;		// à⁄ìÆë¨ìx
		enemy_C.radius = 0.5f;				// îºåa
		enemy_C.health = 5;					// ëÃóÕ
		speed_power_Y_D = -1.0f;			// éŒÇﬂà⁄ìÆÇÃéûÇÃÇ‡Ç§àÍå¬ÇÃë¨ìx

		// Enemy_D
		enemy_D.attack_power = 1;			 // çUåÇóÕ
		enemy_D.attack_interval = 3.0f;		 // çUåÇä‘äu
		enemy_D.speed_power = -1.0f;		 // à⁄ìÆë¨ìx
		enemy_D.radius = 0.5f;				 // îºåa
		enemy_D.health = 5;					 // ëÃóÕ
		dis_D = 1.0f;						 // à⁄ìÆÇ∑ÇÈãóó£

		// ÉfÉoÉbÉOóp
		script_enemy_A = SetEnemy(4.0f, EnemyType::A, Shaft::Side, 4.0f);
		script_enemy_B = SetEnemy(4.0f, EnemyType::B, Shaft::Side, 4.0f);
		script_enemy_C = SetEnemy(4.0f, EnemyType::C, Shaft::Side, 4.0f);
		script_enemy_D = SetEnemy(4.0f, EnemyType::D, Shaft::Side, 4.0f);
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
	// é≤à íuçXêV
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};

	timer += elapsed_time;
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
		// ÉäÉXÉgÇ…í«â¡
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
		// ÉäÉXÉgÇ…í«â¡
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
		// ÉäÉXÉgÇ…í«â¡
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
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
}

void EnemySpawner::DebugImGui()
{
	ImGui::Begin(u8"ìG");
	if (ImGui::TreeNode("enemy1"))
	{
		EnemyImGui(enemy_A);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy2"))
	{
		EnemyImGui(enemy_B);
		ImGui::SliderFloat(u8"àÍãCÇ…êiÇﬁÇ‹Ç≈ÇÃéûä‘", &mover_timer_max_B, 0.0f, 5.0f);
		ImGui::SliderFloat(u8"àÍãCÇ…êiÇﬁãóó£", &dis_B, 0.0f, 10.0f);
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
		ImGui::SliderFloat(u8"éŒÇﬂÇÃéûÇÃí«â¡ë¨ìx", &speed_power_Y_D, 0.0f, 10.0f);
		ImGui::SliderFloat(u8"êiÇﬁãóó£", &dis_D, 0.0f, 10.0f);
		ImGui::TreePop();
	}


	ImGui::End();

	ImGui::Begin(u8"ìGê∂ê¨");

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
	ImGui::SliderInt(u8"çUåÇóÕ", &status.attack_power, 0, 10);
	ImGui::SliderFloat(u8"çUåÇä‘äu", &status.attack_interval, 0.0f, 10.0f);
	ImGui::SliderFloat(u8"ë¨ìx", &status.speed_power, -10.0f, 0.0f);
	ImGui::SliderFloat(u8"îºåa", &status.radius, 0.0f, 1.0f);
}

EnemyScript EnemySpawner::SetEnemy(float second, int enemy_type, int shaft, float pos)
{	
	// ê∂ê¨à íu
	DirectX::XMFLOAT3 spawn_pos = {};

	// é≤ÇîªíË
	switch (shaft)
	{
	case Shaft::Side:// â°é≤ÇÃÇ…ê∂ê¨
		spawn_pos = { shaft_pos.x,0.0f,(stage_pos.y + stage_width.y) - pos };
		break;
	case Shaft::Vertical:// ècé≤ÇÃê∂ê¨
		spawn_pos = { (stage_pos.x + stage_width.x) - pos,0.0f,shaft_pos.y };
		break;
	}

	return { second ,enemy_type,shaft,pos,spawn_pos };
}

void EnemySpawner::InitializeLevel1()
{
	//TODO Ç‡ÇÀ
	//level_1.emplace_back(SetEnemy(4.0f, EnemyType::D, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(2.0f, EnemyType::B, Shaft::Vertical, 2.0f));
	//level_1.emplace_back(SetEnemy(3.0f, EnemyType::B, Shaft::Side, 3.0f));
	//level_1.emplace_back(SetEnemy(4.0f, EnemyType::B, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(5.0f, EnemyType::B, Shaft::Side, 5.0f));
}
