#include "Unit.h"
#include "Lemur/Graphics/Graphics.h"
#include "UnitManager.h"
void Unit::Destroy()
{
    UnitManager::Instance().Remove(this);
}

void Unit::DrawDebugPrimitive()
{
}
