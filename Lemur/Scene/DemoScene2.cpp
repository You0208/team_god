#include "DemoScene2.h"
#include "../Graphics/Camera.h"

void DemoScene2::Initialize()
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

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/skinned_mesh_ps.cso", Try.GetAddressOf());

	}

	// �Q�[������
	{
		Camera& camera = Camera::Instance();

		test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\scarecrow_Re.fbx");
	}
}

void DemoScene2::Finalize()
{
}

void DemoScene2::Update(HWND hwnd, float elapsedTime)
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

}

void DemoScene2::Render(float elapsedTime)
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

	// �G�b�W���o
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		test_model->Render(0.01f, null_pixel_shader);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Deactivate(immediate_context);
	}

	// �f�B�t�@�[�h�����_�����O�̐ݒ�
	SetUpDeffered();

	//�X�e�[�g�̐ݒ�
	if (enable_deferred)immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	else immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// �e�N�X�`�����Z�b�g
	{
		// �m�C�Y
		immediate_context->PSSetShaderResources(9/*slot(1�ԂɃZ�b�g���܂�)*/, 1, noise.GetAddressOf());
		// �V���h�E
		immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
		//�@�[�x�l
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
		// PROJECTION_MAPPING
		immediate_context->PSSetShaderResources(15, 1, projection_mapping_texture.GetAddressOf());
	}

	// �|�X�g�G�t�F�N�g�̊J�n
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D�`��
	if (enable_deferred)
	{
		//shadow_model->Render(0.01f, fbx_gbuffer_ps.Get());
		//test_model_2->Render(0.1f, fbx_gbuffer_ps.Get());
		//shadow_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test_2");

		//gltf_test_model->Render(1.0f, gltf_gbuffer_ps.Get());
		//gltf_test_model_2->Render(1.0f, gltf_gbuffer_ps.Get());
	}
	else
	{
		//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
		test_model->Render(0.02f, Try.Get());
		//test_model_2->Render(0.1f, Try.Get());
		//shadow_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test");
		//gltf_test_model->GetTransform()->SetPositionY(1.0f);
		//gltf_test_model->Render(1.0f, gltf_ps.Get());
		//gltf_test_model_2->Render(100.0f, gltf_ps.Get());

		//for (int x = 0; x < 3; x++)
		//{
		//	for (int y = 0; y < 6; y++)
		//	{
		//		if (y % 2 == 0)
		//		{
		//			fild2->GetTransform()->SetPosition({ -7.4f + (x * 2 * 3.0f),0.5f,-7.4f + (y * 3.0f) });
		//			fild1->GetTransform()->SetPosition({ -4.4f + (x * 2 * 3.0f),0.5f,-7.4f + (y * 3.0f) });
		//			fild1->Render(1.0f, Try.Get());
		//			fild2->Render(1.0f, Try.Get());
		//		}
		//		else
		//		{
		//			fild1->GetTransform()->SetPosition({ -7.4f + (x * 2 * 3.0f),0.5f,-7.4f + (y * 3.0f) });
		//			fild2->GetTransform()->SetPosition({ -4.4f + (x * 2 * 3.0f),0.5f,-7.4f + (y * 3.0f) });
		//			fild1->Render(1.0f, Try.Get());
		//			fild2->Render(1.0f, Try.Get());
		//		}
		//	}
		//}
	}

	//// �X�e�[�g�̐ݒ�
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	//result->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, true);
	////game_over_back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0.0f, SCREEN_WIDTH * 7, SCREEN_HEIGHT*3, SCREEN_WIDTH, SCREEN_HEIGHT);
	//
	RenderingDeffered();

	// �|�X�g�G�t�F�N�g�̎��s
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	// �p�[�e�B�N���`��
	{
		// �u�����h�X�e�[�g�ݒ�
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�ݒ�
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);
		// ���X�^���C�U�[�X�e�[�g�ݒ�
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		if (particle_system)	particle_system->Render(immediate_context);
		if (particle_bomb)	particle_bomb->Render(immediate_context);
	}
	//shadow_model->Render(0.01f, fbx_gbuffer_ps.Get());
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
	//gltf_test_model->Render(1.0f, gltf_ps.Get());
}
