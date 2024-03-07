#include "GltfModel.h"
#define TINYGLTF_IMPLEMENTATION
#include "../External/tinygltf-release/tiny_gltf.h"
#include "misc.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include <stack>
#include <filesystem>

bool null_load_image_data(tinygltf::Image*, const int, std::string*, std::string*,
    int, int, const unsigned char*, int, void*)
{
    return true;
}

GltfModel::GltfModel(ID3D11Device* device, const std::string& filename) : filename(filename)
{
    tinygltf::TinyGLTF tiny_gltf;
    tiny_gltf.SetImageLoader(null_load_image_data, nullptr);

    tinygltf::Model gltf_model;
    std::string error, warning;
    bool succeeded{ false };

    // ファイルネームによって処理を変える
    if (filename.find(".glb") != std::string::npos)
    {
        succeeded = tiny_gltf.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
    }
    else if (filename.find(".gltf") != std::string::npos)
    {
        succeeded = tiny_gltf.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());
    _ASSERT_EXPR_A(error.empty(), warning.c_str());
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");


    for (std::vector<tinygltf::Scene>::const_reference gltf_scene : gltf_model.scenes)
    {
        scene& scene{ scenes.emplace_back() };
        scene.name = gltf_model.scenes.at(0).name;
        scene.nodes = gltf_model.scenes.at(0).nodes;
    }

    FetchNodes(gltf_model);
    FetchMeshes(device, gltf_model);
    FetchMaterials(device, gltf_model);
    FetchTextures(device, gltf_model);
    FetchAnimations(gltf_model);

     // TODO: これは強制的なプログラミングであり、バグが発生する可能性があります。
    const std::map<std::string, buffer_view>& vertex_buffer_views{
        meshes.at(0).primitives.at(0).vertex_buffer_views };
     D3D11_INPUT_ELEMENT_DESC input_element_desc[]
     {
         { "POSITION", 0, vertex_buffer_views.at("POSITION").format, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
         { "NORMAL", 0, vertex_buffer_views.at("NORMAL").format, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "TANGENT", 0, vertex_buffer_views.at("TANGENT").format, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "TEXCOORD", 0, vertex_buffer_views.at("TEXCOORD_0").format, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "JOINTS", 0, vertex_buffer_views.at("JOINTS_0").format, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "WEIGHTS", 0, vertex_buffer_views.at("WEIGHTS_0").format, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
     };
    create_vs_from_cso(device, "./Shader/gltf_vs_re.cso", vertex_shader.ReleaseAndGetAddressOf(),
    input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, "./Shader/gltf_ps_re.cso", pixel_shader.ReleaseAndGetAddressOf());
   
    // 定数バッファー
    {
        HRESULT hr;
        D3D11_BUFFER_DESC buffer_desc{};

        buffer_desc.ByteWidth = sizeof(primitive_constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_cbuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // ボーン
        buffer_desc.ByteWidth = sizeof(primitive_joint_constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, NULL, primitive_joint_cbuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes)
{
    using namespace DirectX;
    immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      
    std::function<void(int)> traverse
    { [&](int node_index)->void 
        {

        const node& node{ nodes.at(node_index) };
        if (node.mesh > -1)// メッシュが存在する場合
        {
            // メッシュ内の各プリミティブに対して、プリミティブの頂点データバッファインデックスバッファ（index_buffer) を設定
            const mesh& mesh{ meshes.at(node.mesh) };
            for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
            {
                ID3D11Buffer* vertex_buffers[]
                {
                     primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                     primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                     primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                     primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                     primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                     primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
                };
                UINT strides[]
                {
                     static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                     static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                     static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                     static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                     static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                     static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
                };
                UINT offsets[_countof(vertex_buffers)]{ 0 };
                immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
                immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
                    primitive.index_buffer_view.format, 0);

                primitive_constants primitive_data{};
                primitive_data.material = primitive.material;
                primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
                primitive_data.skin = node.skin;
                DirectX::XMStoreFloat4x4(&primitive_data.world,
                    XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));
                immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
                immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
                immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


                // texture
                {
                    const material& material{ materials.at(primitive.material) };
                    const int textureIndices[]
                    {
                        material.data.pbr_metallic_roughness.basecolor_texture.index,
                        material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
                        material.data.normal_texture.index,
                        material.data.emissive_texture.index,
                        material.data.occlusion_texture.index,
                    };
                    ID3D11ShaderResourceView* nullShaderResourceView{};
                    std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices));
                    for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
                    {
                        shaderResourceViews.at(textureIndex) = textureIndices[textureIndex] > -1 ?
                            texture_resource_views.at(textures.at(textureIndices[textureIndex]).source).Get() :
                            nullShaderResourceView;
                    }
                    immediate_context->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()),
                        shaderResourceViews.data());
                }

                // animation
                {
                    if (node.skin > -1)
                    {
                        const skin& skin{ skins.at(node.skin) };
                        primitive_joint_constants primitive_joint_data{};
                        for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
                        {
                            XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                                XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                                XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                                XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
                            );
                        }
                        immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);
                        immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
                    }
                }
                immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
            }
        }
        for (std::vector<int>::value_type child_index : node.children)
        {
            traverse(child_index);
        }
    } };
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }

}

