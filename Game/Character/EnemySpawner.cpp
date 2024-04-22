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
	// 軸の位置決定
	stage_to_shaft = 1.0f;
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};
	// タイマーの初期化
	timer = 0.0f;

	// レベル１の初期化
	InitializeLevel1();

	// Imgui用
	 
	//TODO もね
	{
		// Enemy_A
		enemy_A.attack_power = 1;		// 攻撃力
		enemy_A.attack_interval = 3.0f; // 攻撃間隔
		enemy_A.speed_power = -1.0f;    // 移動速度
		enemy_A.radius = 0.5f;			// 半径
		enemy_A.health = 5;				// 体力

		// Enemy_B
		enemy_B.attack_power = 1;		 // 攻撃力
		enemy_B.attack_interval = 3.0f;	 // 攻撃間隔
		enemy_B.speed_power = -1.0f;	 // 移動速度
		enemy_B.radius = 0.5f;			 // 半径
		enemy_B.health = 5;				 // 体力
		mover_timer_max_B = 1.0f;		 // とどまっている時間
		dis_B = 3.0f;					 // 一気に進む距離

		// Enemy_C
		enemy_C.attack_power = 1;			// 攻撃力
		enemy_C.attack_interval = 3.0f;		// 攻撃間隔
		enemy_C.speed_power = -1.0f;		// 移動速度
		enemy_C.radius = 0.5f;				// 半径
		enemy_C.health = 5;					// 体力
		speed_power_Y_D = -1.0f;			// 斜め移動の時のもう一個の速度

		// Enemy_D
		enemy_D.attack_power = 1;			 // 攻撃力
		enemy_D.attack_interval = 3.0f;		 // 攻撃間隔
		enemy_D.speed_power = -1.0f;		 // 移動速度
		enemy_D.radius = 0.5f;				 // 半径
		enemy_D.health = 5;					 // 体力
		dis_D = 1.0f;						 // 移動する距離

		// デバッグ用
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
	// 軸位置更新
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
		// リストに追加
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
		// リストに追加
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
		// リストに追加
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
		// リストに追加
		EnemyManager::Instance().Register(enemy);
		break;
	}
}

void EnemySpawner::DebugImGui()
{
	ImGui::Begin(u8"敵");
	if (ImGui::TreeNode("enemy1"))
	{
		EnemyImGui(enemy_A);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy2"))
	{
		EnemyImGui(enemy_B);
		ImGui::SliderFloat(u8"一気に進むまでの時間", &mover_timer_max_B, 0.0f, 5.0f);
		ImGui::SliderFloat(u8"一気に進む距離", &dis_B, 0.0f, 10.0f);
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
		ImGui::SliderFloat(u8"斜めの時の追加速度", &speed_power_Y_D, 0.0f, 10.0f);
		ImGui::SliderFloat(u8"進む距離", &dis_D, 0.0f, 10.0f);
		ImGui::TreePop();
	}


	ImGui::End();

	ImGui::Begin(u8"敵生成");

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
	ImGui::SliderInt(u8"攻撃力", &status.attack_power, 0, 10);
	ImGui::SliderFloat(u8"攻撃間隔", &status.attack_interval, 0.0f, 10.0f);
	ImGui::SliderFloat(u8"速度", &status.speed_power, -10.0f, 0.0f);
	ImGui::SliderFloat(u8"半径", &status.radius, 0.0f, 1.0f);
}

EnemyScript EnemySpawner::SetEnemy(float second, int enemy_type, int shaft, float pos)
{	
	// 生成位置
	DirectX::XMFLOAT3 spawn_pos = {};

	// 軸を判定
	switch (shaft)
	{
	case Shaft::Side:// 横軸のに生成
		spawn_pos = { shaft_pos.x,0.0f,(stage_pos.y + stage_width.y) - pos };
		break;
	case Shaft::Vertical:// 縦軸の生成
		spawn_pos = { (stage_pos.x + stage_width.x) - pos,0.0f,shaft_pos.y };
		break;
	}

	return { second ,enemy_type,shaft,pos,spawn_pos };
}

void EnemySpawner::InitializeLevel1()
{
	//TODO もね
	// 開始からの秒・エネミーのタイプ・軸（Side:横軸　Vertical:縦軸）・軸に対する長さ
	level_1.emplace_back(SetEnemy(4.0f, EnemyType::D, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(2.0f, EnemyType::B, Shaft::Vertical, 2.0f));
	//level_1.emplace_back(SetEnemy(3.0f, EnemyType::B, Shaft::Side, 3.0f));
	//level_1.emplace_back(SetEnemy(4.0f, EnemyType::B, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(5.0f, EnemyType::B, Shaft::Side, 5.0f));
}

void EnemySpawner::InitializeLevel2()
{

}
