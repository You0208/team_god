#include "SeedManager.h"
#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"
#include "Game/CollisionManager.h"

// XVˆ—
void SeedManager::Update(float elapsedTime)
{
    for (Seed* seed : seeds)
    {
        // ŠÔØ‚ê‚Ì
        if (seed->GetTimer() >= seed_limit_time)
        {
            // €–S‚ÉØ‚è‘Ö‚¦
            seed->SetDead(true);
        }
        else
        {
            // í‚ÌXV
            seed->Update(elapsedTime);
        }

        // €–S‚Ìˆ—
        if (seed->IsDead())
        {
            for (Seed* s : seeds)
            {
                // ‚à‚µíœ—\’è‚Ìí”Ô†‚æ‚è‚à‘å‚«‚¢”Ô†‚Ìí‚ª‚ ‚ê‚ÎA”Ô†‚ğŒ¸Z‚·‚é
                if (seed->GetNumber() < s->GetNumber())s->DecNumber();
            }
            // Œ»İ‚Ìí”Ô†‚ğŒ¸Z
            seed_number--;

            // í‚ğƒŠƒXƒg‚©‚çÁ‚·
            Remove(seed);
        }
    }


    // ”jŠüˆ—
    for (Seed* seed : removes)
    {
        std::vector<Seed*>::iterator it = std::find(seeds.begin(), seeds.end(), seed);
        if (it != seeds.end())
        {
            seeds.erase(it);
        }

        // íœ
        delete seed;
    }
    // ”jŠüƒŠƒXƒg‚ğƒNƒŠƒA
    removes.clear();
}

// •`‰æˆ—
void SeedManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    DrawDebugPrimitive();
    for (Seed* seed : seeds)
    {
        seed->Render(elapsedTime, replaced_pixel_shader);
    }
}

void SeedManager::Register(Seed* seed)
{
    // ‰Šú’l‚ğİ’è
    seed->SetNumber(seed_number);

    seeds.emplace_back(seed);
    // ”Ô†‚ğ‰ÁZ
    seed_number++;
}

void SeedManager::Clear()
{
    for (Seed* seed : seeds)
    {
        delete seed;
    }
    seeds.clear();
}

void SeedManager::DrawDebugPrimitive()
{
    for (Seed* seed : seeds)
    {
        seed->DrawDebugPrimitive();
    }
}

void SeedManager::Remove(Seed* seed)
{
    // ”jŠüƒŠƒXƒg‚É’Ç‰Á
    removes.insert(seed);
}