void GltfModel::FetchAnimations(const tinygltf::Model & gltf_model)
 {
   using namespace std;
   using namespace tinygltf;
   using namespace DirectX;
   
   // スキンデータを読み込みskinsベクターに格納する
   for (vector<Skin>::const_reference transmission_skin : gltf_model.skins)
   {
       skin& skin{ skins.emplace_back() };
       const Accessor& gltf_accessor{ gltf_model.accessors.at(transmission_skin.inverseBindMatrices) };
       const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };
       skin.inverse_bind_matrices.resize(gltf_accessor.count);

       //gltf_buffer_view から inverse_bind_matrices へデータをコピー
       memcpy(skin.inverse_bind_matrices.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
           gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4X4));
      
       skin.joints = transmission_skin.joints;
   }
   

   for (vector<Animation>::const_reference gltf_animation : gltf_model.animations)
   {
       // animationsに規定のコンストラクタで追加され、その参照をanimationに格納する
       animation& animation{ animations.emplace_back() };

       animation.name = gltf_animation.name;

       for (vector<AnimationSampler>::const_reference gltf_sampler : gltf_animation.samplers)
       {
           // animation.samplersに規定のコンストラクタで追加され、その参照をsamplerに格納する
           animation::sampler& sampler{ animation.samplers.emplace_back() };
           sampler.input = gltf_sampler.input;
           sampler.output = gltf_sampler.output;
           sampler.interpolation = gltf_sampler.interpolation;// 補完

           const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.input) };
           const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };
           // アニメーションの入力（sampler.input）をキーとして、新しい float ベクターを追加
           pair<unordered_map<int, vector<float>>::iterator, bool>& timelines{ animation.timelines.emplace(gltf_sampler.input, gltf_accessor.count) };
           
           if (timelines.second)
           {
               memcpy(timelines.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                   gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(FLOAT));
           }
       }

       for (vector<AnimationChannel>::const_reference gltf_channel : gltf_animation.channels)
       {
           animation::channel& channel{ animation.channels.emplace_back() };
           channel.sampler = gltf_channel.sampler;
           channel.target_node = gltf_channel.target_node;
           channel.target_path = gltf_channel.target_path;

           const AnimationSampler& gltf_sampler{ gltf_animation.samplers.at(gltf_channel.sampler) };
           const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.output) };
           const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };
           if (gltf_channel.target_path == "scale")
           {
               pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& scales{
                   animation.scales.emplace(gltf_sampler.output, gltf_accessor.count) };
               if (scales.second)
               {
                   memcpy(scales.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                       gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
               }
           }
           else if (gltf_channel.target_path == "rotation")
           {
               pair<unordered_map<int, vector<XMFLOAT4>>::iterator, bool>& rotations{
                   animation.rotations.emplace(gltf_sampler.output, gltf_accessor.count) };
               if (rotations.second)
               {
                   memcpy(rotations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                       gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4));
               }
           }
           else if (gltf_channel.target_path == "translation")
           {
               pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& translations{
                   animation.translations.emplace(gltf_sampler.output, gltf_accessor.count) };
               if (translations.second)
               {
                   memcpy(translations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                       + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
               }
           }
       }
   }
}

