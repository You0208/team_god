#include "EnemySpawner.h"
#include "EnemyManager.h"
#include "Enemy_ABCD.h"
#include "UniqueEnemy.h"
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
	index = 0;

	// ÉåÉxÉãÇPÇÃèâä˙âª
	InitializeLevel1();
	InitializeLevel2();
	InitializeLevel3();
	InitializeLevel4();
	InitializeLevel5();
	InitializeLevel6();
	InitializeLevel7();
	InitializeLevel8();
	InitializeLevel9();

	// Imguióp
	 
	//TODO Ç‡ÇÀÅ@ìGÉXÉeÅ[É^ÉX
	{
		// Enemy_A
		enemy_A.attack_power = 2;		// çUåÇóÕ
		enemy_A.attack_interval = 3.0f; // çUåÇä‘äu
		enemy_A.speed_power = -1.73f;    // à⁄ìÆë¨ìx
		enemy_A.radius = 0.35f;			// îºåa
		enemy_A.health = 3;				// ëÃóÕ
		enemy_A.attack_effect_size = 0.3f;			
		enemy_A.death_effect_size = 0.4f;
		enemy_A.hit_effect_size = 0.3f;

		// Enemy_B
		enemy_B.attack_power = 2;		 // çUåÇóÕ
		enemy_B.attack_interval = 3.0f;	 // çUåÇä‘äu
		enemy_B.speed_power = -1.73f;	 // à⁄ìÆë¨ìx
		enemy_B.radius = 0.35f;			 // îºåa
		enemy_B.health = 4;				 // ëÃóÕ
		enemy_B.attack_effect_size = 0.3f;
		enemy_B.death_effect_size = 0.4f;
		enemy_B.hit_effect_size = 0.3f;
		mover_timer_max_B = 1.3f;		 // Ç∆Ç«Ç‹Ç¡ÇƒÇ¢ÇÈéûä‘
		dis_B = 3.0f;					 // àÍãCÇ…êiÇﬁãóó£


		// Enemy_C
		enemy_C.attack_power = 1;			// çUåÇóÕ
		enemy_C.attack_interval = 2.8f;		// çUåÇä‘äu
		enemy_C.speed_power = -1.5f;	    // à⁄ìÆë¨ìx
		enemy_C.radius = 0.35f;				// îºåa
		enemy_C.health = 3;					// ëÃóÕ
		enemy_C.attack_effect_size = 0.3f;
		enemy_C.death_effect_size = 0.4f;
		enemy_C.hit_effect_size = 0.3f;
		speed_power_Y_D = -0.0f;			// éŒÇﬂà⁄ìÆÇÃéûÇÃÇ‡Ç§àÍå¬ÇÃë¨ìx

		// Enemy_D
		enemy_D.attack_power = 3;			 // çUåÇóÕ
		enemy_D.attack_interval = 3.0f;		 // çUåÇä‘äu
		enemy_D.speed_power = -1.0f;		 // à⁄ìÆë¨ìx
		enemy_D.radius = 0.35f;				 // îºåa
		enemy_D.health = 2;					 // ëÃóÕ
		enemy_D.attack_effect_size = 0.3f;
		enemy_D.death_effect_size = 0.4f;
		enemy_D.hit_effect_size = 0.3f;
		//enemy_D.hit_effect_size = 0.2f;
		dis_D = 1.0f;						 // à⁄ìÆÇ∑ÇÈãóó£

		// Enemy_A_2
		enemy_A_2.attack_power = 3;		// çUåÇóÕ
		enemy_A_2.attack_interval = 3.0f; // çUåÇä‘äu
		enemy_A_2.speed_power = -1.76f;    // à⁄ìÆë¨ìx
		enemy_A_2.radius = 0.35f;			// îºåa
		enemy_A_2.health = 5;				// ëÃóÕ
		enemy_A_2.attack_effect_size = 0.3f;
		enemy_A_2.death_effect_size = 0.4f;
		enemy_A_2.hit_effect_size = 0.3f;

		// Enemy_B_2
		enemy_B_2.attack_power = 3;		 // çUåÇóÕ
		enemy_B_2.attack_interval = 3.0f;	 // çUåÇä‘äu
		enemy_B_2.speed_power = -1.73f;	 // à⁄ìÆë¨ìx
		enemy_B_2.radius = 0.35f;			 // îºåa
		enemy_B_2.health = 6;				 // ëÃóÕ
		enemy_B_2.attack_effect_size = 0.3f;
		enemy_B_2.death_effect_size = 0.4f;
		enemy_B_2.hit_effect_size = 0.3f;
		mover_timer_max_B_2 = 1.0f;		 // Ç∆Ç«Ç‹Ç¡ÇƒÇ¢ÇÈéûä‘
		dis_B_2 = 3.0f;					 // àÍãCÇ…êiÇﬁãóó£

		// Enemy_C_2
		enemy_C_2.attack_power = 2;			// çUåÇóÕ
		enemy_C_2.attack_interval = 3.0f;		// çUåÇä‘äu
		enemy_C_2.speed_power = -0.82f;		// à⁄ìÆë¨ìx
		enemy_C_2.radius = 0.35f;				// îºåa
		enemy_C_2.health = 4;					// ëÃóÕ
		enemy_C_2.attack_effect_size = 0.3f;
		enemy_C_2.death_effect_size = 0.4f;
		enemy_C_2.hit_effect_size = 0.3f;
		speed_power_Y_D_2 = -0.82f;			// éŒÇﬂà⁄ìÆÇÃéûÇÃÇ‡Ç§àÍå¬ÇÃë¨ìx

		// Enemy_D
		enemy_D_2.attack_power = 5;			 // çUåÇóÕ
		enemy_D_2.attack_interval = 3.0f;		 // çUåÇä‘äu
		enemy_D_2.speed_power = -0.79f;		 // à⁄ìÆë¨ìx
		enemy_D_2.radius = 0.35f;				 // îºåa
		enemy_D_2.health = 5;					 // ëÃóÕ
		enemy_D_2.attack_effect_size = 0.3f;
		enemy_D_2.death_effect_size = 0.4f;
		enemy_D_2.hit_effect_size = 0.3f;
		dis_D_2 = 1.0f;						 // à⁄ìÆÇ∑ÇÈãóó£

		// Summon_Enemy
		summon_enemy.attack_power = 0;			 // çUåÇóÕ
		summon_enemy.attack_interval = 8.0f;		 // çUåÇä‘äu
		summon_enemy.speed_power = 0.0f;		 // à⁄ìÆë¨ìx
		summon_enemy.radius = 0.3f;				 // îºåa
		summon_enemy.health = 2;					 // ëÃóÕ
		summon_enemy.attack_effect_size = 0.3f;
		summon_enemy.death_effect_size = 0.4f;
		summon_enemy.hit_effect_size = 0.3f;

		// boss_Enemy
		boss_enemy.attack_power = 10;		// çUåÇóÕ
		boss_enemy.attack_interval = 5.0f; // çUåÇä‘äu
		boss_enemy.speed_power = -0.55f;    // à⁄ìÆë¨ìx
		boss_enemy.radius = 0.5f;			// îºåa
		boss_enemy.health = 30;				// ëÃóÕ
		boss_enemy.attack_effect_size = 0.3f;
		boss_enemy.death_effect_size = 0.4f;
		boss_enemy.hit_effect_size = 0.3f;

		// Boss_Enemy
		nuisance_enemy.attack_power = 0;		// çUåÇóÕ
		nuisance_enemy.attack_interval = 0.0f; // çUåÇä‘äu
		nuisance_enemy.speed_power = 0.0f;    // à⁄ìÆë¨ìx
		nuisance_enemy.radius = 0.5f;			// îºåa
		nuisance_enemy.health = 3;				// ëÃóÕ
		nuisance_enemy.attack_effect_size = 0.3f;
		nuisance_enemy.death_effect_size = 0.4f;
		nuisance_enemy.hit_effect_size = 0.3f;

		// ÉfÉoÉbÉOóp
		script_enemy_A = SetEnemy(4.0f, EnemyType::A, Shaft::Side, 4.0f);
		script_enemy_B = SetEnemy(4.0f, EnemyType::B, Shaft::Side, 4.0f);
		script_enemy_C = SetEnemy(4.0f, EnemyType::C, Shaft::Side, 4.0f);
		script_enemy_D = SetEnemy(4.0f, EnemyType::D, Shaft::Side, 4.0f);
		script_enemy_A_2 = SetEnemy(4.0f, EnemyType::A_2, Shaft::Side, 4.0f);
		script_enemy_B_2 = SetEnemy(4.0f, EnemyType::B_2, Shaft::Side, 4.0f);
		script_enemy_C_2 = SetEnemy(4.0f, EnemyType::C_2, Shaft::Side, 4.0f);
		script_enemy_D_2 = SetEnemy(4.0f, EnemyType::D_2, Shaft::Vertical, 4.0f);
		script_summon_enemy = SetEnemy(4.0f, EnemyType::Summon, { 4.0f,4.0f });
		script_boss_enemy = SetEnemy(4.0f, EnemyType::Boss, Shaft::Side, 4.0f);
		script_nuisance_enemy = SetEnemy(4.0f, EnemyType::Nuisance, { 4.0f,4.0f });
	}
}

