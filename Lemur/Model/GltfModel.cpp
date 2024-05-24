#include "GltfModel.h"
#define TINYGLTF_IMPLEMENTATION
#include "../External/tinygltf-release/tiny_gltf.h"
#include "../Source/misc.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Graphics/Texture.h"
#include <stack>

#include <filesystem>
// テクスチャ外部読み込み
#include <fstream>
#include <WICTextureLoader.h>
#include <stb_image.h>
#include <string.h> 

#include "./ImGuiCtrl.h"

bool null_load_image_data(tinygltf::Image*, const int, std::string*, std::string*,
    int, int, const unsigned char*, int, void*)
{
    return true;
}

GltfModel::GltfModel(ID3D11Device* device, const std::string& filename, bool external_texture) : filename(filename)
{
    tinygltf::TinyGLTF tiny_gltf;
    // 画像をロードするためのコールバックを設定
    tiny_gltf.SetImageLoader(null_load_image_data, nullptr);

    tinygltf::Model gltf_model;
    std::string error, warning;
    bool succeeded{ false };
    if (filename.find(".glb") != std::string::npos)
    {
        // ファイルをロード(glbのとき)
        succeeded = tiny_gltf.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
    }
    else if (filename.find(".gltf") != std::string::npos)
    {
        // ファイルをロード(gltfのとき)
        succeeded = tiny_gltf.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());      // 警告があれば表示
    _ASSERT_EXPR_A(error.empty(), error.c_str());          // エラーがあれば表示
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");// ファイル読み込みに失敗したとき

    // モデルからシーン情報を1つづつ取り出し（シーンの数だけループを回す）
    for (std::vector<tinygltf::Scene>::const_reference gltf_scene : gltf_model.scenes)
    {
        scene& scene{ scenes.emplace_back() };      // scenesに新しい空のsceneを追加してその参照を格納
        scene.name = gltf_model.scenes.at(0).name;  // すべてのsceneオブジェクトに同じ名前を設定
        scene.nodes = gltf_model.scenes.at(0).nodes;// すべての scene オブジェクトに同じノード情報を設定
    }

    // ノード取得
    FetchNodes(gltf_model);
    // メッシュ取得
    FetchMeshes(device, gltf_model);
    // マテリアル取得
    FetchMaterials(device, gltf_model);
    // テクスチャ取得
    if(!external_texture)FetchTextures(device, gltf_model);
    // 外部からのテクスチャ読み込み
    else FetchExternalTextures(device, gltf_model, filename);
    // アニメーション取得
    FetchAnimations(gltf_model);
    // マテリアルをGPUに渡す
    MaterialForGPU(device);

    // TODO これは強引なプログラミングであり、バグを引き起こす可能性がある。
    const std::map<std::string, buffer_view>& vertex_buffer_views{ meshes.at(0).primitives.at(0).vertex_buffer_views };

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, vertex_buffer_views.at("POSITION").format, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, vertex_buffer_views.at("NORMAL").format, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, vertex_buffer_views.at("TANGENT").format, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, vertex_buffer_views.at("TEXCOORD_0").format, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS", 0, vertex_buffer_views.at("JOINTS_0").format, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, vertex_buffer_views.at("WEIGHTS_0").format, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    create_vs_from_cso(device, "./Shader/gltf_model_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
        input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, "./Shader/gltf_model_ps.cso", pixel_shader.ReleaseAndGetAddressOf());


    // 定数バッファー
    {
        HRESULT hr;
        D3D11_BUFFER_DESC buffer_desc{};

        buffer_desc.ByteWidth = sizeof(PrimitiveConstants);
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

void GltfModel::FetchNodes(const tinygltf::Model& gltf_model)
{
    // モデルからノード情報を1つづつ取り出す（ノードの数だけループを回す）
    for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
    {
        node& node{ nodes.emplace_back() };      // nodesに新しい空のnodeを追加してその参照を格納
        // 各情報を取り出して渡す
        node.name = gltf_node.name;
        node.skin = gltf_node.skin;
        node.mesh = gltf_node.mesh;
        node.children = gltf_node.children;

        // ノードから行列情報を読み取って、XMFLOAT4X4行列に変換
        if (!gltf_node.matrix.empty())
        {
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; row++)// 行（row）を0から3まで順番に処理
            {
                for (size_t column = 0; column < 4; column++)// ：列（column）を0から3まで順番に処理
                {
                    // GLTFの行列情報から、指定された行と列の要素を取得し、DirectXのmatrixの対応する位置に設定
                    // GLTFの行列は1次元の配列として格納されているため、指定された行と列の位置を計算して要素を取得
                    matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
                }
            }
            DirectX::XMVECTOR S, T, R;
            // 行列からscale,rotation,translationを取り出す
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

            // Float3に変換
            DirectX::XMStoreFloat3(&node.scale, S);
            DirectX::XMStoreFloat4(&node.rotation, R);
            DirectX::XMStoreFloat3(&node.translation, T);
        }
        else
        {
            // 行列情報がない場合は、scale, translation, rotationを直接設定
            // (gltf_node.scale, gltf_node.translation, gltf_node.rotationから値を取得して設定)
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

void GltfModel::CumulateTransforms(std::vector<node>& nodes)
{
    using namespace DirectX;

    // 親ノードのグローバル変換行列（transform）を保持するスタック
    // スタックは、最後に追加された要素（親ノードの変換行列）が最初に取り出されるデータ構造
    std::stack<XMFLOAT4X4> parent_global_transforms;
    std::function<void(int)> traverse{ [&](int node_index)->void
    {
        node& node{nodes.at(node_index)};

        // ノードのスケール、回転、平行移動を行列に変換
        XMMATRIX S{ XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
        XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

        // ノードのローカル変換行列を計算し、親のグローバル変換行列と積を取る
        XMStoreFloat4x4(&node.global_transform, S * R * T * XMLoadFloat4x4(&parent_global_transforms.top()));

        // 子ノードを再帰的に処理する
        for (int child_index : node.children)
        {
            // 親のグローバル変換行列をスタックに追加して子ノードを処理
            parent_global_transforms.push(node.global_transform);
            traverse(child_index);
            parent_global_transforms.pop(); // 子ノードの処理が終わったら親の行列をスタックから取り出す
        }
    } };

    // ノード階層構造をトラバースして、各ノードのグローバル変換行列を計算
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        parent_global_transforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
        traverse(node_index);
        parent_global_transforms.pop();
    }
}


GltfModel::buffer_view GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
    // データのフォーマットや格納方法
    buffer_view buffer_view;

    // アクセサのタイプ（データの形式）に応じて処理を分岐
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:// アクセサのデータ型がスカラー

        // アクセサのデータがどのようなデータ型（数値型）であるかに応じて処理を分岐
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:// 符号なし16ビット整数型

            buffer_view.format = DXGI_FORMAT_R16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT);

            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:// 符号なし32ビット整数型

            buffer_view.format = DXGI_FORMAT_R32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT);

            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC2:// アクセサのデータ型が2次元ベクトル

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// 浮動小数点数型のデータ
            buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC3:// アクセサのデータ型が3次元ベクトル

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// 浮動小数点数型のデータ
            buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC4:// アクセサのデータ型が4次元ベクトル

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:// 符号なし16ビット整数型
            buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:// 符号なし32ビット整数型
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// 浮動小数点数型のデータ
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

    // バッファビュー全体のサイズを計算
    buffer_view.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view.stride_in_bytes);
    return buffer_view;
}