void GltfModel::FetchTextures(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr{ S_OK };
    for (const tinygltf::Texture& gltf_texture : gltf_model.textures)
    {
        texture& texture{ textures.emplace_back() };
        texture.name = gltf_texture.name;
        texture.source = gltf_texture.source;
    }

    for (const tinygltf::Image& gltf_image : gltf_model.images)
    {
        image& image{ images.emplace_back() };
        image.name = gltf_image.name;
        image.width = gltf_image.width;
        image.height = gltf_image.height;
        image.component = gltf_image.component;
        image.bits = gltf_image.bits;
        image.pixel_type = gltf_image.pixel_type;
        image.buffer_view = gltf_image.bufferView;
        image.mime_type = gltf_image.mimeType;
        image.uri = gltf_image.uri;
        image.as_is = gltf_image.as_is;

        if (gltf_image.bufferView > -1)
        {
            // 対応するバッファビューとバッファ情報を取得して、テクスチャデータを読み込み
            const tinygltf::BufferView& buffer_view{ gltf_model.bufferViews.at(gltf_image.bufferView) };
            const tinygltf::Buffer& buffer{ gltf_model.buffers.at(buffer_view.buffer) };
            const byte* data = buffer.data.data() + buffer_view.byteOffset;

            ID3D11ShaderResourceView* texture_resource_view{};
            hr = load_texture_from_memory(device, data, buffer_view.byteLength, &texture_resource_view);
            if (hr == S_OK)
            {
                texture_resource_views.emplace_back().Attach(texture_resource_view);
            }
        }
        else
        {
            // バッファビューに関連付けられていない場合、ファイルからテクスチャデータを読み込み
            // ファイルのパスは、GLTFモデルファイルのディレクトリからの相対パスとして構築
            const std::filesystem::path path(filename);
            ID3D11ShaderResourceView* shader_resource_view{};
            D3D11_TEXTURE2D_DESC texture2d_desc;
            std::wstring filename
            {
                path.parent_path().concat(L"/").wstring() +
                    std::wstring(gltf_image.uri.begin(), gltf_image.uri.end())
            };
            hr = load_texture_from_file(device, filename.c_str(), &shader_resource_view, &texture2d_desc);
            if (hr == S_OK)
            {
                texture_resource_views.emplace_back().Attach(shader_resource_view);
            }
        }
    }
}

void GltfModel::FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    for (std::vector<tinygltf::Material>::const_reference gltf_material : gltf_model.materials)
    {
        // 新しいカスタムマテリアルを作成し、materialsベクターに追加
        std::vector<material>::reference material = materials.emplace_back();

        material.name = gltf_material.name;

        // マテリアルのエミッシブをコピー
        material.data.emissive_factor[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
        material.data.emissive_factor[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
        material.data.emissive_factor[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

        // マテリアルのアルファモードを数値に変換してコピー
        material.data.alpha_mode = gltf_material.alphaMode == "OPAQUE" ?
            0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
        
        // マテリアルのアルファカットオフをコピー
        material.data.alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
        // マテリアルの両面描画設定をコピー
        material.data.double_sided = gltf_material.doubleSided ? 1 : 0;

        // マテリアルのPBR情報（基本色ファクターとテクスチャ）をコピー
        material.data.pbr_metallic_roughness.basecolor_factor[0] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(0));
        material.data.pbr_metallic_roughness.basecolor_factor[1] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(1));      
        material.data.pbr_metallic_roughness.basecolor_factor[2] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(2));
        material.data.pbr_metallic_roughness.basecolor_factor[3] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(3));
        material.data.pbr_metallic_roughness.basecolor_texture.index =
            gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        material.data.pbr_metallic_roughness.basecolor_texture.texcoord =
            gltf_material.pbrMetallicRoughness.baseColorTexture.texCoord;
        material.data.pbr_metallic_roughness.metallic_factor =
            static_cast<float>(gltf_material.pbrMetallicRoughness.metallicFactor);
        material.data.pbr_metallic_roughness.roughness_factor =
            static_cast<float>(gltf_material.pbrMetallicRoughness.roughnessFactor);
        material.data.pbr_metallic_roughness.metallic_roughness_texture.index =
            gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord =
            gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

        // マテリアルの法線マップ、オクルージョンマップ、エミッシブマップ情報をコピー
        material.data.normal_texture.index = gltf_material.normalTexture.index;
        material.data.normal_texture.texcoord = gltf_material.normalTexture.texCoord;
        material.data.normal_texture.scale = static_cast<float>(gltf_material.normalTexture.scale);
        material.data.occlusion_texture.index = gltf_material.occlusionTexture.index;
        material.data.occlusion_texture.texcoord = gltf_material.occlusionTexture.texCoord;
        material.data.occlusion_texture.strength =
            static_cast<float>(gltf_material.occlusionTexture.strength);
        material.data.emissive_texture.index = gltf_material.emissiveTexture.index;
        material.data.emissive_texture.texcoord = gltf_material.emissiveTexture.texCoord;

    }

    // GPU上でマテリアルデータをシェーダリソースビューとして作成
    std::vector<material::cbuffer> material_data;
    for (std::vector<material>::const_reference material : materials)
    {
        material_data.emplace_back(material.data);
    }

    // マテリアルデータをGPU上で利用可能にするための準備
    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(material::cbuffer) * material_data.size());
    buffer_desc.StructureByteStride = sizeof(material::cbuffer);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = material_data.data();

    // マテリアルバッファを作成
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};

    // シェーダーリソースビューの設定
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(material_data.size());

    // マテリアルリソースビューを作成
    hr = device->CreateShaderResourceView(material_buffer.Get(), &shader_resource_view_desc, material_resource_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// ノード取得
void GltfModel::FetchNodes(const tinygltf::Model& gltf_model)
{
    for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
    {
        node& node{ nodes.emplace_back() };
        node.name = gltf_node.name;
        node.skin = gltf_node.skin;
        node.mesh = gltf_node.mesh;
        node.children = gltf_node.children;

        if (!gltf_node.matrix.empty())
        {
            // 行列が提供された場合、それを分解してスケール、回転、平行移動に変換
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; row++)
            {
                for (size_t column = 0; column < 4; column++)
                {
                    matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
                }
            }

            DirectX::XMVECTOR S, T, R;
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

            DirectX::XMStoreFloat3(&node.scale, S);
            DirectX::XMStoreFloat4(&node.rotation, R);
            DirectX::XMStoreFloat3(&node.translation, T);
        }
        else
        {
            // 行列が提供されていない場合、スケール、回転、平行移動を直接コピー
            if (gltf_node.scale.size() > 0)
            {
                node.scale.x = static_cast<float>(gltf_node.scale.at(0));
                node.scale.y = static_cast<float>(gltf_node.scale.at(1));
                node.scale.z = static_cast<float>(gltf_node.scale.at(2));
            }
            if (gltf_node.translation.size() > 0)
            {
                node.translation.x = static_cast<float>(gltf_node.translation.at(0));
                node.translation.y = static_cast<float>(gltf_node.translation.at(1));
                node.translation.z = static_cast<float>(gltf_node.translation.at(2));
            }
            if (gltf_node.rotation.size() > 0)
            {
                node.rotation.x = static_cast<float>(gltf_node.rotation.at(0));
                node.rotation.y = static_cast<float>(gltf_node.rotation.at(1));
                node.rotation.z = static_cast<float>(gltf_node.rotation.at(2));
                node.rotation.w = static_cast<float>(gltf_node.rotation.at(3));
            }
        }

    }
     CumulateTransforms(nodes);
    
}

 // メッシュ取得
