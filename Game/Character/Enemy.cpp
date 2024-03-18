#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"
void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}

void Enemy::DrawDebugPrimitive()
{
}
