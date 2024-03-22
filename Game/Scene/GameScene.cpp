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

void GameScene::Initialize()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// シェーダー関連
	{
		SetState();

		HRESULT hr{ S_OK };
		// シーン定数バッファオブジェクトを生成
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;// 読み取りおよび書き込みがどのように行われるか
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファがパイプラインにどのようにバインド(結びつけ)されるかを特定
			buffer_desc.CPUAccessFlags = 0;// CPU アクセス フラグ（CPU アクセスが必要ない場合は 0）
			buffer_desc.MiscFlags = 0;// その他のフラグ（未使用に場合は0）
			buffer_desc.StructureByteStride = 0;//バッファが構造化バッファを表す場合の、バッファ構造内の各要素のサイズ
			{
				buffer_desc.ByteWidth = sizeof(scene_constants);// バッファのサイズ 
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // フォグ
				buffer_desc.ByteWidth = sizeof(fog_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // オプション
				buffer_desc.ByteWidth = sizeof(option_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // 距離フォグ
				buffer_desc.ByteWidth = sizeof(dis_fog_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // PBR
				buffer_desc.ByteWidth = sizeof(adjust_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{ // ライト
				buffer_desc.ByteWidth = sizeof(lightConstants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, light_constant_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			{
				// 半球ライト
				buffer_desc.ByteWidth = sizeof(hemisphere_light_constants);
				hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, hemisphere_light_constant_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}

		// SCENE
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);

		// FOG
		fullscreenQuad = std::make_unique<FullscreenQuad>(graphics.GetDevice());
		framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)] = std::make_unique<Framebuffer>(graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, true);
		create_ps_from_cso(graphics.GetDevice(), "Shader/fog_ps.cso", pixel_shaders[static_cast<size_t>(PS::FOG)].GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "Shader/final_pass_ps.cso", pixel_shaders[static_cast<size_t>(PS::FINAL)].GetAddressOf());

		//BLOOM
		bloomer = std::make_unique<Bloom>(graphics.GetDevice(), 1920, 1080);

		// SKYMAP
		create_ps_from_cso(graphics.GetDevice(), "./Shader/skymap_ps.cso", pixel_shaders[static_cast<size_t>(PS::SKY)].GetAddressOf());
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		double_speed_z = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);

		// dissolve
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO 実験用
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());
	}
	// ゲーム部分
	{
		Camera& camera = Camera::Instance();
		// ステージ初期化
		StageManager& stage_manager = StageManager::Instance();
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// プレイヤーの初期化
		UnitManager& unitManager = UnitManager::Instance();
		Unit_A* unit_A = new Unit_A();
		unit_A->SetPosition(DirectX::XMFLOAT3(2.0f, 0, 5));
		unitManager.Register(unit_A);		


		// エネミーの初期化
		EnemyManager& enemyManager = EnemyManager::Instance();
		for (int i = 0; i < 2; ++i)
		{
			Enemy_A* enemy_A = new Enemy_A();
			enemy_A->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
			enemyManager.Register(enemy_A);
		}


		ohajiki = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\おはじき.png");
	}
	// ポイントライト・スポットライトの初期位置設定
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
}

void GameScene::Finalize()
{
	//ステージ終了
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
}

void GameScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();

	s_l = gamePad.GetAxisLY()*-1.0f;

	if (s_l > 0.1f)
	{
		timer_s += elapsedTime;
		if (s_l_max <= s_l)s_l_max = s_l;
	}
	else
	{
		if(timer_s>0)f_d = (s_l_max * 100.0f) / timer_s;
		s_l_max = 0;
		timer_s = 0;
	}
	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	adapter_timer += 0.01f;

	// カメラ
	{
		camera.Update(elapsedTime);
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
	}

	//ステージ更新処理
	StageManager::Instance().update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime);
	//UnitManager::Instance().Update(elapsedTime);

	// ３Dディゾルブ
	{
	}
	// スポットライト
	{
		Mathf::NormalizeDirectXVector(spot_light[0].direction);
		//spot_light[0].angle = DirectX::XMConvertToRadians(sp_angle);

		for (int i = 0; i < 8; i++)
		{
			// Y軸周りの回転クォータニオンを計算する
			Quaternion qRotY;
			qRotY.SetRotationY(gamePad.GetAxisLX() * 0.001f);

			// 計算したクォータニオンでライトの方向を回す
			qRotY.Apply(spDirection);

			// X軸周りの回転クォータニオンを計算する
			Vector3 rotAxis;
			rotAxis.Cross(g_vec3AxisY, spDirection);
			Quaternion qRotX;
			qRotX.SetRotation(rotAxis, gamePad.GetAxisLY() * 0.001f);

			// 計算したクォータニオンでライトの方向を回す
			qRotX.Apply(spDirection);

			spot_light[i].direction.x = spDirection.x;
			spot_light[i].direction.y = spDirection.y;
			spot_light[i].direction.z = spDirection.z;
			//spot_light[i].
		}
	}
	//---------------------------------------------------------------------------------------
	// Imgui
	//---------------------------------------------------------------------------------------
	{
		ImGui::Begin("ImGUI");
		ImGui::Text("s_l : %f", s_l);
		ImGui::Text("f_d : %f", f_d);
		ImGui::Text("s_l_max : %f", s_l_max);
		ImGui::Text("timer_s : %f", timer_s);


		ImGui::SliderFloat3("pos", &pos.x, -30.0f, +30.0f);

		ImGui::SliderFloat4("parameters", &option_constant.parameters.x, 0.0f, +1.0f);

		ImGui::Separator();
		ImGui::ColorEdit3("sky_color", &sky_color.x);
		ImGui::ColorEdit3("ground_color", &ground_color.x);
		ImGui::SliderFloat("hemisphere_weight", &hemisphere_weight, 0.0f, 1.0f);

		// STATIC_BATCHING
		ImGui::Text("adapter_timer : %f", adapter_timer);
		ImGui::Text("drawcall_count : %d", drawcall_count);
		if (ImGui::TreeNode("camera"))
		{
			ImGui::SliderFloat3("camera_target", &camera_target.x, -10.0f, +10.0f);
			ImGui::SliderFloat("camera_range", &camera_range, 0.0f, +100.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("enable_shader"))
		{
			ImGui::Checkbox("enableShadow", &enable_shadow);
			ImGui::Checkbox("enableSkyMap", &enable_skymap);
			ImGui::Checkbox("enableBloom", &enable_bloom);
			ImGui::Checkbox("enableFog", &enable_fog);
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
		// ポイントライト
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
		// スポットライト
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
		if (ImGui::TreeNode("shadow"))
		{
			ImGui::Image(reinterpret_cast<void*>(double_speed_z->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
			ImGui::SliderFloat("shadow_depth_bias", &scene_constants.shadow_depth_bias, 0.1f, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("bloom"))
		{
			ImGui::SliderFloat("bloom_extraction_threshold", &bloomer->bloom_extraction_threshold, +0.0f, +5.0f);
			ImGui::SliderFloat("bloom_intensity", &bloomer->bloom_intensity, +0.0f, +5.0f);
			ImGui::TreePop();
		}
		ImGui::End();
	}
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
	SetUpRendering();

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
	// SHADOW : make shadow map
	if (enable_shadow)
	{
		using namespace DirectX;

		const float aspect_ratio = double_speed_z->viewport.Width / double_speed_z->viewport.Height;
		XMVECTOR F{ camera.GetFocus() };
		XMVECTOR E{ F - XMVector3Normalize(XMLoadFloat4(&directional_light_direction)) * light_view_distance };
		XMVECTOR U{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		XMMATRIX V{ XMMatrixLookAtLH(E, F, U) };
		XMMATRIX P{ XMMatrixOrthographicLH(light_view_size * aspect_ratio, light_view_size, light_view_near_z, light_view_far_z) };

		DirectX::XMStoreFloat4x4(&scene_constants.view_projection, V * P);
		scene_constants.light_view_projection = scene_constants.view_projection;
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].Get(), 0, 0, &scene_constants, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());

		double_speed_z->Clear(immediate_context, 1.0f);
		double_speed_z->Activate(immediate_context);

		ID3D11PixelShader* null_pixel_shader{ NULL };
		//player->Render(elapsedTime, &null_pixel_shader);
		//skinned_meshes[0]->Render(immediate_context, { -0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 1 }, material_color, nullptr, &null_pixel_shader);
		//skinned_meshes[1]->Render(immediate_context, { -0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 1 }, material_color, nullptr, &null_pixel_shader);
		//skinned_meshes[2]->Render(immediate_context, { -1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1 }, material_color, nullptr, &null_pixel_shader);

		//static std::vector<GltfModel::node> animated_nodes{ gltf_models[0]->nodes };
		//static float time{ 0 };
		//gltf_models[0]->Animate(0, time += elapsedTime, animated_nodes, true);
		//gltf_models[0]->Render(immediate_context, { -0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 1 }, animated_nodes);

		double_speed_z->Deactivate(immediate_context);
	}

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

		DirectX::XMStoreFloat4x4(&scene_constants.view_projection, V * P);
		lights.directional_light_direction = directional_light_direction;
		// UNIT.16
		DirectX::XMStoreFloat4(&camera_position, camera.GetEye());
		scene_constants.camera_position = camera_position;

		// SKYMAP
		if (enable_skymap)
		{
			DirectX::XMStoreFloat4x4(&scene_constants.inv_view_projection, DirectX::XMMatrixInverse(NULL, camera.GetViewMatrix() * camera.GetProjectionMatrix()));
		}

		// FOG
		DirectX::XMStoreFloat4x4(&scene_constants.inverse_projection, DirectX::XMMatrixInverse(NULL, P));
		DirectX::XMStoreFloat4x4(&scene_constants.inv_view_projection, DirectX::XMMatrixInverse(NULL, V * P));
		scene_constants.time = high_resolution_timer::Instance().time_stamp();
	}

	// 定数バッファの更新
	{
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].Get(), 0, 0, &scene_constants, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::SCENE), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::SCENE)].GetAddressOf());

		// FOG
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].Get(), 0, 0, &fog_constants, 0, 0);
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::FOG)].GetAddressOf());

		// オプション
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].Get(), 0, 0, &option_constant, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::OPTION), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::OPTION), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::OPTION)].GetAddressOf());

		// PBR
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].Get(), 0, 0, &pbr, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::PBR), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::PBR), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::PBR)].GetAddressOf());

		// 距離フォグ
		dis_fog_constants fogs{};
		fogs.fog_color = fog_color;
		fogs.fog_range = fog_range;
		immediate_context->UpdateSubresource(constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].Get(), 0, 0, &fogs, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::D_FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::D_FOG), 1, constant_buffers[static_cast<size_t>(CONSTANT_BUFFER::D_FOG)].GetAddressOf());

		// ライト
		lights.ambient_color = ambient_color;
		lights.directional_light_direction = directional_light_direction;
		lights.directional_light_color = directional_light_color;
		memcpy_s(lights.point_light, sizeof(lights.point_light), point_light, sizeof(point_light));
		memcpy_s(lights.spot_light, sizeof(lights.spot_light), spot_light, sizeof(spot_light));
		immediate_context->UpdateSubresource(light_constant_buffer.Get(), 0, 0, &lights, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::LIGHT), 1, light_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::LIGHT), 1, light_constant_buffer.GetAddressOf());

		// 半球ライト
		hemisphere_light_constants hemisphere_lights{};
		hemisphere_lights.sky_color = sky_color;
		hemisphere_lights.ground_color = ground_color;
		hemisphere_lights.hemisphere_weight.x = hemisphere_weight;
		immediate_context->UpdateSubresource(hemisphere_light_constant_buffer.Get(), 0, 0, &hemisphere_lights, 0, 0);
		immediate_context->VSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::HEMISPERE), 1, hemisphere_light_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(static_cast<size_t>(CONSTANT_BUFFER_R::HEMISPERE), 1, hemisphere_light_constant_buffer.GetAddressOf());

	}


	// テクスチャをセット
	{
		// ノイズ
		immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, noise.GetAddressOf());//TODO
		// シャドウ
		immediate_context->PSSetShaderResources(8, 1, double_speed_z->shader_resource_view.GetAddressOf());
	}

	// BLOOM
	framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
	framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);

	if (enable_skymap)
	{
		// SKYMAP
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		fullscreenQuad->Blit(immediate_context, skymap.GetAddressOf(), 0, 1, pixel_shaders[static_cast<size_t>(PS::SKY)].Get());
	}
	//3D描画
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());


		immediate_context->PSSetShaderResources(32, 1, shader_resource_views[0].GetAddressOf());
		immediate_context->PSSetShaderResources(33, 1, shader_resource_views[1].GetAddressOf());
		immediate_context->PSSetShaderResources(34, 1, shader_resource_views[2].GetAddressOf());
		immediate_context->PSSetShaderResources(35, 1, shader_resource_views[3].GetAddressOf());


		{
			//ステージ描画
			StageManager::Instance().Render(0.1f, stage_ps.GetAddressOf());
			//UnitManager::Instance().Render(0.1f, chara_ps.GetAddressOf());
			EnemyManager::Instance().Render(0.1f, chara_ps.GetAddressOf());
		}
	}

	framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);

	// BLOOM
	bloomer->Make(immediate_context, framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->shader_resource_views[0].Get());

	// FOG
	framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)]->Clear(immediate_context);
	framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)]->Activate(immediate_context);

	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);

	fullscreenQuad->Blit(immediate_context, framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->shader_resource_views[1].GetAddressOf()/*Depth*/, 0, 1, pixel_shaders[static_cast<size_t>(PS::FOG)].Get());

	framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)]->Deactivate(immediate_context);

	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	ID3D11ShaderResourceView* final_pass_shader_resource_views[] =
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->shader_resource_views[0].Get(),
		bloomer->ShaderResourceView(),
		framebuffers[static_cast<size_t>(FRAME_BUFFER::FOG)]->shader_resource_views[0].Get(),
	};
	fullscreenQuad->Blit(immediate_context, final_pass_shader_resource_views, 0, _countof(final_pass_shader_resource_views), pixel_shaders[static_cast<size_t>(PS::FINAL)].Get());

	ohajiki->Render(immediate_context, 100, f_d*0.5f, 100, 100);
}

void GameScene::DebugImgui()
{
}