void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr;

    // すべてのメッシュについてループ
    for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
    {
        mesh& mesh{ meshes.emplace_back() };// 新しいメッシュを meshes ベクターに追加し、その参照を取得します。

        mesh.name = gltf_mesh.name;

        // 各メッシュのプリミティブ（部品）についてループ
        for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
        {
            mesh::primitive& primitive{ mesh.primitives.emplace_back() };// メッシュ内のプリミティブを新しいプリミティブとして追加し、その参照を取得
            primitive.material = gltf_primitive.material;

            // インデックスバッファの作成
            // プリミティブ（部品）のインデックス情報を示すアクセサ（Accessor）を取得します。
            // このアクセサには、インデックスバッファの格納されているバッファビュー（BufferView）のインデックスが含まれる
            const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_primitive.indices) };
            // インデックスのバッファビューを取得
            // バッファビューは、バッファの配置やフォーマット情報を表す
            const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // glTF アクセサからインデックスのバッファビューを作成して、プリミティブのindex_buffer_viewに設定
            // これにより、バッファのフォーマットやサイズが設定される
            primitive.index_buffer_view = MakeBufferView(gltf_accessor);

            // バッファを作成するための設定
            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            // バッファに格納するデータを設定
            D3D11_SUBRESOURCE_DATA subresource_data{};
            subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
            // 実際の DirectX 11 のバッファを作成
            // これにより、GPU で使用可能なインデックスバッファが作成される
            hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());

            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


            // 頂点バッファーを作成
            for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
            {
                // 属性に対応するアクセサ（Accessor）を取得
                const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_attribute.second) };
                // アクセサからバッファビュー（BufferView）を取得
                const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

                // バッファビューから頂点バッファビューを作成
                buffer_view vertex_buffer_view{ MakeBufferView(gltf_accessor) };

                // DirectX 11 用のバッファを作成するための設定
                D3D11_BUFFER_DESC buffer_desc{};
                buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
                buffer_desc.Usage = D3D11_USAGE_DEFAULT;
                buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

                // バッファにコピーするデータを設定
                D3D11_SUBRESOURCE_DATA subresource_data{};
                subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                    + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;

                // DirectX 11 のバッファを作成
                hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                    vertex_buffer_view.buffer.ReleaseAndGetAddressOf());

                // エラーチェック
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                // プリミティブに頂点バッファビューを追加
                primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));
            }

            // ダミー属性がない場合は追加する
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

