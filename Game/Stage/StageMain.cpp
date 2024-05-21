#include "StageMain.h"
#include "Fence.h"

StageMain::StageMain()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    stage_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\plantune.fbx");
    ground = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\ground\\ground.fbx");
    fild1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\field_1\\field_1.fbx");
    fild2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\field_2\\field_2.fbx");
    fence = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence\\fence.fbx");
    fence_type1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence_broken1\\fence_broken1.fbx");
    fence_type2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence_broken2\\fence_broken2.fbx");
    fence2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence\\fence.fbx");
    fence2_type1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence_broken1\\fence_broken1.fbx");
    fence2_type2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence_broken2\\fence_broken2.fbx");
    fence_end = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\fence_end\\fence_end.fbx");
    
    ido = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\ido\\ido.fbx");
    ido_yane = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\ido_yane\\ido_yane.fbx");
    cave = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\cave\\cave_set_RE.fbx");
    rock = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\rock\\rock.fbx");
    silo = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\silo\\silo.fbx");
    log = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\log\\log_set.fbx");

    for (int j = 0; j < 3; j++)
    {
        une[j] = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\une\\une.fbx");
    }
    for (int i = 0; i < 7; i++)
    {
        leavs_LV1[i] = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\leavs_LV1\\leavs_LV1.fbx");
        leavs_LV2[i] = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\leavs_LV2\\leavs_LV2.fbx");

        tree[i] = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\tree\\tree.fbx");

        leavs_LV1[i]->GetTransform()->SetPosition({ -7.0f + (i * 2.3f), 0.3f, -15.0f });
        leavs_LV1[i]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand() % 360));

        leavs_LV2[i]->GetTransform()->SetPosition({ -7.0f + (i * 2.3f), 0.3f, -12.0f });
        leavs_LV2[i]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand() % 360));
    }
    scale_facter = 1.0f;
    stage_width = { 9,9 };
    scale = { 1.0f,1.0f,1.0f };
    position = { 0.0f,0.0f,0.0f };
    stage_collision = {
        {position.x - stage_width.x,position.z + stage_width.y},
        {position.x + stage_width.x,position.z - stage_width.y}
    };
    variable_stage_width =
    {
        stage_width.x,
        stage_width.y
    };

    // 木
    tree[0]->GetTransform()->SetPosition({ -9.0f,dec_y,12.0f });
    tree[1]->GetTransform()->SetPosition({ -6.0f,dec_y,16.0f });
    tree[2]->GetTransform()->SetPosition({ 6.0f,dec_y,17.0f });
    tree[3]->GetTransform()->SetPosition({ 10.0f,dec_y,13.0f });
    tree[4]->GetTransform()->SetPosition({ 13.0f,dec_y,8.0f });
    tree[5]->GetTransform()->SetPosition({ 16.0f,dec_y,15.0f });
    tree[6]->GetTransform()->SetPosition({ -12.0f,dec_y,18.0f });

    tree[0]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[1]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[2]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[3]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[4]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[5]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));
    tree[6]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rand()%360));

    ido->GetTransform()->SetPositionX(-13.0f);
    ido->GetTransform()->SetPositionY(dec_y);
    ido_yane->GetTransform()->SetPositionX(-13.0f);
    ido_yane->GetTransform()->SetPositionY(dec_y);

    cave->GetTransform()->SetPositionZ(12.0f);
    cave->GetTransform()->SetPositionY(dec_y);
    cave->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180.0f));

    silo->GetTransform()->SetPosition({ -14.0f,dec_y,11.0f });

    une[0]->GetTransform()->SetPosition({ 0,dec_y ,-12 });
    une[1]->GetTransform()->SetPosition({ 0,dec_y ,-15 });
    une[2]->GetTransform()->SetPosition({ -13,dec_y ,-12 });
    une[2]->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(90.0f));

    log->GetTransform()->SetPosition({ 14.0f,dec_y ,2.0f });
    log->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-14.0f));
}

StageMain::~StageMain()
{
}

void StageMain::Update(float elapsedTime)
{
    //TODO ImGui消す
    //DrawDebugGui();
    scale.x = scale.y = scale.z = scale_facter;

    // 可変のステージ幅をスケーリング
    variable_stage_width.x = stage_width.x * scale_facter;
    variable_stage_width.y = stage_width.y * scale_facter;

    // ステージの四角をスケーリングに合わせる
    stage_collision = {
        {position.x - variable_stage_width.x,position.z + variable_stage_width.y},
        {position.x + variable_stage_width.x,position.z - variable_stage_width.y}
    };

    stage_model->GetTransform()->SetPosition(position);
    stage_model->GetTransform()->SetScale(scale);
}