void EnemySpawner::Finalize()
{
	level_1.clear();
	level_2.clear();
	level_3.clear();
	level_4.clear();
	level_5.clear();
	level_6.clear();
	level_7.clear();
	level_8.clear();
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


	if (EnemyManager::Instance().GetEnemyCount() == 0 && !EnemyManager::Instance().GetTimeUp())timer += elapsed_time * timer_speed;
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
	case 5:
	{
		int count = level_6.size();
		if (index < count)
		{
			EnemyScript& script = level_6.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 6:
	{
		int count = level_7.size();
		if (index < count)
		{
			EnemyScript& script = level_7.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 7:
	{
		int count = level_8.size();
		if (index < count)
		{
			EnemyScript& script = level_8.at(index);
			if (timer >= script.second)
			{
				EnemySpawn(script);
				index++;
			}
		}
		break;
	}
	case 8:
	{
		int count = level_9.size();
		if (index < count)
		{
			EnemyScript& script = level_9.at(index);
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
		enemy = new Enemy_A(false);
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
		enemy = new Enemy_B(false);
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
		enemy = new Enemy_C(false);
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
		enemy = new Enemy_D(false);
		SetBasicEnemyStatus(enemy, enemy_D);
		enemy->SetSpeedPowerY(speed_power_Y_D);
		enemy->SetDis(dis_D);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	case EnemyType::A_2:
	{
		enemy = new Enemy_A(true);
		SetBasicEnemyStatus(enemy, enemy_A_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::B_2:
	{
		enemy = new Enemy_B(true);
		SetBasicEnemyStatus(enemy, enemy_B_2);
		enemy->SetMoveTimerMax(mover_timer_max_B_2);
		enemy->SetDis(dis_B_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::C_2:
	{
		enemy = new Enemy_C(true);
		SetBasicEnemyStatus(enemy, enemy_C_2);
		enemy->SetSpeedPowerY(speed_power_Y_D_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
	}
	break;
	case EnemyType::D_2:
	{
		enemy = new Enemy_D(true);
		SetBasicEnemyStatus(enemy, enemy_D_2);
		enemy->SetDis(dis_D_2);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::Summon:
	{
		enemy = new SummonEnemy(false);
		SetBasicEnemyStatus(enemy, summon_enemy);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();

		// TODOÇ‡ÇÀÅ@ÉXÉeÅ[ÉWÇÃÉåÉxÉãÇ…ÇÊÇ¡ÇƒçUåÇä‘äuïœçX
		switch (StageManager::Instance().GetStageLevel())
		{
		case 0:
			enemy->SetAttackInterval(8.0f);
			break;
		case 1:
			enemy->SetAttackInterval(8.0f);
			break;
		case 2:
			enemy->SetAttackInterval(8.0f);
			break;
		case 3:
			enemy->SetAttackInterval(8.0f);
			break;
		case 4:
			enemy->SetAttackInterval(8.0f);
			break;
		case 5:
			enemy->SetAttackInterval(8.0f);
			break;
		case 6:
			enemy->SetAttackInterval(8.0f);
			break;
		case 7:
			enemy->SetAttackInterval(8.0f);
			break;
		case 8:
			enemy->SetAttackInterval(8.0f);
			break;
		}

		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);

		break;
	}
	case EnemyType::Boss:
	{
		enemy = new BossEnemy(false);
		SetBasicEnemyStatus(enemy, boss_enemy);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
	case EnemyType::Nuisance:
	{
		enemy = new NuisanceEnemy(false);
		SetBasicEnemyStatus(enemy, nuisance_enemy);

		enemy->SetShaft(script.shaft);
		enemy->SetPosition(script.pos);
		enemy->UpdateTransform();
		// ÉäÉXÉgÇ…í«â¡
		EnemyManager::Instance().Register(enemy);
		break;
	}
	};
}

void EnemySpawner::DebugImGui()
{
	ImGui::Begin(u8"ìG");

	ImGui::DragFloat(u8"ìGèoåªópÇÃÉ^ÉCÉ}Å[", &timer, 0.0f, 5.0f);
	ImGui::SliderFloat(u8"ìGÇ¢Ç»Ç¢éûÇÃî{ë¨", &timer_speed, 0.0f, 5.0f);


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
	if (ImGui::TreeNode("enemy1_2"))
	{
		EnemyImGui(enemy_A_2);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("enemy2_2"))
	{
		EnemyImGui(enemy_B_2);
		ImGui::SliderFloat(u8"àÍãCÇ…êiÇﬁÇ‹Ç≈ÇÃéûä‘_2", &mover_timer_max_B, 0.0f, 5.0f);
		ImGui::SliderFloat(u8"àÍãCÇ…êiÇﬁãóó£_2", &dis_B, 0.0f, 10.0f);
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
		ImGui::SliderFloat(u8"éŒÇﬂÇÃéûÇÃí«â¡ë¨ìx_2", &speed_power_Y_D, 0.0f, 10.0f);
		ImGui::SliderFloat(u8"êiÇﬁãóó£_2", &dis_D, 0.0f, 10.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("summon"))
	{
		EnemyImGui(summon_enemy);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("boss"))
	{
		EnemyImGui(boss_enemy);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("nuisance"))
	{
		EnemyImGui(nuisance_enemy);
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
	if (ImGui::Button("summon"))
	{
		EnemySpawn(script_summon_enemy);
	}
	if (ImGui::Button("boss"))
	{
		EnemySpawn(script_boss_enemy);
	}
	if (ImGui::Button("nuisance"))
	{
		EnemySpawn(script_nuisance_enemy);
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
	enemy->SetAttackEffectSize(status.attack_effect_size);
	enemy->SetDeathEffectSize(status.death_effect_size);
	enemy->SetHitEffectSize(status.hit_effect_size);
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

EnemyScript EnemySpawner::SetEnemy(float second, int enemy_type, DirectX::XMFLOAT2 pos)
{
	// ê∂ê¨à íu
	DirectX::XMFLOAT3 spawn_pos = {};
	spawn_pos = { pos.x,0.0f,pos.y };

	return { second ,enemy_type,0,0.0f,spawn_pos };
}

//TODOÅ@Ç‡ÇÀìGÉXÉ|Å[Éì
void EnemySpawner::InitializeLevel1()
{
	// äJénÇ©ÇÁÇÃïbÅEÉGÉlÉ~Å[ÇÃÉ^ÉCÉvÅEé≤ÅiSide:ècé≤Å@Vertical:â°é≤ÅjÅEé≤Ç…ëŒÇ∑ÇÈí∑Ç≥

	//1-1 17ëÃ
	level_1.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 1.0f));
	level_1.emplace_back(SetEnemy(2.0f, EnemyType::A, Shaft::Side, 17.5f));//A1

	level_1.emplace_back(SetEnemy(30.0f, EnemyType::A, Shaft::Vertical, 1.0f));
	level_1.emplace_back(SetEnemy(30.0f, EnemyType::A, Shaft::Vertical, 14.0f));//A2 28

	level_1.emplace_back(SetEnemy(57.0f, EnemyType::A, Shaft::Side, 6.0f));
	level_1.emplace_back(SetEnemy(57.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A3 27

	level_1.emplace_back(SetEnemy(68.0f, EnemyType::A, Shaft::Side, 15.0f));
	level_1.emplace_back(SetEnemy(68.0f, EnemyType::A, Shaft::Side, 13.0f)); //11
	level_1.emplace_back(SetEnemy(72.0f, EnemyType::A, Shaft::Vertical, 16.5f));//A4 15

	level_1.emplace_back(SetEnemy(90.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_1.emplace_back(SetEnemy(90.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A5 18

	level_1.emplace_back(SetEnemy(105.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_1.emplace_back(SetEnemy(105.0f, EnemyType::A, Shaft::Vertical, 5.0f));//6 15

	level_1.emplace_back(SetEnemy(128.0f, EnemyType::A, Shaft::Side, 10.0f));
	level_1.emplace_back(SetEnemy(130.0f, EnemyType::A, Shaft::Vertical, 4.0f));
	level_1.emplace_back(SetEnemy(130.0f, EnemyType::A, Shaft::Vertical, 11.0f));//7 25

	level_1.emplace_back(SetEnemy(150.0f, EnemyType::A, Shaft::Side, 6.0f));
	level_1.emplace_back(SetEnemy(150.0f, EnemyType::A, Shaft::Vertical, 17.5f));//20

	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Side, 15.0f));
	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Side, 17.0f));
	//level_1.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Vertical, 17.5f));//A4

	//level_1.emplace_back(SetEnemy(73.0f, EnemyType::A, Shaft::Side, 9.0f));
	//level_1.emplace_back(SetEnemy(73.0f, EnemyType::A, Shaft::Vertical, 9.0f));//A5


}

void EnemySpawner::InitializeLevel2()
{
	// level_2Ç©íçà” âºäÆ

	//level_1.emplace_back(SetEnemy(1.0f, EnemyType::Summon, { 2,2 }));//11

	level_2.emplace_back(SetEnemy(1.0f, EnemyType::Summon, { 4.5,4.5 }));
	level_2.emplace_back(SetEnemy(1.0f, EnemyType::Summon, { -4.5,4.5 }));

	level_2.emplace_back(SetEnemy(20.0f, EnemyType::A, Shaft::Side, 16.0f));
	level_2.emplace_back(SetEnemy(20.0f, EnemyType::A, Shaft::Vertical, 6.0f));
	level_2.emplace_back(SetEnemy(20.0f, EnemyType::A, Shaft::Side, 4.0f));//1

	level_2.emplace_back(SetEnemy(33.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_2.emplace_back(SetEnemy(33.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_2.emplace_back(SetEnemy(33.0f, EnemyType::A, Shaft::Vertical, 11.0f));//2

	level_2.emplace_back(SetEnemy(40.0f, EnemyType::Summon, { 3,0 }));
	level_2.emplace_back(SetEnemy(40.0f, EnemyType::Summon, { -3,0 }));

	level_2.emplace_back(SetEnemy(53.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_2.emplace_back(SetEnemy(53.0f, EnemyType::A, Shaft::Side, 5.0f));
	level_2.emplace_back(SetEnemy(53.0f, EnemyType::A, Shaft::Side, 3.0f));//3

	//íÜâõ
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 1,0 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 0.5,0.5 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 0,1 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { -0.5,0.5 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { -1,0 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 0.5,-0.5 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 0,-1 }));
	//level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { -0.5,-0.5 }));

	level_2.emplace_back(SetEnemy(55.2f, EnemyType::Summon, { -3, -4 }));//1
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { -4, -3 }));//2
	level_2.emplace_back(SetEnemy(55.6f, EnemyType::Summon, { -3, -2 }));//8
	level_2.emplace_back(SetEnemy(55.4f, EnemyType::Summon, { -2, -3 }));//5

	//level_2.emplace_back(SetEnemy(55.1f, EnemyType::Summon, { -3.5, -3.5 }));//3
	//level_2.emplace_back(SetEnemy(55.3f, EnemyType::Summon, { -2.5, -3.5 }));//4
	//level_2.emplace_back(SetEnemy(55.5f, EnemyType::Summon, { -3.5, -2.5 }));//6
	//level_2.emplace_back(SetEnemy(55.7f, EnemyType::Summon, { -2.5, -2.5 }));//7

	/*level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 4, -3 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 3.5, -3.5 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 3, -4 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 2.5, -3.5 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 2, -3 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 3.5, -2.5 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 3, -2 }));
	level_2.emplace_back(SetEnemy(55.0f, EnemyType::Summon, { 2.5, -2.5 }));*/


	level_2.emplace_back(SetEnemy(65.0f, EnemyType::A, Shaft::Vertical, 12.0f));
	level_2.emplace_back(SetEnemy(65.0f, EnemyType::A, Shaft::Side, 5.0f));//4

	level_2.emplace_back(SetEnemy(73.0f, EnemyType::A, Shaft::Side, 17.0f));
	level_2.emplace_back(SetEnemy(73.0f, EnemyType::B, Shaft::Vertical, 17.0f));//5

	//âEè„
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { 4, -3 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { 3, -4 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { 3, -2 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { 2, -3 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { 3.5, -3.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { 2.5, -3.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { 3.5, -2.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { 2.5, -2.5 }));

	//ç∂è„
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { -4, 3 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { -3, 4 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { -3, 2 }));
	level_2.emplace_back(SetEnemy(85.0f, EnemyType::Summon, { -2, 3 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { -3.5, 3.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { -2.5, 3.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { -3.5, 2.5 }));
	//level_2.emplace_back(SetEnemy(82.0f, EnemyType::Summon, { -2.5, 2.5 }));


	level_2.emplace_back(SetEnemy(94.0f, EnemyType::B, Shaft::Side, 1.0f));
	level_2.emplace_back(SetEnemy(94.0f, EnemyType::B, Shaft::Vertical, 1.0f));//6

	level_2.emplace_back(SetEnemy(103.0f, EnemyType::A, Shaft::Side, 16.0f));
	level_2.emplace_back(SetEnemy(103.0f, EnemyType::A, Shaft::Vertical, 15.0f));//7

	level_2.emplace_back(SetEnemy(109.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_2.emplace_back(SetEnemy(109.0f, EnemyType::A, Shaft::Vertical, 8.0f));
	level_2.emplace_back(SetEnemy(109.0f, EnemyType::A, Shaft::Side, 10.0f));//8(3)

	level_2.emplace_back(SetEnemy(119.0f, EnemyType::A, Shaft::Side, 17.0f));
	level_2.emplace_back(SetEnemy(119.0f, EnemyType::B, Shaft::Vertical, 17.0f));//5



}

void EnemySpawner::InitializeLevel3()
{
	level_3.emplace_back(SetEnemy(1.0f, EnemyType::A, Shaft::Side, 5.0f));
	level_3.emplace_back(SetEnemy(1.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_3.emplace_back(SetEnemy(3.0f, EnemyType::B, Shaft::Vertical, 13.0f));//1

	level_3.emplace_back(SetEnemy(22.0f, EnemyType::B, Shaft::Side, 3.0f));
	level_3.emplace_back(SetEnemy(25.0f, EnemyType::B, Shaft::Vertical, 6.0f));//2

	level_3.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_3.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_3.emplace_back(SetEnemy(32.0f, EnemyType::A, Shaft::Side, 10.0f));//3

	//level_3.emplace_back(SetEnemy(40.0f, EnemyType::B, Shaft::Vertical, 5.0f));
	//level_3.emplace_back(SetEnemy(40.0f, EnemyType::B, Shaft::Vertical,));
	level_3.emplace_back(SetEnemy(33.0f, EnemyType::B, Shaft::Vertical, 7.0f));//4

	level_3.emplace_back(SetEnemy(34.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_3.emplace_back(SetEnemy(34.0f, EnemyType::A, Shaft::Side, 15.0f));

	level_3.emplace_back(SetEnemy(36.0f, EnemyType::A, Shaft::Side, 6.0f));
	level_3.emplace_back(SetEnemy(36.0f, EnemyType::A, Shaft::Side, 12.0f));//3

	level_3.emplace_back(SetEnemy(45.0f, EnemyType::Summon, { -3, -4 }));//1
	level_3.emplace_back(SetEnemy(45.0f, EnemyType::Summon, { -4, -3 }));//2
	level_3.emplace_back(SetEnemy(45.0f, EnemyType::Summon, { -3, -2 }));//8
	level_3.emplace_back(SetEnemy(45.0f, EnemyType::Summon, { -2, -3 }));//5

	level_3.emplace_back(SetEnemy(50.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_3.emplace_back(SetEnemy(50.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_3.emplace_back(SetEnemy(50.0f, EnemyType::A, Shaft::Side, 10.0f));//3

	level_3.emplace_back(SetEnemy(52.0f, EnemyType::B, Shaft::Vertical, 15.5f));//6

	level_3.emplace_back(SetEnemy(54.0f, EnemyType::A, Shaft::Side, 7.5f));
	level_3.emplace_back(SetEnemy(54.0f, EnemyType::A, Shaft::Side, 8.5f));
	level_3.emplace_back(SetEnemy(54.0f, EnemyType::A, Shaft::Side, 9.5f));//3
	level_3.emplace_back(SetEnemy(54.0f, EnemyType::A, Shaft::Side, 10.5f));//3

	level_3.emplace_back(SetEnemy(55.0f, EnemyType::B, Shaft::Vertical, 13.5f));//6

	level_3.emplace_back(SetEnemy(60.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_3.emplace_back(SetEnemy(60.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_3.emplace_back(SetEnemy(60.0f, EnemyType::A, Shaft::Side, 10.0f));//3

	level_3.emplace_back(SetEnemy(65.0f, EnemyType::Boss, Shaft::Side, 9.0f));

	level_3.emplace_back(SetEnemy(67.0f, EnemyType::Summon, { -4, 3 }));
	level_3.emplace_back(SetEnemy(67.0f, EnemyType::Summon, { -3, 4 }));
	level_3.emplace_back(SetEnemy(67.0f, EnemyType::Summon, { -3, 2 }));
	level_3.emplace_back(SetEnemy(67.0f, EnemyType::Summon, { -2, 3 }));

	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { 4, -3 }));
	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { 3, -4 }));
	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { 3, -2 }));
	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { 2, -3 }));	

	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { -3, 6 }));	
	level_3.emplace_back(SetEnemy(69.0f, EnemyType::Summon, { 3, 6 }));	
	


	level_3.emplace_back(SetEnemy(75.0f, EnemyType::B, Shaft::Vertical, 8.5f));//6
	level_3.emplace_back(SetEnemy(75.0f, EnemyType::B, Shaft::Vertical, 15.5f));//6


	level_3.emplace_back(SetEnemy(90.0f, EnemyType::B, Shaft::Vertical, 1.0f));
	level_3.emplace_back(SetEnemy(90.0f, EnemyType::A, Shaft::Side, 11.0f));

	//level_3.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Vertical, 1.0f));
	//level_3.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Side, 1.0f));
	//level_3.emplace_back(SetEnemy(82.0f, EnemyType::B, Shaft::Side, 4.0f));
	//level_3.emplace_back(SetEnemy(82.0f, EnemyType::A, Shaft::Vertical, 9.0f));//5

	//level_3.emplace_back(SetEnemy(92.0f, EnemyType::B, Shaft::Side, 8.0f));
	//level_3.emplace_back(SetEnemy(92.0f, EnemyType::A, Shaft::Vertical, 1.0f));

}

void EnemySpawner::InitializeLevel4()
{
	// level_4Ç©íçà”
	//level_4.emplace_back(SetEnemy(1.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_4.emplace_back(SetEnemy(1.0f, EnemyType::B, Shaft::Vertical, 1.0f));
	level_4.emplace_back(SetEnemy(3.0f, EnemyType::A, Shaft::Vertical, 17.0f));//1

	level_4.emplace_back(SetEnemy(13.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_4.emplace_back(SetEnemy(15.0f, EnemyType::A, Shaft::Side, 7.0f));
	level_4.emplace_back(SetEnemy(15.0f, EnemyType::A, Shaft::Side, 9.0f));
	level_4.emplace_back(SetEnemy(17.0f, EnemyType::A, Shaft::Side, 8.0f));

	//level_4.emplace_back(SetEnemy(1.0f, EnemyType::B, Shaft::Vertical, 1.0f));
	//level_4.emplace_back(SetEnemy(3.0f, EnemyType::A, Shaft::Vertical, 17.0f));//1

	level_4.emplace_back(SetEnemy(23.0f, EnemyType::B, Shaft::Side, 3.0f));
	//level_4.emplace_back(SetEnemy(23.0f, EnemyType::B, Shaft::Side, 16.0f));
	level_4.emplace_back(SetEnemy(23.0f, EnemyType::B, Shaft::Vertical, 11.0f));//2

	level_4.emplace_back(SetEnemy(29.0f, EnemyType::A, Shaft::Vertical, 5.0f));
	level_4.emplace_back(SetEnemy(31.0f, EnemyType::A, Shaft::Vertical, 4.0f));
	level_4.emplace_back(SetEnemy(31.0f, EnemyType::A, Shaft::Vertical, 6.0f));
	level_4.emplace_back(SetEnemy(33.0f, EnemyType::A, Shaft::Vertical, 5.0f));


	level_4.emplace_back(SetEnemy(38.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_4.emplace_back(SetEnemy(38.0f, EnemyType::C, Shaft::Vertical, 3.0f));
	level_4.emplace_back(SetEnemy(39.0f, EnemyType::C, Shaft::Vertical, 15.0f));//3

	level_4.emplace_back(SetEnemy(42.0f, EnemyType::B, Shaft::Side, 1.0f));
	level_4.emplace_back(SetEnemy(42.0f, EnemyType::B, Shaft::Vertical, 9.0f));//4

	level_4.emplace_back(SetEnemy(50.0f, EnemyType::C, Shaft::Vertical, 5.0f));
	//level_4.emplace_back(SetEnemy(50.0f, EnemyType::B, Shaft::Side, 3.0f));
	level_4.emplace_back(SetEnemy(51.0f, EnemyType::B, Shaft::Side, 12.0f));//5

	level_4.emplace_back(SetEnemy(52.0f, EnemyType::Summon, { 1, 7 }));
    level_4.emplace_back(SetEnemy(52.0f, EnemyType::Summon, { 0, 8 }));
	level_4.emplace_back(SetEnemy(52.0f, EnemyType::Summon, { -1, 7 }));
	level_4.emplace_back(SetEnemy(52.0f, EnemyType::Summon, { 0, 6 }));

	level_4.emplace_back(SetEnemy(51.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_4.emplace_back(SetEnemy(55.0f, EnemyType::A, Shaft::Side, 8.0f));

	level_4.emplace_back(SetEnemy(59.0f, EnemyType::C, Shaft::Side, 16.0f));
	level_4.emplace_back(SetEnemy(60.0f, EnemyType::A, Shaft::Vertical, 17.5f));//6

	level_4.emplace_back(SetEnemy(69.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_4.emplace_back(SetEnemy(70.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_4.emplace_back(SetEnemy(79.0f, EnemyType::A, Shaft::Vertical, 6.0f));//7

	level_4.emplace_back(SetEnemy(87.0f, EnemyType::C, Shaft::Side, 16.0f));
	level_4.emplace_back(SetEnemy(90.0f, EnemyType::B, Shaft::Side, 1.0f));//8

	level_4.emplace_back(SetEnemy(100.0f, EnemyType::A, Shaft::Vertical, 14.0f));
	level_4.emplace_back(SetEnemy(105.0f, EnemyType::B, Shaft::Side, 15.0f));//8



}

void EnemySpawner::InitializeLevel5()
{
	// level_5Ç©íçà”
	level_5.emplace_back(SetEnemy(1.0f, EnemyType::A, Shaft::Vertical, 3.0f));
	level_5.emplace_back(SetEnemy(1.0f, EnemyType::B, Shaft::Side, 5.0f));//1

	level_5.emplace_back(SetEnemy(10.0f, EnemyType::A, Shaft::Vertical, 8.0f));
	level_5.emplace_back(SetEnemy(13.0f, EnemyType::C, Shaft::Vertical, 15.0f));

	level_5.emplace_back(SetEnemy(19.0f, EnemyType::C, Shaft::Vertical, 17.5f));
	level_5.emplace_back(SetEnemy(19.0f, EnemyType::C, Shaft::Vertical, 8.0f));
	level_5.emplace_back(SetEnemy(22.0f, EnemyType::B, Shaft::Side, 9.0f));//2

	level_5.emplace_back(SetEnemy(30.0f, EnemyType::C, Shaft::Vertical, 10.0f));
	level_5.emplace_back(SetEnemy(32.0f, EnemyType::C, Shaft::Side, 2.0f));//3
	level_5.emplace_back(SetEnemy(32.0f, EnemyType::B, Shaft::Side, 9.0f));//2

	level_5.emplace_back(SetEnemy(39.0f, EnemyType::A, Shaft::Vertical, 2.0f));
	level_5.emplace_back(SetEnemy(42.0f, EnemyType::B_2, Shaft::Side, 17.5f));//4

	level_5.emplace_back(SetEnemy(48.0f, EnemyType::B, Shaft::Side, 4.0f));

	level_5.emplace_back(SetEnemy(55.0f, EnemyType::C, Shaft::Vertical, 4.0f));
	level_5.emplace_back(SetEnemy(58.0f, EnemyType::B_2, Shaft::Vertical, 16.0f));
	level_5.emplace_back(SetEnemy(63.0f, EnemyType::A, Shaft::Side, 9.0f));//5

	level_5.emplace_back(SetEnemy(70.0f, EnemyType::C, Shaft::Vertical, 8.5f));
	level_5.emplace_back(SetEnemy(72.0f, EnemyType::C, Shaft::Side, 8.0f));//6

	level_5.emplace_back(SetEnemy(80.0f, EnemyType::B_2, Shaft::Vertical, 16.0f));
	level_5.emplace_back(SetEnemy(83.0f, EnemyType::A_2, Shaft::Side, 9.0f));//7

	level_5.emplace_back(SetEnemy(92.0f, EnemyType::A_2, Shaft::Vertical, 8.0f));
	level_5.emplace_back(SetEnemy(97.0f, EnemyType::C, Shaft::Vertical, 15.0f));//8

	level_5.emplace_back(SetEnemy(102.0f, EnemyType::A, Shaft::Vertical, 2.0f));
	level_5.emplace_back(SetEnemy(108.0f, EnemyType::B_2, Shaft::Side, 13.5f));

	level_5.emplace_back(SetEnemy(113.0f, EnemyType::B, Shaft::Side, 5.0f));//9

	level_5.emplace_back(SetEnemy(119.0f, EnemyType::C, Shaft::Vertical, 3.0f));
	level_5.emplace_back(SetEnemy(124.0f, EnemyType::B_2, Shaft::Vertical, 13.0f));
	level_5.emplace_back(SetEnemy(130.0f, EnemyType::A_2, Shaft::Side, 14.0f));//5

}

void EnemySpawner::InitializeLevel6()
{
	level_6.emplace_back(SetEnemy(1.0f, EnemyType::B, Shaft::Vertical, 5.0f));
	level_6.emplace_back(SetEnemy(2.0f, EnemyType::C, Shaft::Side, 8.0f));//1

	level_6.emplace_back(SetEnemy(10.0f, EnemyType::D, Shaft::Vertical, 15.0f));
	level_6.emplace_back(SetEnemy(13.0f, EnemyType::B_2, Shaft::Side, 8.0f));//2

	level_6.emplace_back(SetEnemy(20.0f, EnemyType::D, Shaft::Vertical, 7.0f));
	level_6.emplace_back(SetEnemy(20.0f, EnemyType::D, Shaft::Vertical, 16.5f));
	level_6.emplace_back(SetEnemy(22.0f, EnemyType::C, Shaft::Side, 14.0f));//3

	level_6.emplace_back(SetEnemy(27.0f, EnemyType::C, Shaft::Side, 17.5f));

	level_6.emplace_back(SetEnemy(29.0f, EnemyType::A, Shaft::Vertical, 10.0f));
	level_6.emplace_back(SetEnemy(34.0f, EnemyType::A_2, Shaft::Side, 16.0f));//4

	level_6.emplace_back(SetEnemy(40.0f, EnemyType::D, Shaft::Vertical, 2.0f));
	level_6.emplace_back(SetEnemy(42.0f, EnemyType::A_2, Shaft::Vertical, 16.0f));
	level_6.emplace_back(SetEnemy(44.0f, EnemyType::C, Shaft::Side, 1.5f));//5

	level_6.emplace_back(SetEnemy(49.0f, EnemyType::C, Shaft::Vertical, 15.0f));//6
	level_6.emplace_back(SetEnemy(52.0f, EnemyType::D, Shaft::Side, 5.0f));

	level_6.emplace_back(SetEnemy(59.0f, EnemyType::C, Shaft::Side, 17.0f));
	level_6.emplace_back(SetEnemy(61.0f, EnemyType::A, Shaft::Side, 16.0f));//7

	level_6.emplace_back(SetEnemy(68.0f, EnemyType::A, Shaft::Side, 8.0f));
	level_6.emplace_back(SetEnemy(72.0f, EnemyType::D, Shaft::Vertical, 12.0f));//8

	level_6.emplace_back(SetEnemy(80.0f, EnemyType::A, Shaft::Side, 3.0f));
	level_6.emplace_back(SetEnemy(81.0f, EnemyType::B_2, Shaft::Side, 16.0f));//9

	level_6.emplace_back(SetEnemy(90.0f, EnemyType::D, Shaft::Vertical, 2.0f));
	level_6.emplace_back(SetEnemy(93.0f, EnemyType::A, Shaft::Vertical, 16.0f));
	level_6.emplace_back(SetEnemy(100.0f, EnemyType::C, Shaft::Side, 1.5f));//10

	level_6.emplace_back(SetEnemy(105.0f, EnemyType::C, Shaft::Vertical, 14.0f));//11
	level_6.emplace_back(SetEnemy(110.0f, EnemyType::D, Shaft::Side, 13.0f));
}

void EnemySpawner::InitializeLevel7()
{
	level_7.emplace_back(SetEnemy(1.0f, EnemyType::Summon, {-2.5,6}));//11
	level_7.emplace_back(SetEnemy(1.0f, EnemyType::Summon, {2.5,6}));//11
	level_7.emplace_back(SetEnemy(1.0f, EnemyType::C, Shaft::Vertical, 14.0f));//11
}

void EnemySpawner::InitializeLevel8()
{
	level_8.emplace_back(SetEnemy(1.0f, EnemyType::Summon, { -2.5,6 }));//11
}

void EnemySpawner::InitializeLevel9()
{
}