void GltfModel::FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    // 各マテリアルに対して処理を行う
    for (std::vector<tinygltf::Material>::const_reference gltf_material : gltf_model.materials)
    {
        // material構造体を materialsに追加し、その参照を取得
        std::vector<material>::reference material = materials.emplace_back();

        // マテリアルの名前が設定
        material.name = gltf_material.name;

        // 各種情報が、 material.data の対応するメンバーに設定
        material.data.emissive_factor[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
        material.data.emissive_factor[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
        material.data.emissive_factor[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

        material.data.alpha_mode = gltf_material.alphaMode == "OPAQUE" ? 0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
        material.data.alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
        material.data.double_sided = gltf_material.doubleSided ? 1 : 0;

        // 各テクスチャ情報も設定
        // それぞれのテクスチャ情報は、構造体を通じて格納
        material.data.pbr_metallic_roughness.basecolor_factor[0] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(0));
        material.data.pbr_metallic_roughness.basecolor_factor[1] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(1));
        material.data.pbr_metallic_roughness.basecolor_factor[2] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(2));
        material.data.pbr_metallic_roughness.basecolor_factor[3] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(3));
        material.data.pbr_metallic_roughness.basecolor_texture.index = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        material.data.pbr_metallic_roughness.basecolor_texture.texcoord = gltf_material.pbrMetallicRoughness.baseColorTexture.texCoord;
        material.data.pbr_metallic_roughness.metallic_factor = static_cast<float>(gltf_material.pbrMetallicRoughness.metallicFactor);
        material.data.pbr_metallic_roughness.roughness_factor = static_cast<float>(gltf_material.pbrMetallicRoughness.roughnessFactor);
        material.data.pbr_metallic_roughness.metallic_roughness_texture.index = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

        material.data.normal_texture.index = gltf_material.normalTexture.index;
        material.data.normal_texture.texcoord = gltf_material.normalTexture.texCoord;
        material.data.normal_texture.scale = static_cast<float>(gltf_material.normalTexture.scale);

        material.data.occlusion_texture.index = gltf_material.occlusionTexture.index;
        material.data.occlusion_texture.texcoord = gltf_material.occlusionTexture.texCoord;
        material.data.occlusion_texture.strength = static_cast<float>(gltf_material.occlusionTexture.strength);

        material.data.emissive_texture.index = gltf_material.emissiveTexture.index;
        material.data.emissive_texture.texcoord = gltf_material.emissiveTexture.texCoord;
    }

}

void GltfModel::FetchTextures(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr{ S_OK };

    // テクスチャ分ループ
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
            // gltfバッファからテクスチャデータ読み込み
            const tinygltf::BufferView& buffer_view{ gltf_model.bufferViews.at(gltf_image.bufferView) };
            const tinygltf::Buffer& buffer{ gltf_model.buffers.at(buffer_view.buffer) };
            const byte* data = buffer.data.data() + buffer_view.byteOffset;

            ID3D11ShaderResourceView* texture_resource_view{};
            hr = LoadTextureFromMemory(device, data, buffer_view.byteLength, &texture_resource_view);
            if (hr == S_OK)
            {
                // 読み込んだテクスチャをシェーダーリソースビューとして追加
                texture_resource_views.emplace_back().Attach(texture_resource_view);
            }
        }
        else
        {
            // バッファビューが存在しない場合は、ファイルからテクスチャデータを読み込む
            const std::filesystem::path path(filename);
            ID3D11ShaderResourceView* shader_resource_view{};
            D3D11_TEXTURE2D_DESC texture2d_desc;
            std::wstring filename{
            path.parent_path().concat(L"/").wstring() +
             std::wstring(gltf_image.uri.begin(), gltf_image.uri.end()) };
            hr = LoadTextureFromFile(device, filename.c_str(), &shader_resource_view, &texture2d_desc);
            if (hr == S_OK)
            {
                // 読み込んだテクスチャをシェーダーリソースビューとして追加
                texture_resource_views.emplace_back().Attach(shader_resource_view);
            }
        }
    }
}