void StageMain::Render(float scale, ID3D11PixelShader** replaced_pixel_shader, ID3D11PixelShader** replaced_pixel_shader2)
{
    // 地形
    ground->GetTransform()->SetPositionY(dec_y);
    ground->Render(scale*2, replaced_pixel_shader);
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 6; y++)
        {
            if (y % 2 == 0)
            {
                fild2->GetTransform()->SetPosition({ -7.4f + (x * 2 * 3.0f),dec_y,-7.4f + (y * 3.0f) });
                fild1->GetTransform()->SetPosition({ -4.4f + (x * 2 * 3.0f),dec_y,-7.4f + (y * 3.0f) });
                fild1->Render(scale, replaced_pixel_shader);
                fild2->Render(scale, replaced_pixel_shader);
            }
            else
            {
                fild1->GetTransform()->SetPosition({ -7.4f + (x * 2 * 3.0f),dec_y,-7.4f + (y * 3.0f) });
                fild2->GetTransform()->SetPosition({ -4.4f + (x * 2 * 3.0f),dec_y,-7.4f + (y * 3.0f) });
                fild1->Render(scale, replaced_pixel_shader);
                fild2->Render(scale, replaced_pixel_shader);
            }
        }
    }
    
    
    switch (Fence::Instance().fence_state)
    {
    case 0:
        // 柵
        for (int y = 0; y < 7; y++)
        {
            fence2->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence2->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) });
            fence2->Render(scale, replaced_pixel_shader);
            if (y == 6)
            {
                fence_end->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) - 2.5f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(0.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        for (int x = 0; x < 7; x++)
        {
            fence->GetTransform()->SetPosition({ -8.0f + (x * 2.5f),dec_y,-9.0f });
            fence->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
            fence->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence->Render(scale, replaced_pixel_shader);

            if (x == 6)
            {
                fence_end->GetTransform()->SetPosition({ -8.0f + (x * 2.5f) + 2.5f,dec_y,-9.0f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        fence->Render(scale, replaced_pixel_shader);
        break;
    case 1:
        // 柵
        for (int y = 0; y < 7; y++)
        {
            fence2_type1->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence2_type1->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) });
            fence2_type1->Render(scale, replaced_pixel_shader);
            if (y == 6)
            {
                fence_end->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) - 2.5f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(0.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        for (int x = 0; x < 7; x++)
        {
            fence_type1->GetTransform()->SetPosition({ -8.0f + (x * 2.5f),dec_y,-9.0f });
            fence_type1->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
            fence_type1->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence_type1->Render(scale, replaced_pixel_shader);

            if (x == 6)
            {
                fence_end->GetTransform()->SetPosition({ -8.0f + (x * 2.5f) + 2.5f,dec_y,-9.0f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        fence_type1->Render(scale, replaced_pixel_shader);
        break;
    case 2:
        // 柵
        for (int y = 0; y < 7; y++)
        {
            fence2_type2->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence2_type2->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) });
            fence2_type2->Render(scale, replaced_pixel_shader);
            if (y == 6)
            {
                fence_end->GetTransform()->SetPosition({ -9.0f,dec_y,8.0f - (y * 2.5f) - 2.5f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(0.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        for (int x = 0; x < 7; x++)
        {
            fence_type2->GetTransform()->SetPosition({ -8.0f + (x * 2.5f),dec_y,-9.0f });
            fence_type2->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
            fence_type2->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
            fence_type2->Render(scale, replaced_pixel_shader);

            if (x == 6)
            {
                fence_end->GetTransform()->SetPosition({ -8.0f + (x * 2.5f) + 2.5f,dec_y,-9.0f });
                fence_end->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-90.0f));
                fence_end->GetTransform()->SetRotationZ(DirectX::XMConvertToRadians(20.0f));
                fence_end->Render(scale, replaced_pixel_shader);
            }
        }
        fence_type2->Render(scale, replaced_pixel_shader);
        break;

    }
    // その他
    // 井戸
    ido->Render(scale, replaced_pixel_shader);
    ido_yane->Render(scale, replaced_pixel_shader);

    // 岩のオブジェ
    cave->Render(scale, replaced_pixel_shader);

    // 城
    silo->Render(scale, replaced_pixel_shader);

    // 木
    for (int i = 0; i < 7; i++)
    {
        tree[i]->Render(scale, replaced_pixel_shader);
        // 草
        leavs_LV1[i]->Render(scale, replaced_pixel_shader);
        leavs_LV2[i]->Render(scale, replaced_pixel_shader);
    }
    for (int i = 0; i < 3; i++)
    {
        une[i]->Render(scale, replaced_pixel_shader);
    }

    log->Render(scale, replaced_pixel_shader);
}

void StageMain::DrawDebugGui()
{
    std::string name = "StageMain";
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        ImGui::SliderFloat2("stage_collision_l", &stage_collision.left_up.x, 15.0f, -15.0f);
        ImGui::SliderFloat2("stage_collision_r", &stage_collision.right_down.x, 15.0f, -15.0f);

        std::string de = std::string("death") + name;
        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scale_facter, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}