void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr;
    for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
    {
        mesh& mesh{ meshes.emplace_back() };
        mesh.name = gltf_mesh.name;
        for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
        {
            mesh::primitive& primitive{ mesh.primitives.emplace_back() };
            primitive.material = gltf_primitive.material;

            // インデックスバッファを作成
            const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_primitive.indices) };
            const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };
            primitive.index_buffer_view = MakeBufferView(gltf_accessor);

            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            D3D11_SUBRESOURCE_DATA subresource_data{};
            subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
            hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            // 頂点バッファを作成
            for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
            {
                const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_attribute.second) };
                const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

                buffer_view vertex_buffer_view{ MakeBufferView(gltf_accessor) };

                D3D11_BUFFER_DESC buffer_desc{};
                buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
                buffer_desc.Usage = D3D11_USAGE_DEFAULT;
                buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresource_data{};
                subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                    + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
                hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                    vertex_buffer_view.buffer.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));
            }

            // ダミー属性が不足している場合は追加
            const std::unordered_map<std::string, buffer_view> attributes{
                { "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
                { "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
                { "JOINTS_0", { DXGI_FORMAT_R16G16B16A16_UINT } },
                { "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
            };
            for (std::unordered_map<std::string, buffer_view>::const_reference attribute : attributes)
            {
                if (primitive.vertex_buffer_views.find(attribute.first) == primitive.vertex_buffer_views.end())
                {
                    primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
                }
            }
        }
    }
}

