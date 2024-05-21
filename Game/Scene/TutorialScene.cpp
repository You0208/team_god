#include "TutorialScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Scene/OverScene.h"
#include "Game/Scene/LoadingScene.h"
#include "./high_resolution_timer.h"

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
#include "../Character/UniqueEnemy.h"
#include "../Scene/SelectScene.h"


void TutorialScene::Initialize()
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

		// �}�X�N�̏�����
		InitializeMask();

		// �X�J�C�}�b�v�e�N�X�`���̃��[�h
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO �����p
		create_ps_from_cso(graphics.GetDevice(), "./Shader/sprite_ui.cso", sprite_ui.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_1.cso", stage_ps_1.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_2.cso", stage_ps_2.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/unit_ps.cso", unit_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/scarecrow.cso", scarecrow_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/enemy_ps.cso", enemy_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps.cso", collision.GetAddressOf());

	}
	// �X�v���C�g�ǂݍ���
	{
		button_ui_base = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\button_UI.png");
		button_ui_chara = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_sheet.png");

		pause_main = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Pause_kakasi.png");
		pause_text_continue = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Continue.png");
		pause_text_select = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Stageselect.png");

		tutorial_glf[0] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\1.png");
		tutorial_glf[1] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\2.png");
		tutorial_glf[2] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\3.png");
		tutorial_glf[3] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\4.png");
		tutorial_glf[4] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\5.png");
		tutorial_glf[5] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\6.png");
		tutorial_glf[6] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\7.png");
		tutorial_glf[7] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\8.png");
	}
	// �Q�[������
	{
		// �J����
		Camera& camera = Camera::Instance();
		camera_range = 33.0f;
		camera_angle = { DirectX::XMConvertToRadians(40),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera_target = { -7.0f,0.0f,0.0f };
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
		camera.SetEyeYOffset(8.0f);
		camera.SetAngle(camera_angle);

		StageManager& stage_manager = StageManager::Instance();

		// ���C�e�B���O�A�F���␳
		// ��
		directional_light_direction = { -0.342f,-1.00f,0.0f,0.0f };
		option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
		option_constant.rgb_adjustment = { 1.1f,1.0f,1.0f,1.0f };
		option_constant.parameters.y = 0.5f;

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
		// �G�l�~�[�}�l�[�W���[�̏�����
		EnemyManager::Instance().Initialize();

		// �|�[�Y���
		is_pause = false;
		is_bloom = true;
		pause_text_continue_scale.value = 1.0f;
		pause_text_select_scale.value = 1.0f;

		// �`���[�g���A��
		tutorial_state = 0;
		tutorial_gif_num = 0;

		// �A�C���X�A�E�g���Ă�
		CallTransition(false);
	}
}

