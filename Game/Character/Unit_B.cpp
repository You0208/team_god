#include "Unit_B.h"
#include "EnemyManager.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_B::Unit_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times    = 5;    // 攻撃回数
    attack_power    = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_radius   = 1.0f; // 攻撃範囲
    radius          = 0.3f; // 半径
    height          = 0.5f; // デバッグ用
    dec_pos         = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // 三角
    t_height = 1.0f;// 高さ
    t_base = 1.0f;// 底辺

    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左奥側の頂点
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // 右手前側の頂点
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };


    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_B::~Unit_B()
{
}

void Unit_B::Update(float elapsedTime)
{
    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左奥側の頂点
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // 右手前側の頂点
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    DrawDebugPrimitive();
    DrawDebugGUI();

    AttackEnemy(elapsedTime);
}

void Unit_B::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_B::AttackEnemy(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        // 左三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->position.x,enemy->position.z },  // 敵の位置(XZ平面)
            enemy->radius                           // 敵の当たり判定
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                if (enemy->ApplyDamage(attack_power))
                {
                    attack_timer = 0.0f;
                    attack_times--;
                }
            }
        }
        // 右三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->position.x,enemy->position.z },  // 敵の位置(XZ平面)
            enemy->radius                           // 敵の当たり判定
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                if (enemy->ApplyDamage(attack_power))
                {
                    attack_timer = 0.0f;
                    attack_times--;
                }
            }
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        UnitManager::Instance().Remove(this);
    }
}

void Unit_B::DrawDebugGUI()
{
    std::string name = "Unit_B";

    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string spe = std::string("t_height") + name;
        ImGui::SliderFloat(spe.c_str(), &t_height, 5.0f, 0.0f);
        std::string pe = std::string("t_base") + name;
        ImGui::SliderFloat(pe.c_str(), &t_base, 5.0f, 0.0f);

        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + name;
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}

void Unit_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    // 左三角
    debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    // 右三角
    debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}