void GltfModel::FetchAnimations(const tinygltf::Model& gltf_model)
{
    using namespace std;
    using namespace tinygltf;
    using namespace DirectX;

    for (vector<Skin>::const_reference transmission_skin : gltf_model.skins)
    {
        // 新しい skin オブジェクトを作成し、skins ベクターに追加する
        skin& skin{ skins.emplace_back() };

        // スキンに関連付けられた逆バインド行列 (inverseBindMatrices) の Accessor を取得する
        const Accessor& gltf_accessor{ gltf_model.accessors.at(transmission_skin.inverseBindMatrices) };

        // 対応する BufferView を取得する
        const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

        // 逆バインド行列のサイズを設定して、必要なメモリを確保する
        skin.inverse_bind_matrices.resize(gltf_accessor.count);

        // コピー元データから逆バインド行列データをコピーする
        memcpy(skin.inverse_bind_matrices.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4X4));

        // スキンに関連付けられたジョイントのリストを設定する
        skin.joints = transmission_skin.joints;
    }

    for (vector<Animation>::const_reference gltf_animation : gltf_model.animations)
    {
        // アニメーションを表す構造体を作成して、animationsベクターに追加
        animation& animation{ animations.emplace_back() };
        animation.name = gltf_animation.name; // アニメーションの名前を設定

        // アニメーションのサンプラーごとに処理
        for (vector<AnimationSampler>::const_reference gltf_sampler : gltf_animation.samplers)
        {
            animation::sampler& sampler{ animation.samplers.emplace_back() };
            sampler.input = gltf_sampler.input;// サンプラーの入力（時間軸）のインデックスを設定
            sampler.output = gltf_sampler.output;// サンプラーの出力（値）のインデックスを設定
            sampler.interpolation = gltf_sampler.interpolation; // 補間方法を設定

            // 入力に対応するアクセサ情報を取得
            const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.input) };
            const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // アニメーションのタイムラインを追加（まだ追加されていない場合）
            // タイムラインは入力（時間軸）に対応し、そのタイムラインには値の配列が保存される
            pair<unordered_map<int, vector<float>>::iterator, bool>& timelines{
            animation.timelines.emplace(gltf_sampler.input, gltf_accessor.count) };
            if (timelines.second)
            {
                // タイムラインのデータをバッファからコピー
                memcpy(timelines.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                    gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(FLOAT));
            }
        }

        // アニメーションのチャンネルごとに処理
        for (vector<AnimationChannel>::const_reference gltf_channel : gltf_animation.channels)
        {
            // アニメーションのチャンネルを表す構造体を作成して、channelsベクターに追加
            animation::channel& channel{ animation.channels.emplace_back() };

            channel.sampler = gltf_channel.sampler; // チャンネルが使用するサンプラーのインデックスを設定
            channel.target_node = gltf_channel.target_node;// チャンネルのターゲットノードのインデックスを設定
            channel.target_path = gltf_channel.target_path; // チャンネルのターゲットパス（scale, rotation, translation）を設定

            // チャンネルが指定するサンプラーに対応する情報を取得
            const AnimationSampler& gltf_sampler{ gltf_animation.samplers.at(gltf_channel.sampler) };
            const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.output) };
            const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // ターゲットパスごとに処理を分岐
            if (gltf_channel.target_path == "scale")
            {
                // スケール情報を追加（まだ追加されていない場合）
                pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& scales{
                animation.scales.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (scales.second)
                {
                    // スケールのデータをバッファからコピー
                    memcpy(scales.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
                }
            }
            else if (gltf_channel.target_path == "rotation")
            {
                // 回転情報を追加（まだ追加されていない場合）
                pair<unordered_map<int, vector<XMFLOAT4>>::iterator, bool>& rotations{
                animation.rotations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (rotations.second)
                {
                    // 回転のデータをバッファからコピー
                    memcpy(rotations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4));
                }
            }
            else if (gltf_channel.target_path == "translation")
            {
                // 移動情報を追加（まだ追加されていない場合）
                pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& translations{
                animation.translations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (translations.second)
                {
                    // 移動のデータをバッファからコピー
                    memcpy(translations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
                }
            }
        }
    }
}


void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes)
{
    using namespace DirectX;

    // シェーダーや入力レイアウトのセット
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());

    std::function<void(int)> traverse{ [&](int node_index)->void {

        //ノードを取得
       const node& node{nodes.at(node_index)};

       // ノードがスキンを持っているか
       if (node.skin > -1)
       {
           // ノードが参照するスキンを取得
           const skin& skin{ skins.at(node.skin) };

           // ジョイント行列を計算するための定数バッファーを初期化
           primitive_joint_constants primitive_joint_data{};

           // スキンに関連する各ジョイントについて処理
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // ジョイント行列の計算
               // 1. 逆バインド行列を取得してXMMatrixに変換
               // 2. ジョイントのグローバル変換行列を取得してXMMatrixに変換
               // 3. ノードのグローバル変換行列を逆行列化してXMMatrixに変換
               // 4. ジョイント行列の計算：逆バインド行列 * ジョイントのグローバル変換行列 * ノードのグローバル変換行列の逆行列
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // 定数バッファーを更新：ジョイント行列データをシェーダーの定数バッファーにコピー
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // 頂点シェーダーに定数バッファーを設定
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // ノードがメッシュを持っているか
       if (node.mesh > -1)
       {
           // ノードが参照するメッシュを取得
           const mesh& mesh{ meshes.at(node.mesh) };

           // メッシュ内の各プリミティブに対してループ
           // 各プリミティブは、頂点バッファやインデックスバッファ、マテリアルなどの情報を持っている
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // 各プリミティブの頂点バッファを配列として取得
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // 頂点バッファのサイズを取得
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // 頂点バッファを入力として設定
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // プリミティブのインデックスバッファを設定
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // プリミティブのマテリアルや変換行列などの情報を設定
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));


               // 定数バッファに primitive_data の内容をコピー
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // マテリアル内の各テクスチャに関連付けられたインデックスを配列として取得
               const int texture_indices[]
               {
                   material.data.pbr_metallic_roughness.basecolor_texture.index,
                   material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
                   material.data.normal_texture.index,
                   material.data.emissive_texture.index,
                   material.data.occlusion_texture.index,
               };

               ID3D11ShaderResourceView* null_shader_resource_view{};
               std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

               // テクスチャのインデックスをループし、各テクスチャに対応するシェーダーリソースビューを設定
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // ピクセルシェーダーにシェーダーリソースビューを設定
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // プリミティブを描画
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // 各ノードが持つメッシュを描画
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // 各ノードが持つメッシュを描画
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader* replaced_pixel_shader)
{
    using namespace DirectX;

    // シェーダーや入力レイアウトのセット
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    if (replaced_pixel_shader)
    {
        immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
    }
    else
    {
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    }
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());

    std::function<void(int)> traverse{ [&](int node_index)->void {

        //ノードを取得
       const node& node{nodes.at(node_index)};

       // ノードがスキンを持っているか
       if (node.skin > -1)
       {
           // ノードが参照するスキンを取得
           const skin& skin{ skins.at(node.skin) };

           // ジョイント行列を計算するための定数バッファーを初期化
           primitive_joint_constants primitive_joint_data{};

           // スキンに関連する各ジョイントについて処理
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // ジョイント行列の計算
               // 1. 逆バインド行列を取得してXMMatrixに変換
               // 2. ジョイントのグローバル変換行列を取得してXMMatrixに変換
               // 3. ノードのグローバル変換行列を逆行列化してXMMatrixに変換
               // 4. ジョイント行列の計算：逆バインド行列 * ジョイントのグローバル変換行列 * ノードのグローバル変換行列の逆行列
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // 定数バッファーを更新：ジョイント行列データをシェーダーの定数バッファーにコピー
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // 頂点シェーダーに定数バッファーを設定
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // ノードがメッシュを持っているか
       if (node.mesh > -1)
       {
           // ノードが参照するメッシュを取得
           const mesh& mesh{ meshes.at(node.mesh) };

           // メッシュ内の各プリミティブに対してループ
           // 各プリミティブは、頂点バッファやインデックスバッファ、マテリアルなどの情報を持っている
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // 各プリミティブの頂点バッファを配列として取得
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // 頂点バッファのサイズを取得
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // 頂点バッファを入力として設定
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // プリミティブのインデックスバッファを設定
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // プリミティブのマテリアルや変換行列などの情報を設定
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));

               // 定数バッファに primitive_data の内容をコピー
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // マテリアル内の各テクスチャに関連付けられたインデックスを配列として取得
               const int texture_indices[]
               {
                   material.data.pbr_metallic_roughness.basecolor_texture.index,
                   material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
                   material.data.normal_texture.index,
                   material.data.emissive_texture.index,
                   material.data.occlusion_texture.index,
               };

               ID3D11ShaderResourceView* null_shader_resource_view{};
               std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

               // テクスチャのインデックスをループし、各テクスチャに対応するシェーダーリソースビューを設定
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // ピクセルシェーダーにシェーダーリソースビューを設定
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // プリミティブを描画
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // 各ノードが持つメッシュを描画
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // 各ノードが持つメッシュを描画
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader** replaced_pixel_shader)
{
    using namespace DirectX;

    // シェーダーや入力レイアウトのセット
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    if (replaced_pixel_shader)
    {
        immediate_context->PSSetShader(*replaced_pixel_shader, nullptr, 0);
    }
    else
    {
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    }
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());

    std::function<void(int)> traverse{ [&](int node_index)->void {

        //ノードを取得
       const node& node{nodes.at(node_index)};

       // ノードがスキンを持っているか
       if (node.skin > -1)
       {
           // ノードが参照するスキンを取得
           const skin& skin{ skins.at(node.skin) };

           // ジョイント行列を計算するための定数バッファーを初期化
           primitive_joint_constants primitive_joint_data{};

           // スキンに関連する各ジョイントについて処理
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // ジョイント行列の計算
               // 1. 逆バインド行列を取得してXMMatrixに変換
               // 2. ジョイントのグローバル変換行列を取得してXMMatrixに変換
               // 3. ノードのグローバル変換行列を逆行列化してXMMatrixに変換
               // 4. ジョイント行列の計算：逆バインド行列 * ジョイントのグローバル変換行列 * ノードのグローバル変換行列の逆行列
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // 定数バッファーを更新：ジョイント行列データをシェーダーの定数バッファーにコピー
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // 頂点シェーダーに定数バッファーを設定
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // ノードがメッシュを持っているか
       if (node.mesh > -1)
       {
           // ノードが参照するメッシュを取得
           const mesh& mesh{ meshes.at(node.mesh) };

           // メッシュ内の各プリミティブに対してループ
           // 各プリミティブは、頂点バッファやインデックスバッファ、マテリアルなどの情報を持っている
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // 各プリミティブの頂点バッファを配列として取得
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // 頂点バッファのサイズを取得
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // 頂点バッファを入力として設定
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // プリミティブのインデックスバッファを設定
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // プリミティブのマテリアルや変換行列などの情報を設定
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));

               // 定数バッファに primitive_data の内容をコピー
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // マテリアル内の各テクスチャに関連付けられたインデックスを配列として取得
               const int texture_indices[]
               {
                   material.data.pbr_metallic_roughness.basecolor_texture.index,
                   material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
                   material.data.normal_texture.index,
                   material.data.emissive_texture.index,
                   material.data.occlusion_texture.index,
               };

               ID3D11ShaderResourceView* null_shader_resource_view{};
               std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

               // テクスチャのインデックスをループし、各テクスチャに対応するシェーダーリソースビューを設定
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // ピクセルシェーダーにシェーダーリソースビューを設定
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // プリミティブを描画
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // 各ノードが持つメッシュを描画
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // 各ノードが持つメッシュを描画
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void GltfModel::Animate(size_t animation_index, float time, std::vector<node>& animated_nodes, bool loopback)
{
    using namespace std;
    using namespace DirectX;

    if (animations.size() > 0)
    {
        // 指定されたアニメーションインデックスに対応するアニメーションを取得
        const animation& animation{ animations.at(animation_index) };

        // アニメーションの各チャンネルに対して処理を行う
        for (vector<animation::channel>::const_reference channel : animation.channels)
        {
            // チャンネルに対応するサンプラーを取得
            const animation::sampler& sampler{ animation.samplers.at(channel.sampler) };

            // サンプラーの入力となるタイムラインを取得
            const vector<float>& timeline{ animation.timelines.at(sampler.input) };

            // タイムラインのサイズが0の場合はスキップ（不正なデータの可能性があるため）
            if (timeline.size() == 0)
            {
                continue;
            }

            // タイムラインから指定された時間に対応するキーフレームを取得
            float interpolation_factor{};
            size_t keyframe_index{ IndexOf(timeline, time, interpolation_factor, loopback) };

            // チャンネルのターゲットパスに基づいてノードの変換を計算
            if (channel.target_path == "scale")
            {
                // スケールに対応するキーフレームを取得
                const vector<XMFLOAT3>& scales{ animation.scales.at(sampler.output) };

                // 補間を使ってスケールを計算し、アニメーションノードに設定
                XMStoreFloat3(&animated_nodes.at(channel.target_node).scale, 
                    XMVectorLerp(XMLoadFloat3(&scales.at(keyframe_index + 0)),
                        XMLoadFloat3(&scales.at(keyframe_index + 1)), interpolation_factor));
            }
            else if (channel.target_path == "rotation")
            {
                // 回転に対応するキーフレームを取得
                const vector<XMFLOAT4>& rotations{ animation.rotations.at(sampler.output) };

                // 補間を使って回転を計算し、アニメーションノードに設定
                XMStoreFloat4(&animated_nodes.at(channel.target_node).rotation,
                    XMQuaternionNormalize(XMQuaternionSlerp(XMLoadFloat4(&rotations.at(keyframe_index + 0)),
                        XMLoadFloat4(&rotations.at(keyframe_index + 1)), interpolation_factor)));
            }
            else if (channel.target_path == "translation")
            {
                // 位置に対応するキーフレームを取得
                const vector<XMFLOAT3>& translations{ animation.translations.at(sampler.output) };

                // 補間を使って位置を計算し、アニメーションノードに設定
                XMStoreFloat3(&animated_nodes.at(channel.target_node).translation,
                    XMVectorLerp(XMLoadFloat3(&translations.at(keyframe_index + 0)),
                        XMLoadFloat3(&translations.at(keyframe_index + 1)), interpolation_factor));
            }
        }
        // アニメーション後のノードの変換を累積する
        CumulateTransforms(animated_nodes);
    }
    else
    {
        // アニメーションがない場合は、元のノードの状態をそのまま使う
        animated_nodes = nodes;
    }
}


HRESULT GltfModel::LoadTextures(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color)
{
    // パスを分解
    char drive[256], dirname[256], fname[256], ext[256];
    ::_splitpath_s(filename, drive, sizeof(drive), dirname, sizeof(dirname), fname, sizeof(fname), ext, sizeof(ext));

    // 末尾文字を追加
    if (suffix != nullptr)
    {
        ::strcat_s(fname, sizeof(fname), suffix);
    }
    // パスを結合
    char filepath[256];
    ::_makepath_s(filepath, 256, drive, dirname, fname, ext);

    // マルチバイト文字からワイド文字へ変換
    wchar_t wfilepath[256];
    ::MultiByteToWideChar(CP_ACP, 0, filepath, -1, wfilepath, 256);

    // テクスチャ読み込み
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilepath, resource.GetAddressOf(), srv);
    if (FAILED(hr))
    {
        // WICでサポートされていないフォーマットの場合（TGAなど）は
        // STBで画像読み込みをしてテクスチャを生成する
        int width, height, bpp;
        unsigned char* pixels = stbi_load(filepath, &width, &height, &bpp, STBI_rgb_alpha);
        if (pixels != nullptr)
        {
            D3D11_TEXTURE2D_DESC desc = { 0 };
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            D3D11_SUBRESOURCE_DATA data;
            ::memset(&data, 0, sizeof(data));
            data.pSysMem = pixels;
            data.SysMemPitch = width * 4;

            Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
            hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv);
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            // 後始末
            stbi_image_free(pixels);
        }
        else if (dummy)
        {
            // 読み込み失敗したらダミーテクスチャを作る
            //LOG("load failed : %s\n", filepath);

            const int width = 8;
            const int height = 8;
            UINT pixels[width * height];
            for (int yy = 0; yy < height; ++yy)
            {
                for (int xx = 0; xx < width; ++xx)
                {
                    pixels[yy * width + xx] = dummy_color;
                }
            }

            D3D11_TEXTURE2D_DESC desc = { 0 };
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            D3D11_SUBRESOURCE_DATA data;
            ::memset(&data, 0, sizeof(data));
            data.pSysMem = pixels;
            data.SysMemPitch = width;

            Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
            hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv);
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }
    }
    return hr;
}

size_t GltfModel::IndexOf(const std::vector<float>& timelines, float time, float& interpolation_factor, bool loopback)
{  
    // タイムラインのキーフレーム数を取得
    const size_t keyframe_count{ timelines.size() };

    // 時間が最後のキーフレームを超えた場合の処理
    if (time > timelines.at(keyframe_count - 1))
    {
        if (loopback)
        {
            // ループする場合は時間をループさせる
            time = fmodf(time, timelines.at(keyframe_count - 1));
        }
        else
        {
            // ループしない場合は最後のキーフレームの直前の位置を返す
            interpolation_factor = 1.0f;
            end_flag = true;
            return keyframe_count - 2;  // 最後のキーフレームの直前のインデックスを返す
        }
    }
    // 時間が最初のキーフレームよりも前の場合の処理
    else if (time < timelines.at(0))
    {
        // 最初のキーフレームの位置を返す
        interpolation_factor = 0.0f;
        return 0;
    }

    // 最も近いキーフレームを探す
    size_t keyframe_index{ 0 };
    for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
    {
        if (time < timelines.at(time_index))
        {
            // 時間が現在のキーフレームよりも小さい場合、その直前のキーフレームを選択
            keyframe_index = std::max<size_t>(0LL, time_index - 1);
            break;
        }
    }

    // 補間係数を計算
    interpolation_factor = (time - timelines.at(keyframe_index + 0)) /
        (timelines.at(keyframe_index + 1) - timelines.at(keyframe_index + 0));

    // 最も近いキーフレームのインデックスを返す
    return keyframe_index;
}

void GltfModel::FetchExternalTextures(ID3D11Device* device, const tinygltf::Model& gltf_model, const std::string& gltf_filename)
{
    for (auto& gltf_material : materials)
    {
        // ディレクトリパス取得
        char drive[32], dir[256], dirname[256], fname[256];
        ::_splitpath_s(gltf_filename.c_str(), drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

        // ファイル階層を追加
        ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

        // マテリアルの名前
        char tname[256] = {};
        // std::string（gltf_material.name）をchar配列（tname）にコピー
        std::copy(gltf_material.name.begin(), gltf_material.name.end(), tname);

        // 末尾文字を追加
        ::strcat_s(tname, sizeof(tname), ".png");

        // 相対パスの解決
        char filename[256];
        ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);


        // ベースカラー
        char filename_base[256] = {};
        strcpy_s(filename_base, filename);
        ID3D11ShaderResourceView* base_color{};
        LoadTextures(device, filename_base, "_BaseColor", true, &base_color, 0x00FFFF00);
        texture_resource_views.emplace_back().Attach(base_color);
        gltf_material.data.pbr_metallic_roughness.basecolor_texture.index = index_count;

        texture& texture_b{ textures.emplace_back() };
        texture_b.name = filename_base;
        texture_b.source = index_count;
        index_count++;

        // ノーマル
        char filename_normal[256] = {};
        strcpy_s(filename_normal, filename);
        ID3D11ShaderResourceView* normal{};
        LoadTextures(device, filename_normal, "_Normal", true, &normal, 0x00FFFF00);
        texture_resource_views.emplace_back().Attach(normal);
        gltf_material.data.normal_texture.index = index_count;

        texture& texture_n{ textures.emplace_back() };
        texture_n.name = filename_normal;
        texture_n.source = index_count;
        index_count++;

        //TODO 今回ラフネスのみなのでラフネス
        // メタルネスいるときは書き換え
        char filename_pbr[256] = {};
        strcpy_s(filename_pbr, filename);
        ID3D11ShaderResourceView* pbr{};
        LoadTextures(device, filename_pbr, "_Roughness", true, &pbr, 0x00FFFF00);
        texture_resource_views.emplace_back().Attach(pbr);
        gltf_material.data.pbr_metallic_roughness.metallic_roughness_texture.index = index_count;

        texture& texture_pbr{ textures.emplace_back() };
        texture_pbr.name = filename_pbr;
        texture_pbr.source = index_count;
        index_count++;
    }


}

void GltfModel::MaterialForGPU(ID3D11Device* device)
{
    // GPU上のシェーダー・リソース・ビューとしてマテリアル・データを作成する
    std::vector<material::cbuffer> material_data;
    for (std::vector<material>::const_reference material : materials)
    {
        material_data.emplace_back(material.data);
    }

    HRESULT hr;
    // GPU 上でマテリアルデータを格納するためのバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;

    D3D11_BUFFER_DESC buffer_desc{};
    // バッファのサイズをバイト単位
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(material::cbuffer) * material_data.size());
    // 要素の構造体サイズ
    buffer_desc.StructureByteStride = sizeof(material::cbuffer);
    // バッファの使用方法
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    // バッファのバインド方法
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;// シェーダーリソースとしてバインド
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = material_data.data();
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // シェーダーリソースビューの設定を行うための構造体
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    // バッファのフォーマット
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    // ビューの次元
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    // バッファ内の要素数
    shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(material_data.size());

    hr = device->CreateShaderResourceView(material_buffer.Get(),
        &shader_resource_view_desc, material_resource_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GltfModel::DebugThreshold()
{
    //ImGui::Begin("mask");
    //ImGui::SliderFloat("threshold", &threshold, 0.0f, 1.0f);
    //ImGui::End();
}

void GltfModel::BlendAnimation(const std::vector<node>* nodes_[2], float factor, std::vector<GltfModel::node>* node_)
{
    using namespace DirectX;
    _ASSERT_EXPR(nodes_[0]->size() == nodes_[1]->size(), "The size of the two node arrays must be the same.");

    size_t node_count{ nodes_[0]->size() };
    node_->resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{ XMLoadFloat3(&nodes_[0]->at(node_index).scale), XMLoadFloat3(&nodes_[1]->at(node_index).scale) };
        XMStoreFloat3(&node_->at(node_index).scale, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{ XMLoadFloat4(&nodes_[0]->at(node_index).rotation), XMLoadFloat4(&nodes_[1]->at(node_index).rotation) };
        XMStoreFloat4(&node_->at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2]{ XMLoadFloat3(&nodes_[0]->at(node_index).translation), XMLoadFloat3(&nodes_[1]->at(node_index).translation) };
        XMStoreFloat3(&node_->at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
    }
    // アニメーション後のノードの変換を累積する
    CumulateTransforms(*node_);
}