void TutorialScene::Finalize()
{
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

void TutorialScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	// �J����
	{
		camera.Update(elapsedTime);
	}

	// ���C�g�̍X�V
	LightUpdate();

	// �}�X�N�̍X�V
	TransitionMask(elapsedTime);

	if (gamePad.GetButtonUp() & gamePad.BTN_BACK)
	{
		is_pause = !is_pause;
		pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
	}
	if (is_pause)
	{
		PauseUpdate(elapsedTime);
		return;
	}
	else is_bloom = true;

	// �J�ډ��o�̍Œ��͂����艺�ɍs���Ȃ�
	if (start_transition)return;

	// �Q�[��
	{
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

		if (gamePad.GetButtonUp() & gamePad.BTN_RIGHT)
		{
			if (tutorial_gif_num < 7)tutorial_gif_num++;
		}
		if (gamePad.GetButtonUp() & gamePad.BTN_LEFT)
		{
			if (tutorial_gif_num > 0)tutorial_gif_num--;
		}


		if (gamePad.GetButtonUp() & gamePad.BTN_LEFT_SHOULDER || gamePad.GetButtonUp() & gamePad.BTN_RIGHT_SHOULDER)
		{
			UnitManager::Instance().Clear();
			SeedManager::Instance().Clear();
		}

		if (EnemyManager::Instance().GetEnemyCount() <= 0&& tutorial_state!= MISSION::attack)
		{
			Enemy* new_enemy = new NuisanceEnemy(false);
			new_enemy->SetHealth(1);
			EnemyManager::Instance().Register(new_enemy);
		}

		switch (tutorial_state)
		{
		case MISSION::throw_seed:// ����΂�
			if (SeedManager::Instance().GetSeedCount() > 0)
			{
				tutorial_state = MISSION::move_player;
				player_current_position = player->GetPosition();
			}
			break;
		case MISSION::move_player:// �������𓮂���
			if (!Equal(player_current_position.x, player->GetPosition().x) || !Equal(player_current_position.z, player->GetPosition().z))
			{
				unit_category = player->GetCategory();
				tutorial_state = MISSION::change_unit;
			}
			break;
		case MISSION::change_unit:// ���j�b�g��ς���
			if (player->GetCategory() != unit_category)
			{
				tutorial_state = MISSION::attack;
			}
			break;
		case MISSION::attack:// �U������
			if (EnemyManager::Instance().GetEnemyCount() <= 0)
			{
				tutorial_state = MISSION::clear;
			}
			break;
		case MISSION::clear:

			break;
		}
	}

	// Imgui
	DebugImgui();
}

void TutorialScene::Render(float elapsedTime)
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
	SetUpConstantBuffer();

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// �X�P�[��
	const float scale = 0.015f;
	// TODO ���ˁ@���˂݁[�̑傫��
	const float enemy_scale = 0.016f;

	SetUpShadowMap();

	shadow_map->Clear(immediate_context);
	shadow_map->Activate(immediate_context);

	ID3D11PixelShader* null_pixel_shader{ NULL };
	// �v���C���[�`��
	player->Render(scale, &null_pixel_shader);

	//// �X�e�[�W�`��
	StageManager::Instance().Render(1.0f, &null_pixel_shader, &null_pixel_shader);
	// ���j�b�g�`��
	UnitManager::Instance().Render(scale, null_pixel_shader);
	for (int i = 0; i < UnitManager::Instance().GetUnitCount(); i++)
	{
		UnitManager::Instance().GetUnit(i)->collision_model->Render(scale, null_pixel_shader);
	}
	// �G�l�~�[�`��
	EnemyManager::Instance().Render(enemy_scale, null_pixel_shader);
	// ��`��
	SeedManager::Instance().Render(0.1f, null_pixel_shader);

	shadow_map->Deactivate(immediate_context);

	SetUpConstantBuffer();
	// �G�b�W���o
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		player->Render(0.025f, &null_pixel_shader);
		UnitManager::Instance().CollisionRender(scale, null_pixel_shader);
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
		immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
		//�@�[�x�l
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
	}
	// �|�X�g�G�t�F�N�g�̊J�n
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context, 0.2f, 0.3f, 0.5f, 1.0f);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D�`��
	{
		if (enable_deferred)
		{
			// �v���C���[�`��
			player->Render(scale, fbx_gbuffer_ps.GetAddressOf());
			//�X�e�[�W�`��
			//StageManager::Instance().Render(1.0f, fbx_gbuffer_ps.GetAddressOf());
			// ���j�b�g�`��
			UnitManager::Instance().Render(scale, fbx_gbuffer_ps.Get());
			// �G�l�~�[�`��
			EnemyManager::Instance().Render(enemy_scale, fbx_gbuffer_ps.Get());
			// ��`��
			SeedManager::Instance().Render(scale, fbx_gbuffer_ps.Get());
		}
		else
		{
			// �v���C���[�`��
			player->Render(0.025f, scarecrow_ps.GetAddressOf());

			// ��`��
			//fence->Render(scale, Try.GetAddressOf());

			// �X�e�[�W�`��
			StageManager::Instance().Render(1.0f, stage_ps_1.GetAddressOf(), stage_ps_2.GetAddressOf());
			// ���j�b�g�`��
			UnitManager::Instance().Render(scale, unit_ps.Get());
			UnitManager::Instance().CollisionRender(scale, collision.Get());
			// �G�l�~�[�`��
			EnemyManager::Instance().Render(enemy_scale, enemy_ps.Get());
			// ��`��
			SeedManager::Instance().Render(0.1f, enemy_ps.Get());
		}
	}

	// �X�e�[�g�̍Đݒ�
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	// �o�b�t�@�[�̕ύX
	RenderingDeffered();

	// �G�t�F�N�g
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		//TODO ���ˁ@�����蔻��
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);

		// �G�t�F�N�g�Đ�
		EffectManager::Instance().Render(view, projection);
	}

	// �|�X�g�G�t�F�N�g�̎��s
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	if (is_bloom)
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

		GamePad& gamePad = Input::Instance().GetGamePad();
		// 2D�`��
		button_ui_base->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A], SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B], SCREEN_HEIGHT * 1, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X], SCREEN_HEIGHT * 2, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y], SCREEN_HEIGHT * 3, SCREEN_WIDTH, SCREEN_HEIGHT);
	
		tutorial_glf[tutorial_gif_num]->Animation(immediate_context, elapsedTime, 0.05f, { 40,10 }, { 600 ,450 }, { 1,1,1,1 }, 0.0f, { 600 ,450 }, tutorial_glf_num_x[tutorial_gif_num]);
	}
	else
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

		pause_main->Render(immediate_context, SCREEN_WIDTH - 820, 0, 820, SCREEN_HEIGHT);
		pause_text_continue->RenderCenter(immediate_context, 1520, 550, 500 * pause_text_continue_scale.value, 120 * pause_text_continue_scale.value);
		pause_text_select->RenderCenter(immediate_context, 1520, 700, 500 * pause_text_select_scale.value, 120 * pause_text_select_scale.value);

	}
	// �}�X�N�̕`��
	RenderTransitionMask(elapsedTime);
}

