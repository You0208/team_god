#include "Seed.h"
#include "Lemur/Input/Input.h"
#include "UnitManager.h"
#include "SeedManager.h"
#include "Unit_ABC.h"
#include "Unit_DEF.h"

#include <random>

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\seed.fbx");
    scaleFactor = 0.1f;
    radius = 0.3f;
    height = 0.3f;
    model->PlayAnimation(0, false);
}

Seed::~Seed()
{
}

void Seed::Update(float elapsedTime)
{
    // スケール更新
    UpdateScale();

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // モデルアニメーション更新
    model->UpdateAnimation(elapsedTime);

    // 行列更新処理
    UpdateTransform();

    // Imgui
    DrawDebugGUI();

    if (is_direction)
    {
        throwDirection();
    }
    else if (is_dis_direction)
    {
        DisDirection();
    }
    else
    {

        timer += elapsedTime;

        if (born)
        {
            Unit* unit = nullptr;

            switch (category)
            {
            case 0:
                unit = new Unit_A;
                break;
            case 1:
                unit = new Unit_B;
                break;
            case 2:
                unit = new Unit_C;
                break;
            case 3:
                unit = new Unit_D;
                break;
            case 4:
                unit = new Unit_E;
                break;
            case 5:
                unit = new Unit_F;
                break;
            }

            if (unit != nullptr)
            {
                unit->SetPosition(position);
                // 奥行は適当に設定
                Rect square = {
                    {position.x - radius,position.z + 100.0f},
                    {position.x + radius,position.z - radius}
                };
                unit->SetSquare(square);
                unit->SetCategory(category);
                unit->UpdateTransform();
                UnitManager::Instance().Register(unit);

                death = true;// 種を消す
            }
        }

        // 飛ばしたての種の位置決定
        if (!decision_pos)DecisionPos();
    }
}

void Seed::throwDirection()
{
    if (position.z>= distination_position.z)
    {
        velocity.z = 0;
        is_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
}

void Seed::DisDirection()
{
    if (position.z >= distination_position.z)
    {
        velocity.z = 0;
        death = true;
        is_dis_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
}

void Seed::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Seed::DrawDebugGUI()
{
    ImGui::Begin("Seed");

    ImGui::DragInt("number", &number);
    ImGui::DragFloat("timer", &timer);
    ImGui::Checkbox("born", &born);

    ImGui::End();
}

void Seed::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 1,0,0,1 });
}

void Seed::DecisionPos()
{
    SeedManager& seedManager = SeedManager::Instance();

    // 全ての種と総当たりで衝突判定
    int seedCount = seedManager.GetSeedCount();
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        if (!seed->decision_pos)continue;

        if (Collision::IntersectCircleVsCircle(
            { position.x,position.z }, radius,
            { seed->GetPosition().x, seed->GetPosition().z },
            seed->GetRadius()
        ))
        {
            if (overlap_num == 0)
            {
                outPosition.x = MoveInRandomDirection({ position.x,position.z }, 1.0f).x;
                outPosition.y = MoveInRandomDirection({ position.x,position.z }, 1.0f).y;
                overlap_num++;// 重なった回数を加算
            }

            // 押し出された地点にまた種があるか確認
            for (int j = 0; j < seedCount; ++j)
            {
                if (j == i)continue;

                Seed* seed2 = seedManager.GetSeed(j);
                if (!seed2->decision_pos)continue;

                if (Collision::IntersectCircleVsCircle(
                    outPosition, radius,
                    { seed2->GetPosition().x, seed2->GetPosition().z },
                    seed2->GetRadius()
                ))
                {
                    // 重なった回数を加算
                    overlap_num++;
                }
            }

        }
    }
    // 当たらなかったら
    if (overlap_num == 0)
    {
        outPosition = {};
        decision_pos = true;// 何もせず位置決定フラグをON
    }
    else if (overlap_num == 1)// 重なった回数が１回なら
    {
        // 押し出し位置にずらし
        position.x = outPosition.x;
        position.z = outPosition.y;
        decision_pos = true;// 位置決定フラグをON
        outPosition = {};
    }
    else if (overlap_num >= 2)// 重なった回数が２回以上なら
    {
        outPosition = {};
        death = true;// 種を破壊
    }
}

DirectX::XMFLOAT2 Seed::GenerateRandomDirection()
{
    // 乱数生成器の準備
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    // ランダムな角度を計算
    float angle = dis(gen) * DirectX::XM_2PI;

    // ベクトルを生成
    DirectX::XMFLOAT2 direction;
    direction.x = cos(angle);
    direction.y = sin(angle);

    return direction;
}

DirectX::XMFLOAT2 Seed::MoveInRandomDirection(const DirectX::XMFLOAT2& position, float distance)
{
    // ランダムな方向ベクトルを生成
    DirectX::XMFLOAT2 direction = GenerateRandomDirection();

    // 距離を掛けて新しい座標を計算
    DirectX::XMFLOAT2 newPosition;
    newPosition.x = position.x + direction.x * distance;
    newPosition.y = position.y + direction.y * distance;

    return newPosition;
}
