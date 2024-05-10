#include "BaseScene.h"
#include "../Graphics/Camera.h"
#include "../Math/Easing.h"

void Lemur::Scene::BaseScene::DebugImgui()
{
	ImGui::Begin("Shader");

	ImGui::SliderFloat4("parameters", &option_constant.parameters.x, 0.0f, +1.0f);

	ImGui::Separator();
	ImGui::ColorEdit3("sky_color", &sky_color.x);
	ImGui::ColorEdit3("ground_color", &ground_color.x);
	ImGui::SliderFloat("hemisphere_weight", &hemisphere_weight, 0.0f, 1.0f);

	if (ImGui::TreeNode("enable_shader"))
	{
		ImGui::Checkbox("enableShadow", &enable_shadow);
		ImGui::Checkbox("enableSkyMap", &enable_skymap);
		ImGui::Checkbox("enableBloom", &enable_bloom);
		ImGui::Checkbox("enableFog", &enable_fog);
		ImGui::Checkbox("enableDeferred", &enable_deferred);
		if (enable_deferred)ImGui::Checkbox("enableDeferredPost", &enable_deferred_post);
		if(!enable_deferred)ImGui::Checkbox("enablePostEffect", &enable_post_effect);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("camera"))
	{
		ImGui::SliderFloat3("camera_target", &camera_target.x, -10.0f, +10.0f);
		ImGui::SliderFloat("camera_range", &camera_range, 0.0f, +100.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("dis_fog"))
	{
		ImGui::ColorEdit3("fog_color", &fog_color.x);
		ImGui::SliderFloat("fog_near", &fog_range.x, 0.1f, +100.0f);
		ImGui::SliderFloat("fog_far", &fog_range.y, 0.1f, +100.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("fog"))
	{
		ImGui::ColorEdit4("fog_color", fog_constants.fog_color);
		ImGui::SliderFloat("fog_density", &fog_constants.fog_density, +0.0f, +0.005f, "%.4f");
		ImGui::SliderFloat("fog_height_falloff", &fog_constants.fog_height_falloff, +0.0001f, +10.0f, "%.4f");
		ImGui::SliderFloat("fog_cutoff_distance", &fog_constants.fog_cutoff_distance, +0.0f, +1000.0f, "%.4f");
		ImGui::SliderFloat("start_distance", &fog_constants.start_distance, +0.0f, +100.0f, "%.4f");
		ImGui::SliderFloat("time_scale", &fog_constants.time_scale, +0.0f, +10.0f, "%.4f");
		ImGui::SliderFloat("seed_scale", &fog_constants.seed_scale, +0.0f, +0.5f, "%.4f");
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("pbr"))
	{
		ImGui::SliderFloat("adjustMetalness", &pbr.adjustMetalness.x, -1.0f, +1.0f);
		ImGui::SliderFloat("adjustSmoothness", &pbr.adjustSmoothness.x, 0.0f, +1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("light"))
	{
		ImGui::SliderFloat3("directional_light_direction", &directional_light_direction.x, -1.0f, +1.0f);
		ImGui::ColorEdit3("directional_light_color", &directional_light_color.x);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("bloom"))
	{
		ImGui::SliderFloat("bloom_extraction_threshold", &bloomer->bloom_extraction_threshold, +0.0f, +5.0f);
		ImGui::SliderFloat("bloom_intensity", &bloomer->bloom_intensity, +0.0f, +5.0f);
		ImGui::TreePop();
	}
	// �|�C���g���C�g
	if (ImGui::TreeNode("points"))
	{
		for (int i = 0; i < 8; ++i)
		{
			std::string p = std::string("position") + std::to_string(i);
			ImGui::SliderFloat3(p.c_str(), &point_light[i].position.x, -10.0f, +10.0f);
			std::string c = std::string("color") + std::to_string(i);
			ImGui::ColorEdit3(c.c_str(), &point_light[i].color.x);
			std::string r = std::string("range") + std::to_string(i);
			ImGui::SliderFloat(r.c_str(), &point_light[i].range, 0.0f, +100.0f);
		}
		ImGui::TreePop();
	}
	// �X�|�b�g���C�g
	if (ImGui::TreeNode("spots"))
	{
		for (int i = 0; i < 8; ++i)
		{
			std::string p = std::string("position") + std::to_string(i);
			ImGui::SliderFloat3(p.c_str(), &spot_light[i].position.x, -10.0f, +10.0f);
			std::string d = std::string("direction") + std::to_string(i);
			ImGui::SliderFloat3(d.c_str(), &spot_light[i].direction.x, 0.0f, +1.0f);
			std::string c = std::string("color") + std::to_string(i);
			ImGui::ColorEdit3(c.c_str(), &spot_light[i].color.x);
			std::string r = std::string("range") + std::to_string(i);
			ImGui::SliderFloat(r.c_str(), &spot_light[i].range, 0.0f, +1000.0f);
			std::string ic = std::string("angle") + std::to_string(i);
			ImGui::SliderFloat(ic.c_str(), &sp_angle, 0.0f, +1000.0);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("texture"))
	{
		// GBuffer���m�F����
		static constexpr char* GBufferNames[]
		{
			"color",
			"normal",
			"position",
			"metalSmooth",
			"emissive",
			"oculsion",
		};
		for (int i = 0; i < G_MAX; ++i)
		{
			ImGui::Text(GBufferNames[i]);
			ImGui::Image(shaderResourceViewArray[i].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1,1, 1 });
		}
		ImGui::TreePop();
	}

	ImGui::End();
}


void Lemur::Scene::BaseScene::InitializeState()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	HRESULT hr{ S_OK };

	// �[�x�X�e���V���X�e�[�g�I�u�W�F�N�g���쐬����
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = graphics.GetDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = graphics.GetDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = graphics.GetDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = graphics.GetDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// �u�����f�B���O�X�e�[�g�I�u�W�F�N�g���쐬����
	{
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = FALSE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = graphics.GetDevice()->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = graphics.GetDevice()->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		//TODO �f�B�t�@�[�h�����_�����O�p�ɁiG-Buffer�̖����𑝂₵���ꍇ�͂��������₷�j
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = TRUE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		for (int i = 1; i < G_MAX; i++)
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = TRUE;
			blend_desc.RenderTarget[i].BlendEnable = FALSE;
			blend_desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		hr = graphics.GetDevice()->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = graphics.GetDevice()->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::ADD)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = graphics.GetDevice()->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::MULTIPLY)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// ���X�^���C�U�X�e�[�g�I�u�W�F�N�g�𐶐�(�O�̃X�e�[�W�ŏ������ꂽ�_��O�p�`�Ȃǂ���ʂ̃s�N�Z���ɕϊ�����)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;// �w���Ȃ����
		// ����n�E��n�ł�����ς���
		rasterizer_desc.FrontCounterClockwise = TRUE;// �����v���iCCW�j��O�ʂƂ���iCW�����v���j
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = graphics.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = graphics.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = graphics.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = graphics.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::WIREFRAME_CULL_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// �T���v���[�X�e�[�g�I�u�W�F�N�g�𐶐�����
	{
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// �e�N�X�`���̃T���v�����O���Ɏg�p����t�B���^�[�������@
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;// 0 ���� 1 �͈̔͊O�� u �e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;// 0 ���� 1 �͈̔͊O�� v �e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;// 0 ���� 1 �͈̔͊O�� w �e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
		sampler_desc.MipLODBias = 0;// �v�Z���ꂽ�~�b�v�}�b�v ���x������̃I�t�Z�b�g
		sampler_desc.MaxAnisotropy = 16;// �L���Ȓl�� 1 �` 16
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;// �T���v�����O���ꂽ�f�[�^�������̃T���v�����O���ꂽ�f�[�^�Ɣ�r����֐�
		sampler_desc.BorderColor[0] = 0;// ���E���̐F
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;// �A�N�Z�X���N�����v����~�b�v�}�b�v�͈͂̉��[(0�������΂�ڍ�)
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;// �A�N�Z�X���N�����v����~�b�v�}�b�v�͈͂̏��
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.BorderColor[0] = 1;
		sampler_desc.BorderColor[1] = 1;
		sampler_desc.BorderColor[2] = 1;
		sampler_desc.BorderColor[3] = 1;
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SHADOW
		sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //D3D11_COMPARISON_LESS_EQUAL
		sampler_desc.BorderColor[0] = 1;
		sampler_desc.BorderColor[1] = 1;
		sampler_desc.BorderColor[2] = 1;
		sampler_desc.BorderColor[3] = 1;
		hr = graphics.GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::COMPARISON_LINEAR_BORDER_WHITE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		HRESULT hr{ S_OK };
		// �V�[���萔�o�b�t�@�I�u�W�F�N�g�𐶐�
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;// �ǂݎ�肨��я������݂��ǂ̂悤�ɍs���邩
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@���p�C�v���C���ɂǂ̂悤�Ƀo�C���h(���т�)����邩�����
			buffer_desc.CPUAccessFlags = 0;// CPU �A�N�Z�X �t���O�iCPU �A�N�Z�X���K�v�Ȃ��ꍇ�� 0�j
			buffer_desc.MiscFlags = 0;// ���̑��̃t���O�i���g�p�ɏꍇ��0�j
			buffer_desc.StructureByteStride = 0;//�o�b�t�@���\�����o�b�t�@��\���ꍇ�́A�o�b�t�@�\�����̊e�v�f�̃T�C�Y
			{
				buffer_desc.ByteWidth = sizeof(scene_constant);// �o�b�t�@�̃T�C�Y 
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // �t�H�O
				buffer_desc.ByteWidth = sizeof(fog_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // �I�v�V����
				buffer_desc.ByteWidth = sizeof(option_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // �����t�H�O
				buffer_desc.ByteWidth = sizeof(dis_fog_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // PBR
				buffer_desc.ByteWidth = sizeof(adjust_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // ���C�g
				buffer_desc.ByteWidth = sizeof(lightConstants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, light_constant_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{
				// �������C�g
				buffer_desc.ByteWidth = sizeof(hemisphere_light_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, hemisphere_light_constant_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
	}
}

void Lemur::Scene::BaseScene::InitializeDeffered(int textureWidth, int textureHeight)
{
	HRESULT hr{ S_OK };

	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// GBuffer����
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		DXGI_FORMAT formats[G_MAX] =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		};
		for (int i = 0; i < G_MAX; ++i)
		{
			texture2d_desc.Format = formats[i];
			Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};
			hr = graphics.GetDevice()->CreateTexture2D(&texture2d_desc, NULL, color_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// �����_�[�^�[�Q�b�g�r���[����
			hr = graphics.GetDevice()->CreateRenderTargetView(color_buffer.Get(), NULL,
				renderTargetViewArray[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// �V�F�[�_�[���\�[�X�r���[����
			hr = graphics.GetDevice()->CreateShaderResourceView(color_buffer.Get(), NULL,
				shaderResourceViewArray[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}


}

void Lemur::Scene::BaseScene::InitializeLight()
{
	point_light[0].position.x = 10;
	point_light[0].position.y = 1;
	point_light[0].range = 10;
	point_light[0].color = { 1, 0, 0, 1 };
	point_light[1].position.x = -10;
	point_light[1].position.y = 1;
	point_light[1].range = 10;
	point_light[1].color = { 0, 1, 0, 1 };
	point_light[2].position.y = 1;
	point_light[2].position.z = 10;
	point_light[2].range = 10;
	point_light[2].position.y = 1;
	point_light[2].color = { 0, 0, 1, 1 };
	point_light[3].position.y = 1;
	point_light[3].position.z = -10;
	point_light[3].range = 10;
	point_light[3].color = { 1, 1, 1, 1 };
	point_light[4].range = 10;
	point_light[4].color = { 1, 1, 1, 1 };
	ZeroMemory(&point_light[8], sizeof(pointLights) * 8);

	spot_light[0].position = { 0, 10, 0, 0 };
	spot_light[0].direction = { -1, -1, 1, 0 };
	Mathf::NormalizeDirectXVector(spot_light[0].direction);

	spot_light[0].range = 300;
	spot_light[0].angle = DirectX::XMConvertToRadians(25.0f);
	spot_light[0].color = { 1, 0, 0, 1 };


	spot_light[0].position = { 0, 50, 0, 0 };
	spot_light[0].direction = { 1, -1, 1, 0 };
	Mathf::NormalizeDirectXVector(spot_light[0].direction);
	spDirection.Set(1, -1, 1);
	spDirection.Normalize();
	spot_light[0].range = 300;
	spot_light[0].angle = DirectX::XMConvertToRadians(25.0f);
	spot_light[0].color = { 1, 0, 0, 1 };

	spot_light[1].position = { -15, 3, 15, 0 };
	spot_light[1].direction = { +1, -1, -1, 0 };
	spot_light[1].range = 100;
	spot_light[1].color = { 0, 1, 0, 1 };
	spot_light[2].position = { 15, 3, -15, 0 };
	spot_light[2].direction = { -1, -1, +1, 0 };
	spot_light[2].range = 100;
	spot_light[2].color = { 0, 0, 1, 1 };
	spot_light[3].position = { -15, 3, -15, 0 };
	spot_light[3].direction = { +1, -1, +1, 0 };
	spot_light[3].range = 100;
	spot_light[3].color = { 1, 1, 1, 1 };
	ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);
}

void Lemur::Scene::BaseScene::InitializeFramebuffer()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// FOG
	fullscreenQuad = std::make_unique<FullscreenQuad>(graphics.GetDevice());

	// SCENE
	framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);

	// FOG
	framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);

	// TEX
	framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);

	//BLOOM
	bloomer = std::make_unique<Bloom>(graphics.GetDevice(), 1920, 1080);

	// �֊s��
	framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);

}

void Lemur::Scene::BaseScene::InitializePS()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// FOG
	create_ps_from_cso(graphics.GetDevice(), "Shader/fog_ps.cso", pixel_shaders[static_cast<size_t>(PS::FOG)].GetAddressOf());
	// �|�X�g�G�t�F�N�g�ŏI
	create_ps_from_cso(graphics.GetDevice(), "Shader/final_pass_ps.cso", pixel_shaders[static_cast<size_t>(PS::FINAL)].GetAddressOf());
	// SKYMAP
	create_ps_from_cso(graphics.GetDevice(), "./Shader/skymap_ps.cso", pixel_shaders[static_cast<size_t>(PS::SKY)].GetAddressOf());
	// DEFERRED
	create_ps_from_cso(graphics.GetDevice(), "./Shader/deferred_rendering_ps.cso", pixel_shaders[static_cast<size_t>(PS::DEFFERED)].GetAddressOf());
}


void Lemur::Scene::BaseScene::SetUpRendering()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
	ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediate_context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	immediate_context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	// �����_�[�^�[�Q�b�g���̐ݒ�ƃN���A
	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	// �L�����o�X�S�̂��w�肵���F�ɓh��Ԃ�
	immediate_context->ClearRenderTargetView(render_target_view, color);

	if (!enable_fog)immediate_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ���ꂩ��`���L�����o�X���w�肷��
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	immediate_context->PSSetSamplers(0, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
	immediate_context->PSSetSamplers(1, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
	immediate_context->PSSetSamplers(2, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
	immediate_context->PSSetSamplers(3, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
	immediate_context->PSSetSamplers(4, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
	// SHADOW
	immediate_context->PSSetSamplers(5, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::COMPARISON_LINEAR_BORDER_WHITE)].GetAddressOf());
	//FOG
	immediate_context->PSSetSamplers(6, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_CLAMP)].GetAddressOf());


	Camera& camera = Camera::Instance();

	// �v�Z
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);

		float aspect_ratio{ viewport.Width / viewport.Height };
		DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 1000.0f) };

		DirectX::XMVECTOR eye = camera.GetEye();
		DirectX::XMVECTOR focus = camera.GetFocus();
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };

		DirectX::XMStoreFloat4x4(&scene_constant.view_projection, V * P);
		lights.directional_light_direction = directional_light_direction;
		// UNIT.16
		DirectX::XMStoreFloat4(&camera_position, camera.GetEye());
		scene_constant.camera_position = camera_position;

		// FOG
		DirectX::XMStoreFloat4x4(&scene_constant.inverse_projection, DirectX::XMMatrixInverse(NULL, P));
		DirectX::XMStoreFloat4x4(&scene_constant.inv_view_projection, DirectX::XMMatrixInverse(NULL, V * P));
		scene_constant.time = high_resolution_timer::Instance().time_stamp();

		// �W�I���g���V�F�[�_�[�ɂăr���{�[�h�v�Z���邽�ߊe�s���[��
		DirectX::XMStoreFloat4x4(&scene_constant.view_matrix, V);
		DirectX::XMStoreFloat4x4(&scene_constant.projection_matrix, P);
	}


	// �萔�o�b�t�@�̍X�V
	{
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].Get(), 0, 0, &scene_constant, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());

		// FOG
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].Get(), 0, 0, &fog_constants, 0, 0);
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].GetAddressOf());

		// �I�v�V����
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].Get(), 0, 0, &option_constant, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::OPTION), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::OPTION), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::OPTION), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());

		// PBR
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].Get(), 0, 0, &pbr, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::PBR), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::PBR), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::PBR), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());

		// �����t�H�O
		dis_fog_constants fogs{};
		fogs.fog_color = fog_color;
		fogs.fog_range = fog_range;
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].Get(), 0, 0, &fogs, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::D_FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::D_FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::D_FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());

		// ���C�g
		lights.ambient_color = ambient_color;
		lights.directional_light_direction = directional_light_direction;
		lights.directional_light_color = directional_light_color;
		memcpy_s(lights.point_light, sizeof(lights.point_light), point_light, sizeof(point_light));
		memcpy_s(lights.spot_light, sizeof(lights.spot_light), spot_light, sizeof(spot_light));
		immediate_context->UpdateSubresource(light_constant_buffer.Get(), 0, 0, &lights, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::LIGHT), 1, light_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::LIGHT), 1, light_constant_buffer.GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::LIGHT), 1, light_constant_buffer.GetAddressOf());

		// �������C�g
		hemisphere_light_constants hemisphere_lights{};
		hemisphere_lights.sky_color = sky_color;
		hemisphere_lights.ground_color = ground_color;
		hemisphere_lights.hemisphere_weight.x = hemisphere_weight;
		immediate_context->UpdateSubresource(hemisphere_light_constant_buffer.Get(), 0, 0, &hemisphere_lights, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::HEMISPERE), 1, hemisphere_light_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::HEMISPERE), 1, hemisphere_light_constant_buffer.GetAddressOf());
		immediate_context->GSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::HEMISPERE), 1, hemisphere_light_constant_buffer.GetAddressOf());
	}
}

void Lemur::Scene::BaseScene::SetUpDeffered()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
	ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

	FLOAT color[4] = { 0,0,0,1 };
	if (enable_deferred)
	{
		// �����_�[�^�[�Q�b�g�̃r���[�z��Ɛ[�x�X�e���V���o�b�t�@���o�̓����_�[�p�C�v���C���Ƀo�C���h����B
		immediate_context->OMSetRenderTargets(G_MAX, renderTargetViewArray->GetAddressOf(), depth_stencil_view);

		// �����_�[�^�[�Q�b�g�o�b�t�@�[���N���A
		for (int i = 0; i < G_MAX; i++)
		{
			immediate_context->ClearRenderTargetView(renderTargetViewArray[i].Get(), color);
		}
	}
}

void Lemur::Scene::BaseScene::RenderingDeffered()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
	ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

	if (enable_deferred)
	{
		// �����_�[�^�[�Q�b�g���ꖇ��
		immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

		// �V�F�[�_�[�ݒ�
		// GBuffer�ݒ�
		ID3D11ShaderResourceView* shader_resource_views[G_MAX] =
		{
			// GB_BaseColor�̓X�v���C�g���Ŏw�肵�Ă���̂ł�����l��
			shaderResourceViewArray[BASECOLOR].Get(),
			shaderResourceViewArray[NORMAL].Get(),
			shaderResourceViewArray[POSITION].Get(),
			shaderResourceViewArray[MS].Get(),
			shaderResourceViewArray[EMISSIVE].Get(),
			shaderResourceViewArray[OCCLUSION].Get(),
		};

		immediate_context->PSSetShaderResources(1, G_MAX, shader_resource_views);

		// �|�X�g�G�t�F�N�g�J�n
		if (enable_deferred_post)
		{
			framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
			framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
		}

		// �`��
		fullscreenQuad->Blit(immediate_context, shader_resource_views, 0, _countof(shader_resource_views), pixel_shaders[static_cast<size_t>(PS::DEFFERED)].Get());

		if (enable_deferred_post)
		{
			framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
			ExePostEffct();
		}

		ID3D11ShaderResourceView* clear_shader_resource_view[G_MAX]{ nullptr, nullptr, nullptr,nullptr,nullptr,nullptr };
		immediate_context->PSSetShaderResources(1, G_MAX, clear_shader_resource_view);
	}
}

void Lemur::Scene::BaseScene::ExePostEffct()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();

	// BLOOM
	bloomer->Make(immediate_context, framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->shader_resource_views[0].Get());

	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	ID3D11ShaderResourceView* final_pass_shader_resource_views[] =
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->shader_resource_views[0].Get(),
		bloomer->ShaderResourceView(),
		framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)]->shader_resource_views[0].Get(),
		framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->shader_resource_views[0].Get(),
	};
	fullscreenQuad->Blit(immediate_context, final_pass_shader_resource_views, 0, _countof(final_pass_shader_resource_views), pixel_shaders[static_cast<size_t>(PS::FINAL)].Get());
}

