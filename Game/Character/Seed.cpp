#include "Seed.h"
#include "Lemur/Input/Input.h"
#include "UnitManager.h"
#include "SeedManager.h"
#include "Unit_ABC.h"
#include "Unit_DEF.h"
#include "Unit_HIJ.h"
#include "../Stage/Fence.h"

#include "Lemur/Effekseer/EffekseerManager.h"

#include <random>

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\seed.fbx");
    error_effect = new Effect(".\\resources\\Effect\\SEED_ERROR\\SEED_ERROR.efk");

    scaleFactor = 0.1f;

    //TODO　もね
    radius = 0.28f;
    height = 0.28f;
    rotation.y = DirectX::XMConvertToRadians(90.0f);

    //TODO もね　
    effect_size = 0.3f;

    // 回す
    PlayAnimation(Animation_Index::Throw, true);
}

Seed::~Seed()
{
    //error_effect->Stop(error_effect_handle);
    delete error_effect;
    //EffectManager::Instance().Update(0.01f);
}

void Seed::Update(float elapsedTime)
{
    // 常に実行されている
    {
        // スケール更新
        UpdateScale();

        // 速力処理更新
        UpdateVelocity(elapsedTime);

        // モデルアニメーション更新
        UpdateAnimation(elapsedTime);

        // 行列更新処理
        UpdateTransform();

        // Imgui消す
        //DrawDebugGUI();
    }

    if (is_direction)// 種の演出
    {
        throwDirection();
    }
    else if (is_dis_direction)// ぶつかって消える種の演出
    {
        DisDirection();
    }
    else
    {
        timer += elapsedTime;

        // 発芽フラグがたっていたら
        if (born)
        {
            Unit* unit = nullptr;
            // ユニットの種類によって発芽先のユニットを変更
            if (!unit) {
                switch (category)
                {
                case UnitManager::UNIT_INDEX::Chili:
                    unit = new Chili();
                    break;
                case UnitManager::UNIT_INDEX::Shishito:
                    unit = new Shishito();
                    break;
                case UnitManager::UNIT_INDEX::GreenPumpkin:
                    unit = new GreenPumpkin();
                    break;
                case UnitManager::UNIT_INDEX::OrangePumpkin:
                    unit = new OrangePumpkin();
                    break;
                case UnitManager::UNIT_INDEX::Broccoli:
                    unit = new Broccoli();
                    break;
                case UnitManager::UNIT_INDEX::Cauliflower:
                    unit = new Cauliflower();
                    break;
                case UnitManager::UNIT_INDEX::J:
                    unit = new Unit_J();
                    break;
                default:
                    unit = new Chili();
                    break;
                }
                UnitManager::Instance().SetUpUnit(unit, category);
            }
            // ユニットが生まれたら
            if (unit != nullptr)
            {
                //発芽位置
                unit->SetPosition(position);
                // ユニット奥の四角（奥行は適当に設定）
                Rect square = {
                    {position.x - radius,position.z + 100.0f},
                    {position.x + radius,position.z - radius},
                };
                unit->SetSquare(square);
                // カテゴリーをセット
                unit->SetCategory(category);
                // 設置エフェクトを再生
                unit->PlaySetEffect(unit->GetPosition(), unit->GetSetEffectSize());

                // 適当な角度にする
                unit->SetAngle({ 0.0f,DirectX::XMConvertToRadians(rand() % 360),0.0f });

                // 姿勢を更新しておく
                unit->collision_pos = unit->GetPosition();
                unit->collision_scale = unit->GetScale();
                //unit->collision_rotation = unit->GetAngle();
                unit->Update(elapsedTime);

                // Managerにセット
                UnitManager::Instance().Register(unit);

                // 種を消す
                death = true;
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

        PlayAnimation(Animation_Index::Idle, true);
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
        PlayAnimation(Animation_Index::Jump, true);
        error_effect_handle = error_effect->Play(position, effect_size);
        velocity.z = 0;
        death = true;
        is_dis_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
}

void Seed::Render(float scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Character::Render(scale, replaced_pixel_shader);
}

void Seed::DrawDebugGUI()
{/*
    ImGui::Begin("Seed");

    ImGui::DragInt("number", &number);
    ImGui::DragFloat("timer", &timer);
    ImGui::Checkbox("born", &born);

    ImGui::End();*/
}

void Seed::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 1,0,0,1 });
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

    // 種が柵の外側にでたら消す
    if (Fence::Instance().GetLeftRect().right_down.x + seed_fence_dis >= outPosition.x||// 左
        Fence::Instance().GetRightRect().left_up.x - seed_fence_dis <= outPosition.x|| // 右
        Fence::Instance().GetBackRect().right_down.y - seed_fence_dis <= outPosition.y ||// 奥
        Fence::Instance().GetFrontRect().left_up.y + seed_fence_dis >= outPosition.y // 手前
        )
    {
        error_effect_handle = error_effect->Play(position, effect_size);
        death = true;// 種を破壊
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

        error_effect_handle = error_effect->Play(position, effect_size);
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
