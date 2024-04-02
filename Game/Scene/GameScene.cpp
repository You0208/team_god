#include "GameScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"
// Effect
#include "./Lemur/Effekseer/EffekseerManager.h"

// Game
#include "../Stage/StageMain.h"
#include "../Stage/StageManager.h"
#include "../Character/EnemyManager.h"
#include "../Character/Enemy_A.h"
#include "../Character/UnitManager.h"
#include "../Character/Unit_A.h"
#include "../Character/SeedManager.h"

void GameScene::Initialize()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// �V�F�[�_�[�֘A
	{
		// �X�e�[�g�̏�����
		InitializeState();
		// �f�B�t�@�[�h�����_�����O�̏�����
		InitializeDeffered(SCREEN_WIDTH, SCREEN_HEIGHT);
		// �t���[���o�b�t�@�[�̏�����
		InitializeFramebuffer();
		// �s�N�Z���V�F�[�_�[�̏�����
		InitializePS();
		// �|�C���g���C�g�E�X�|�b�g���C�g�̏����ʒu�ݒ�
		InitializeLight();

		// �X�J�C�}�b�v�e�N�X�`���̃��[�h
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		double_speed_z = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO �����p
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/differd_model_ps.cso", defefferd_model.GetAddressOf());
	}
	// �Q�[������
	{
		// �J����
		Camera& camera = Camera::Instance();
		// �J��������
		camera_range = 20.0f;

		// �X�e�[�W������
		StageManager& stage_manager = StageManager::Instance();
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// �v���C���[�̏�����
		player = new Player;

		// �v���C���[�̏�����
		//UnitManager& unitManager = UnitManager::Instance();
		//Unit_A* unit_A = new Unit_A();
		//unit_A->SetPosition(DirectX::XMFLOAT3(2.0f, 0, 5));
		//unitManager.Register(unit_A);		

		// �G�l�~�[�̏�����
		//EnemyManager& enemyManager = EnemyManager::Instance();
		//for (int i = 0; i < 2; ++i)
		//{
		//	Enemy_A* enemy_A = new Enemy_A();
		//	enemy_A->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		//	enemyManager.Register(enemy_A);
		//}


		ohajiki = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\���͂���.png");
	}

}

void GameScene::Finalize()
{
	//�X�e�[�W�I��
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	//�v���C���[�I��
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	// �G�l�~�[�I��
	EnemyManager::Instance().Clear();
	// ���j�b�g�I��
	UnitManager::Instance().Clear();
	// ��I��
	SeedManager::Instance().Clear();
}

void GameScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();

	// �J����
	{
		camera.Update(elapsedTime);
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
	}

	// ���C�g�̍X�V
	LightUpdate();

	// �Q�[��
	{
		s_l = gamePad.GetAxisLY() * -1.0f;

		if (s_l > 0.1f)
		{
			timer_s += elapsedTime;
			if (s_l_max <= s_l)s_l_max = s_l;
		}
		else
		{
			if (timer_s > 0)f_d = (s_l_max * 100.0f) / timer_s;
			s_l_max = 0;
			timer_s = 0;
		}

		// �v���C���[�̍X�V
		player->Update(elapsedTime);
		// �G�t�F�N�g�X�V����
		EffectManager::Instance().Update(elapsedTime);
		//�X�e�[�W�X�V����
		StageManager::Instance().update(elapsedTime);
		EnemyManager::Instance().Update(elapsedTime);
		UnitManager::Instance().Update(elapsedTime);
		// ��X�V
		SeedManager::Instance().Update(elapsedTime);
	}

	// Imgui
	DebugImgui();
}

void GameScene::Render(float elapsedTime)
{
	HRESULT hr{ S_OK };

	Camera& camera = Camera::Instance();
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
	ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

	camera.SetPerspectiveFov(immediate_context);
	
	// �`��̐ݒ�
	SetUpRendering();

	// �G�b�W���o
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		//EnemyManager::Instance().Render(0.1f, null_pixel_shader);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Deactivate(immediate_context);
	}

	// �f�B�t�@�[�h�����_�����O�̐ݒ�
	SetUpDeffered();

	//�X�e�[�g�̐ݒ�
	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// �e�N�X�`�����Z�b�g
	{
		// �m�C�Y
		immediate_context->PSSetShaderResources(9/*slot(1�ԂɃZ�b�g���܂�)*/, 1, noise.GetAddressOf());//TODO
		// �V���h�E
		immediate_context->PSSetShaderResources(8, 1, double_speed_z->shader_resource_view.GetAddressOf());
		//�@�[�x�l
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
	}

	// �|�X�g�G�t�F�N�g�̊J�n
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D�`��
	{
		const float scale = 0.01f;
		if (enable_deferred)
		{
			// �v���C���[�`��
			player->Render(scale, defefferd_model.GetAddressOf());
			//�X�e�[�W�`��
			StageManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// ���j�b�g�`��
			UnitManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// �G�l�~�[�`��
			EnemyManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// ��`��
			SeedManager::Instance().Render(scale, defefferd_model.GetAddressOf());
		}
		else
		{
			// �v���C���[�`��
			player->Render(scale, stage_ps.GetAddressOf());
			//�X�e�[�W�`��
			StageManager::Instance().Render(scale, stage_ps.GetAddressOf());
			// ���j�b�g�`��
			UnitManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// �G�l�~�[�`��
			EnemyManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// ��`��
			SeedManager::Instance().Render(scale, chara_ps.GetAddressOf());
		}
	}

	// �X�e�[�g�̍Đݒ�
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	// �o�b�t�@�[�̕ύX
	RenderingDeffered();

	// �|�X�g�G�t�F�N�g�̎��s
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}
}

void GameScene::DebugImgui()
{
	BaseScene::DebugImgui();

	ImGui::Begin("ImGUI");

	// STATIC_BATCHING
	if (ImGui::TreeNode("shadow"))
	{
		ImGui::Image(reinterpret_cast<void*>(double_speed_z->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
		ImGui::SliderFloat("shadow_depth_bias", &scene_constants.shadow_depth_bias, 0.1f, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
}
