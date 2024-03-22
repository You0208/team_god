#include "Unit_A.h"
#include "Lemur/Input/Input.h"

static Unit_A* instance = nullptr;

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_unit_A = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    position = { 0,5.0f,0 };
    height = 1.0f;

    instance = this;
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // ステートごとの処理
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Move:
        UpdateMoveState(elapsedTime);
        break;
    case State::Jump:
        UpdateJumpState(elapsedTime);
        break;
    case State::Fall:
        UpdateFallState(elapsedTime);
        break;
    case State::Land:
        UpdateLandState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    case State::Revive:
        UpdateReviveState(elapsedTime);
        break;
    }

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // プレイヤーと敵との衝突判定
    CollisionPlayerVsEnemies();

    UpdateTransform();

    // モデルアニメーション更新処理
    model_unit_A->UpdateAnimation(elapsedTime);

}

void Unit_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    model_unit_A->Render(elapsedTime, replaced_pixel_shader);
}

void Unit_A::DrawDebugGUI()
{
    if (ImGui::TreeNode("Unit_A"))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置
            ImGui::InputFloat3("position", &position.x);

            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(rotation.x);
            a.y = DirectX::XMConvertToDegrees(rotation.y);
            a.z = DirectX::XMConvertToDegrees(rotation.z);
            ImGui::InputFloat3("rotation", &a.x);
            rotation.x = DirectX::XMConvertToRadians(a.x);
            rotation.y = DirectX::XMConvertToRadians(a.y);
            rotation.z = DirectX::XMConvertToRadians(a.z);

            // スケール
            ImGui::InputFloat3("Scale", &scale.x);

            // ヴェロシティ
            ImGui::InputFloat3("velocity", &velocity.x);
            // 着地してるか
            ImGui::Checkbox("isGround", &isGround);
            // ジャンプカウント
            ImGui::InputInt("jumpCount", &jumpCount);

            // 発射レート
            ImGui::InputFloat("LuanchRate", &LuanchRate);
            // 発射クールタイム
            ImGui::InputFloat("LuanchCollTime", &LuanchCollTime);
        }
        ImGui::TreePop();
    }
}

void Unit_A::DrawDebugPrimitive()
{
}


Unit_A& Unit_A::Instance()
{
    return *instance;
}

void Unit_A::TransitionIdleState()
{
    state = State::Idle;

    model_unit_A->PlayAnimation(Anim_Idle, true);
}

void Unit_A::UpdateIdleState(float elapsedTime)
{
    // 移動入力処理
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }


    // 弾丸入力処理
    LuanchProjectile(elapsedTime);

    // 攻撃入力処理
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

void Unit_A::TransitionMoveState()
{
    state = State::Move;

    // 走りアニメーション再生
    model_unit_A->PlayAnimation(Anim_Running, true);
}

void Unit_A::UpdateMoveState(float elapsedTime)
{
    // 移動入力がなくなったら待機に切り替え
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }


    // 弾丸入力処理
    LuanchProjectile(elapsedTime);

    // 攻撃入力処理
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

void Unit_A::TransitionJumpState()
{
    state = State::Jump;

    // ジャンプアニメーション再生
    model_unit_A->PlayAnimation(Anim_Jump, false);
}

void Unit_A::UpdateJumpState(float elapsedTime)
{
    // 移動入力処理
    InputMove(elapsedTime);

    // 弾丸入力処理
    LuanchProjectile(elapsedTime);
}

void Unit_A::TransitionFallState()
{
    if (state == State::Damage || state == State::Death)
        return;

    state = State::Fall;

    model_unit_A->PlayAnimation(Anim_Falling, true);
}

void Unit_A::UpdateFallState(float elapsedTime)
{
    // 移動入力処理
    InputMove(elapsedTime);


    // 弾丸入力処理
    LuanchProjectile(elapsedTime);
}