void Lemur::Scene::BaseScene::LightUpdate()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	Mathf::NormalizeDirectXVector(spot_light[0].direction);
	//spot_light[0].angle = DirectX::XMConvertToRadians(sp_angle);

	for (int i = 0; i < 8; i++)
	{
		// Y������̉�]�N�H�[�^�j�I�����v�Z����
		Quaternion qRotY;
		qRotY.SetRotationY(gamePad.GetAxisLX() * 0.001f);

		// �v�Z�����N�H�[�^�j�I���Ń��C�g�̕�������
		qRotY.Apply(spDirection);

		// X������̉�]�N�H�[�^�j�I�����v�Z����
		Vector3 rotAxis;
		rotAxis.Cross(g_vec3AxisY, spDirection);
		Quaternion qRotX;
		qRotX.SetRotation(rotAxis, gamePad.GetAxisLY() * 0.001f);

		// �v�Z�����N�H�[�^�j�I���Ń��C�g�̕�������
		qRotX.Apply(spDirection);

		spot_light[i].direction.x = spDirection.x;
		spot_light[i].direction.y = spDirection.y;
		spot_light[i].direction.z = spDirection.z;
	}
}


void Lemur::Scene::BaseScene::EasingFunction::EasingScale(float elapsed_time)
{
	if (is_easing)
	{
		if (timer > time_max)
		{
			timer = time_max;
			scale = target_scale;
			is_easing = false;
			timer = 0.0f;
			return;
		}

		if (target_scale >= start_scale)
		{
			if (scale >= target_scale) {
				scale = target_scale;
				is_easing = false;
				timer = 0.0f;
				return;
			}
		}
		else if (target_scale >= start_scale)
		{
			if (scale <= target_scale) {
				scale = target_scale;
				is_easing = false;
				timer = 0.0f;
				return;
			}
		}

		timer += elapsed_time;

		scale = {
		Easing::InSine(timer, time_max, target_scale, start_scale)
		};
	}
}

