#include "GameScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Scene/ResultScene.h"
#include "./high_resolution_timer.h"

// Effect
#include "./Lemur/Effekseer/EffekseerManager.h"

// Game
#include "Game/CollisionManager.h"
#include "../Stage/StageMain.h"
#include "../Stage/StageManager.h"
#include "../Character/EnemyManager.h"
#include "../Character/Enemy_ABCD.h"
#include "../Character/UnitManager.h"
#include "../Character/Unit_ABC.h"
#include "../Character/Unit_DEF.h"
#include "../Character/SeedManager.h"
#include "../Character/EnemySpawner.h"

#include "Lemur/Graphics/DebugRenderer.h"

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
		camera_range = 30.0f;


		StageManager& stage_manager = StageManager::Instance();
		stage_manager.SetStageLevel(1);


		//TODO ���� �������Ԃ̏�����
		switch (StageManager::Instance().GetStageLevel())
		{
		case 0:// ���x���P
			time_limit = 60.0f;// �����Ő������Ԃ𒲐�
			break;
		case 1:// ���x���Q
			time_limit = 200.0f;// �����Ő������Ԃ𒲐�
			break;
		case 2:// ���x���R
			time_limit = 200.0f;// �����Ő������Ԃ𒲐�
			break;
		case 3:// ���x���S
			time_limit = 200.0f;// �����Ő������Ԃ𒲐�
			break;
		case 4:// ���x���T
			time_limit = 200.0f;// �����Ő������Ԃ𒲐�
			break;
		}
		timer = 0.0f;
		
		// �X�e�[�W������
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// �v���C���[�̏�����
		player = new Player;

		fence = new Fence;

		// �G�X�|�[�����䑕�u�̏�����
		EnemySpawner::Instance().Initialize();
		// ���j�b�g�}�l�[�W���[�̏�����
		UnitManager::Instance().Initialize();

		ohajiki = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\���͂���.png");
	}

	// �f�o�b�O
	{
		Try_T.B.y = 2.0;
		Try_T.C.x = 2.0;
		Try_T.C.y = 1.0;

		r0 = 0.1f;
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
	//�v���C���[�I��
	if (fence != nullptr)
	{
		delete fence;
		fence = nullptr;
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
	//LightUpdate();

	// �Q�[��
	{
		// �^�C�}�[
		timer += elapsedTime;

		if (timer >= time_limit)
		{
			Lemur::Scene::SceneManager::Instance().ChangeScene(new ResultScene);
		}


		// �v���C���[�̍X�V
		player->Update(elapsedTime);
		// ��̍X�V
		fence->Update(elapsedTime);
		// �G�t�F�N�g�X�V����
		EffectManager::Instance().Update(elapsedTime);
		//�X�e�[�W�X�V����
		StageManager::Instance().update(elapsedTime);
		EnemyManager::Instance().Update(elapsedTime);
		UnitManager::Instance().Update(elapsedTime);
		// ��X�V
		SeedManager::Instance().Update(elapsedTime);

		// ��ƃ��j�b�g�̓����蔻��i�킪���܂�邩�j
		CollisionManager::Instance().CollisionSeedVsUnit();

		// �X�|�[��
		EnemySpawner::Instance().Update(elapsedTime);
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
			// ��`��
			fence->Render(scale, defefferd_model.GetAddressOf());
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
			// ��`��
			fence->Render(scale, stage_ps.GetAddressOf());
			//�X�e�[�W�`��
			StageManager::Instance().Render(1.0f, stage_ps.GetAddressOf());
			// ���j�b�g�`��
			UnitManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// �G�l�~�[�`��
			EnemyManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// ��`��
			SeedManager::Instance().Render(scale, chara_ps.GetAddressOf());
		}
	}

	//TODO debug
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);
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
	ImGui::Begin(u8"�Q�[��");
	float t = time_limit - timer;
	ImGui::DragFloat(u8"�c�莞��", &t, 0.0f, 0.1f);
	ImGui::SliderFloat(u8"��������", &time_limit, 0.0f, 600.0f);
	ImGui::End();

	BaseScene::DebugImgui();

}