void Unit_A::TransitionLandState()
{
    // ダメージ、死亡ステート時は遷移しない
    if (state == State::Damage || state == State::Death)
        return;

    state = State::Land;

    // 着地アニメーション再生
    model_unit_A->PlayAnimation(Anim_Landing, false);
}

void Unit_A::UpdateLandState(float elapsedTime)
{ 
    // アニメーション再生が終わったら着地状態終了
    if (!model_unit_A->IsPlayAnimation())
    {
        TransitionIdleState();
    }
}

void Unit_A::TransitionAttackState()
{
    state = State::Attack;

    model_unit_A->PlayAnimation(Anim_Attack, false);
}

void Unit_A::UpdateAttackState(float elapsedTime)
{
    // アニメーション再生が終わったら攻撃状態終了
    if (!model_unit_A->IsPlayAnimation())
    {
        TransitionIdleState();
    }

    // 任意のアニメーション区間でのみ衝突判定処理をする

    float attackStart = 0.3f;
    float attackEnd = 0.4f;
    float animeTime = model_unit_A->GetCurrentAnimationSecond();

    attackCollisionFlag = (animeTime > attackStart && animeTime < attackEnd);

    if (attackCollisionFlag)
    {
        // 左手ノードとエネミーの衝突処理
        CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
    }
}

void Unit_A::TransitionDamageState()
{
    state = State::Damage;

    // ダメージアニメーション再生
    model_unit_A->PlayAnimation(Anim_GetHit1, false);
}

void Unit_A::UpdateDamageState(float elapsedTime)
{
    // アニメーションが終わったら待機ステートへ遷移
    if (!model_unit_A->IsPlayAnimation())
        TransitionIdleState();
}

void Unit_A::TransitionDeathState()
{
    state = State::Death;

    // 死亡アニメーション再生
    model_unit_A->PlayAnimation(Anim_Death, false);
}

void Unit_A::UpdateDeathState(float elapsedTime)
{
    if (!model_unit_A->IsPlayAnimation())
    {
        // ボタンを押したら復活ステートに切り替え
        GamePad& game_pad = Input::Instance().GetGamePad();
        if (game_pad.GetButtonDown() & GamePad::BTN_A)
        {
            TransitionReviveState();
        }
    }
}

void Unit_A::TransitionReviveState()
{
    state = State::Revive;

    // 体力全回復
    health = max_health;

    // 復活アニメーション再生
    model_unit_A->PlayAnimation(Anim_Revive, false);
}

void Unit_A::UpdateReviveState(float elapsedTime)
{
    // 復活アニメーションが終わったら待機ステートへ遷移
    if (!model_unit_A->IsPlayAnimation())
        TransitionIdleState();
}

void Unit_A::OnLanding()
{
    // 着地したら着地ステートに切り替え
    if (state != State::Damage || state != State::Death)
    {
        TransitionLandState();
        jumpCount = 0;
    }
}

void Unit_A::OnDamage()
{
    TransitionDamageState();
}

void Unit_A::OnDead()
{
    TransitionDeathState();
}

DirectX::XMFLOAT3 Unit_A::GetMoveVec() const
{
    return DirectX::XMFLOAT3();
}

bool Unit_A::InputMove(float elapsedTime)
{ 
    // 進行ベクトル取得
    DirectX::XMFLOAT3 vec = GetMoveVec();
    Move(vec.x, vec.z, moveSpeed);
    Turn(elapsedTime, vec.x, vec.z, moveSpeed);

    // 進行ベクトルがゼロベクトルでない場合は入力された
    if (vec.x != 0.0f || vec.z != 0.0f)
        return true;

    return false;
}

void Unit_A::CollisionPlayerVsEnemies()
{
}

void Unit_A::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
}

void Unit_A::LuanchProjectile(float elapsedTime)
{
}

bool Unit_A::InputProjectile()
{
    return false;
}

bool Unit_A::InputAttack()
{
    return false;
}