void Lemur::Scene::BaseScene::EasingFunction::ContinueEasing(float elapsed_time)
{
	if (!is_easing && is_continue_scale)
	{
		switch (continue_state)
		{
		case 0:// Up
			// ���Ԑ؂�
			if (timer > continue_time_max)
			{
				timer = continue_time_max;// ���߂������Ԃ�߂�
				scale = continue_target_scale;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �_�E���X�P�[���̏���
				continue_start_scale = scale;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_scale = continue_min;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 1;// �X�e�[�g������
				break;
			}
			if (scale >= continue_target_scale)
			{
				scale = continue_target_scale;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �_�E���X�P�[���̏���
				continue_start_scale = scale;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_scale = continue_min;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 1;// �X�e�[�g������
				break;
			}

			timer += elapsed_time;

			scale = {
			Easing::OutSine(timer, continue_time_max, continue_target_scale, continue_start_scale)
			};

			break;

		case 1:// Down
			if (timer > continue_time_max)
			{
				timer = continue_time_max;
				scale = continue_target_scale;
				timer = 0.0f;

				// �A�b�v�X�P�[���̏���
				continue_start_scale = scale;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_scale = continue_max;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 0;// �X�e�[�g��O��
				break;
			}

			if (scale <= continue_target_scale) {
				scale = continue_target_scale;// �X�P�[���𒲐�
				timer = 0.0f;// �^�C�}�[��������

				// �A�b�v�X�P�[���̏���
				continue_start_scale = scale;// �X�^�[�g�X�P�[���̐ݒ�
				continue_target_scale = continue_max;// �^�[�Q�b�g�X�P�[���̐ݒ�
				continue_state = 0;// �X�e�[�g��O��
				break;
			}

			timer += high_resolution_timer::Instance().time_interval();

			scale = {
			Easing::InSine(timer, continue_time_max, continue_target_scale, continue_start_scale)
			};
			break;
		}
	}
}
