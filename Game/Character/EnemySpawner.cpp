#include "EnemySpawner.h"
#include "EnemyManager.h"
#include "Enemy_ABCD.h"
#include "Game/Stage/StageManager.h"

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

	timer = 0.0f;
	stage_to_shaft = 1.0f;
	shaft_pos =
	{
		{stage_pos.x + stage_width.x + stage_to_shaft},
		{stage_pos.y + stage_width.y + stage_to_shaft}
	};

	InitializeLevel1();
}

void EnemySpawner::EnemySpawn(EnemyScript script)
{
	Enemy* enemy = nullptr;
	switch (script.enemy_type)
	{
	case EnemyType::A:
	{
		enemy = new Enemy_A;
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
		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// リストに追加
		EnemyManager::Instance().Register(enemy);
	}
	break;
	case EnemyType::D:
		enemy = new Enemy_D;
		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// リストに追加
		EnemyManager::Instance().Register(enemy);
		break;
	}
}

void EnemySpawner::Update(float elapsed_time)
{
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
	//level_1.emplace_back(SetEnemy(4.0f, EnemyType::D, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(2.0f, EnemyType::B, Shaft::Vertical, 2.0f));
	//level_1.emplace_back(SetEnemy(3.0f, EnemyType::B, Shaft::Side, 3.0f));
	//level_1.emplace_back(SetEnemy(4.0f, EnemyType::B, Shaft::Vertical, 4.0f));
	//level_1.emplace_back(SetEnemy(5.0f, EnemyType::B, Shaft::Side, 5.0f));
}