void TutorialScene::DebugImgui()
{
	ImGui::Begin("tutorial_state");
	switch (tutorial_state)
	{
	case MISSION::throw_seed:
		ImGui::Text("throw_seed");
		break;
	case MISSION::change_unit:
		ImGui::Text("change_unit");
		break;
	case MISSION::move_player:
		ImGui::Text("move_player");
		break;
	case MISSION::clear:
		ImGui::Text("clear");
		break;
	case MISSION::attack:
		ImGui::Text("attack");
		break;
	}
	ImGui::End();
	BaseScene::DebugImgui();
	Camera::Instance().DrawDebug();
}

void TutorialScene::InitializeLight()
{
	point_light[0].position.x = 0;
	point_light[0].position.y = 50;
	point_light[0].range = 1000;
	point_light[0].color = { 1, 1, 1, 1 };
	point_light[1].position.x = 0;
	point_light[1].position.y = 10;
	point_light[1].range = 1000;
	point_light[1].color = { 1, 1, 1, 1 };
	ZeroMemory(&point_light[8], sizeof(pointLights) * 8);
	ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);
}

void TutorialScene::PauseUpdate(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	is_bloom = false;

	switch (pause_num)
	{
	case 0:
		pause_text_continue_scale.ContinueEasing(elapsedTime);
		pause_text_select_scale.EasingValue(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
		{
			pause_text_select_scale.CallValueContinue(1.0f, 1.1f, pause_text_select_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_continue_scale.CallValueEasing(1.0f, pause_text_continue_scale.value, EasingFunction::EasingType::InSine);
			pause_num++;
		}
		else if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			is_bloom = true;
			pause_num = 0;
			is_pause = false;
		}
		break;
	case 1:
		pause_text_select_scale.ContinueEasing(elapsedTime);
		pause_text_continue_scale.EasingValue(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_UP)
		{
			pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_select_scale.CallValueEasing(1.0f, pause_text_select_scale.value, EasingFunction::EasingType::InSine);
			pause_num--;
		}
		else if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			is_bloom = true;
			pause_num = 0;
			Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
		}
		break;
	}
}