// 姿勢計算
void GltfModel::CumulateTransforms(std::vector<node>& nodes)
{
    using namespace DirectX;

    std::stack<XMFLOAT4X4> parent_global_transforms;
    std::function<void(int)> traverse{ [&](int node_index)->void
        {
            node& node{ nodes.at(node_index) };
            XMMATRIX S{ XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
            XMMATRIX R{ XMMatrixRotationQuaternion(
            XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
            XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * XMLoadFloat4x4(&parent_global_transforms.top()));
            for (int child_index : node.children)
            {
                parent_global_transforms.push(node.global_transform);
                traverse(child_index);
                parent_global_transforms.pop();

            }
    } };
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        parent_global_transforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
        traverse(node_index);
        parent_global_transforms.pop();
    }

}

GltfModel::buffer_view GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
    buffer_view buffer_view;
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            buffer_view.format = DXGI_FORMAT_R16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            buffer_view.format = DXGI_FORMAT_R32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT);
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC2:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;

        }
        break;
    case TINYGLTF_TYPE_VEC3:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC4:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 4;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    default:
        _ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
        break;

    }
    buffer_view.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view.stride_in_bytes);
    return buffer_view;
}

void GltfModel::Animate(size_t animation_index, float time, std::vector<node>& animated_nodes, bool loopback)
{
    using namespace std;
    using namespace DirectX;

    function<size_t(const vector<float>&, float, float&, bool)> indexof
    {[](const vector<float>& timelines, float time, float& interpolation_factor, bool loopback)->size_t 
        {
            // ラムダ式内で特定の操作を行う
            // timelines: float値のvector
            // time: 検索する時間
            // interpolation_factor: 補間係数（出力用）
            // loopback: ループバックフラグ
            const size_t keyframe_count{ timelines.size() };

            // 時間が最後のキーフレームを超える場合
            if (time > timelines.at(keyframe_count - 1))
            {
                if (loopback)
                {
                    time = fmodf(time, timelines.at(keyframe_count - 1));
                }
                else
                {
                    interpolation_factor = 1.0f;
                    return keyframe_count - 2;
                }
            }

            // 時間が最初のキーフレームよりも早い場合
            else if (time < timelines.at(0))
            {
                interpolation_factor = 0.0f;
                return 0;
            }

            size_t keyframe_index{ 0 };

            // キーフレームを探す
            for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
            {
                if (time < timelines.at(time_index))
                {
                    keyframe_index = max<size_t>(0LL, time_index - 1);
                    break;
                }
            }

            // 補間係数を計算
            interpolation_factor = (time - timelines.at(keyframe_index + 0)) /
                (timelines.at(keyframe_index + 1) - timelines.at(keyframe_index + 0));
            return keyframe_index;
        }
    };

    // アニメーションが空でないか確認
    if (animations.size() > 0)
    {
        // 指定されたアニメーションを取得
        const animation& animation{ animations.at(animation_index) };

        // アニメーションの各チャンネル
        for (vector<animation::channel>::const_reference channel : animation.channels)
        {
            const animation::sampler& sampler{ animation.samplers.at(channel.sampler) };
            // キーフレームの時間情報を取得
            const vector<float>& timeline{ animation.timelines.at(sampler.input) };
            if (timeline.size() == 0)// 中身がなければ次のチャンネルへ
            {
                continue;
            }
            float interpolation_factor{};
            // 上で定義した関数オブジェクトを使用して time の位置に最も近いキーフレームのインデックスと補間係数を計算。
            size_t keyframe_index{ indexof(timeline, time, interpolation_factor, loopback) };
            // チャンネルのパスによってそれぞれ別の処理を適用する
            if (channel.target_path == "scale")
            {
                const vector<XMFLOAT3>& scales{ animation.scales.at(sampler.output) };
                XMStoreFloat3(&animated_nodes.at(channel.target_node).scale,
                    XMVectorLerp(XMLoadFloat3(&scales.at(keyframe_index + 0)),
                        XMLoadFloat3(&scales.at(keyframe_index + 1)), interpolation_factor));
            }
            else if (channel.target_path == "rotation")
            {
                const vector<XMFLOAT4>& rotations{ animation.rotations.at(sampler.output) };
                DirectX::XMStoreFloat4(&animated_nodes.at(channel.target_node).rotation,
                    XMQuaternionNormalize(XMQuaternionSlerp(XMLoadFloat4(&rotations.at(keyframe_index + 0)),
                        XMLoadFloat4(&rotations.at(keyframe_index + 1)), interpolation_factor)));
            }
            else if (channel.target_path == "translation")
            {
                const vector<XMFLOAT3>& translations{ animation.translations.at(sampler.output) };
                XMStoreFloat3(&animated_nodes.at(channel.target_node).translation,
                    XMVectorLerp(XMLoadFloat3(&translations.at(keyframe_index + 0)),
                        XMLoadFloat3(&translations.at(keyframe_index + 1)), interpolation_factor));
            }
        }
        CumulateTransforms(animated_nodes);
    }
    else
    {
        animated_nodes = nodes;
    }
}
