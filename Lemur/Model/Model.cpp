#include "misc.h"
#include "Model.h"

#include <sstream>
#include <functional>

using namespace DirectX;

#include "./Lemur/Graphics/Shader.h"

#include <filesystem>
#include "./Lemur/Graphics/Texture.h"

#include <fstream>
#include <WICTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <DDSTextureLoader.h>

#include "ImGuiCtrl.h"

inline XMFLOAT4X4  to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
    XMFLOAT4X4 xmfloat4x4;
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
    XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
    XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}
// ボーン影響度
struct bone_influence
{
    uint32_t bone_index;
    float bone_weight;
};
using bone_influences_per_control_point = std::vector<bone_influence>;

// ボーン影響度を FBX データから取得
void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };
    bone_influences.resize(control_points_count);

    // Mesh→Skin
    const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
    {
        // Mesh→Skin
        const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

        const int cluster_count{ fbx_skin->GetClusterCount() };
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // このメッシュに影響するボーンの数はFbxClusterの数
            const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
            {
#if 1
                int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                double control_point_weight{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
                bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
                bone_influence.bone_weight = static_cast<float>(control_point_weight);
#else
                bone_influences.at((fbx_cluster->GetControlPointIndices())[control_point_indices_index]).emplace_back()
                    = { static_cast<uint32_t>(cluster_index),  static_cast<float>((fbx_cluster->GetControlPointWeights())[control_point_indices_index]) };
#endif
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------
//  ↓ メンバ関数
//-----------------------------------------------------------------------------------------------------------

// コンストラクタ共通部
void SkinnedMesh::FetchScene(const char* fbx_filename, bool triangulate, float sampling_rate)
{
    // 全体の統括
    FbxManager* fbx_manager{ FbxManager::Create() };
    // FbxImporterを元に初期化することで.fbxファイルの内容を構築する
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
    // fbxファイルを読み込みデータを保持する
    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    // シーングラフ全体をシーケンスコンテナにシリアライズする
    std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node)
        {
#if 0
            if (fbx_node->GetNodeAttribute())
            {
                switch (fbx_node->GetNodeAttribute()->GetAttributeType())
                {
                case FbxNodeAttribute::EType::eNull:
                case FbxNodeAttribute::EType::eMesh:
                case FbxNodeAttribute::EType::eSkeleton:
                case FbxNodeAttribute::EType::eUnknown:
                case FbxNodeAttribute::EType::eMarker:
                case FbxNodeAttribute::EType::eCamera:
                case FbxNodeAttribute::EType::eLight:
                    scene::node& node{ scene_view.nodes.emplace_back() };
                    node.attribute = fbx_node->GetNodeAttribute()->GetAttributeType();
                    node.name = fbx_node->GetName();
                    node.unique_id = fbx_node->GetUniqueID();
                    node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
                    break;
                }
            }
#else
            Scene::node& node{ scene_view.nodes.emplace_back() };
            node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
            node.name = fbx_node->GetName();
            node.unique_id = fbx_node->GetUniqueID();
            node.parent_index = scene_view.Indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
#endif
            for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
            {
                traverse(fbx_node->GetChild(child_index));
            }
    } };
    traverse(fbx_scene->GetRootNode());


#if 0
    for (const scene::node& node : scene_view.nodes)
    {
        // ツリー構造のノードオブジェクト
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        // Display node data in the output window as debug
        std::string node_name = fbx_node->GetName();
        uint64_t uid = fbx_node->GetUniqueID();
        uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
        int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;
        std::stringstream debug_string;
        debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
        OutputDebugStringA(debug_string.str().c_str());
    }
#endif
    FetchMeshes(fbx_scene, meshes);
    FetchMaterials(fbx_scene, materials);
    FetchAnimations(fbx_scene, animation_clips, sampling_rate);

    // マネージャーの破棄（Scene等もまとめて破棄される）
    fbx_manager->Destroy();
}

// 通常
SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate/*UNIT.25*/, bool used_as_collider/*RAYCAST*/)
{
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        FetchScene(fbx_filename, triangulate, sampling_rate);

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scene_view, meshes, materials, animation_clips);
    }
    CreateComObjects(device, fbx_filename, used_as_collider/*RAYCAST*/);
    dissolve = 1.0f;
}

// アペンドアニメーション
SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate, float sampling_rate)
{
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        FetchScene(fbx_filename, triangulate, sampling_rate);

        for (const std::string animation_filename : animation_filenames)
        {
            AppendAnimations(animation_filename.c_str(), sampling_rate);
        }

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scene_view, meshes, materials, animation_clips);
    }
    CreateComObjects(device, fbx_filename, false);
}


void SkinnedMesh::UpdateAnimation(Animation::keyframe& keyframe)
{
    size_t node_count{ keyframe.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        Animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

        int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
        DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() :
        DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

        DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
    }
}

void SkinnedMesh::CreateComObjects(ID3D11Device* device, const char* fbx_filename, bool used_as_collider/*RAYCAST*/)
{
    for (mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        // RAYCAST
        if (!used_as_collider)
        {
            mesh.vertices.clear();
            mesh.indices.clear();
        }
    }
    // マテリアル
    for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
        //TODO EMISSIVE
        D3D11_TEXTURE2D_DESC texture2d_desc;
        // Diffuse
        if (iterator->second.texture_filenames[0].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[0]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            // ディレクトリパス取得
            char drive[32], dir[256], dirname[256], fname[256];
            ::_splitpath_s(fbx_filename, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

            // ファイル階層を追加
            ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

            // マテリアルの名前
            char tname[256] = {};
            // std::string（gltf_material.name）をchar配列（tname）にコピー
            std::copy(iterator->second.name.begin(), iterator->second.name.end(), tname);

            // 末尾文字を追加
            ::strcat_s(tname, sizeof(tname), ".DDS");

            // 相対パスの解決
            char filename[256];
            ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);

            LoadTexture(device, filename, "_BaseColor", true, iterator->second.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF);
        }
        // Normal
        if (iterator->second.texture_filenames[1].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[1]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            // ディレクトリパス取得
            char drive[32], dir[256], dirname[256], fname[256];
            ::_splitpath_s(fbx_filename, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

            // ファイル階層を追加
            ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

            // マテリアルの名前
            char tname[256] = {};
            // std::string（gltf_material.name）をchar配列（tname）にコピー
            std::copy(iterator->second.name.begin(), iterator->second.name.end(), tname);

            // 末尾文字を追加
            ::strcat_s(tname, sizeof(tname), ".DDS");

            // 相対パスの解決
            char filename[256];
            ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);

            LoadTexture(device, filename, "_Normal", true, iterator->second.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F);

        }
        // Emissive
        if (iterator->second.texture_filenames[2].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[2]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[2].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            // ディレクトリパス取得
            char drive[32], dir[256], dirname[256], fname[256];
            ::_splitpath_s(fbx_filename, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

            // ファイル階層を追加
            ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

            // マテリアルの名前
            char tname[256] = {};
            // std::string（gltf_material.name）をchar配列（tname）にコピー
            std::copy(iterator->second.name.begin(), iterator->second.name.end(), tname);

            // 末尾文字を追加
            ::strcat_s(tname, sizeof(tname), ".DDS");

            // 相対パスの解決
            char filename[256];
            ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);

            LoadTexture(device, filename, "_Emissive", true, iterator->second.shader_resource_views[2].GetAddressOf(), 0xFF000000);
        }


        // ディレクトリパス取得
        char drive[32], dir[256], dirname[256], fname[256];
        ::_splitpath_s(fbx_filename, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

        // ファイル階層を追加
        ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

        // マテリアルの名前
        char tname[256] = {};
        // std::string（gltf_material.name）をchar配列（tname）にコピー
        std::copy(iterator->second.name.begin(), iterator->second.name.end(), tname);

        // 末尾文字を追加
        ::strcat_s(tname, sizeof(tname), ".DDS");

        // 相対パスの解決
        char filename[256];
        ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);

        //TODO materil実験
        LoadTexture(device, filename, "_MS", true, iterator->second.metalness_smoothness.GetAddressOf(), 0x00FFFF00);
        LoadTexture(device, filename, "_Metalness", true, iterator->second.metalness.GetAddressOf(), 0x0000000);
        LoadTexture(device, filename, "_Roughness", true, iterator->second.roughness.GetAddressOf(), 0xFFFFFFF);
        LoadTexture(device, filename, "_Opacity", true, iterator->second.opacity.GetAddressOf(), 0xFFFFFFFF);
        LoadTexture(device, filename, "_Height", true, iterator->second.height.GetAddressOf(), 0xFFFFFFFF);
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // UNIT.29
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
    };

    create_vs_from_cso(device, "Shader/skinned_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "Shader/skinned_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void SkinnedMesh::Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color, const Animation::keyframe* keyframe/*UNIT.25*/, ID3D11PixelShader* replaced_pixel_shader)
{
    for (const mesh& mesh : meshes)
    {
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        if (replaced_pixel_shader)
        {
            immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
        }
        else
        {
          //  immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
        }

        constants data;
        data.threshold = dissolve;

        if (keyframe && keyframe->nodes.size() > 0)
        {
            // メッシュ全体が動くように変更
            //XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
            const Animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

            const size_t bone_count{ mesh.bind_pose.bones.size() };
            _ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

            for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const Skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
                const Animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
                XMStoreFloat4x4(&data.bone_transforms[bone_index],
                    XMLoadFloat4x4(&bone.offset_transform) *
                    XMLoadFloat4x4(&bone_node.global_transform) *
                    XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
                );
            }
        }
        else
        {
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

            // Bind dummy bone transforms
            for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
            {
                data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
            }
        }

        for (const mesh::subset& subset : mesh.subsets)
        {
            const Material& material{ materials.at(subset.material_unique_id) };

            // material 構造体のメンバ変数 Kd と  material_color を合成する
            XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));;

            // サブリソースにデータをコピー
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
            immediate_context->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

            // テクスチャを設定する
            immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
            // 法線マップのシェーダーリソースビューをバインド
            immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());
            //TODO EMISSIVE
            immediate_context->PSSetShaderResources(2, 1, material.shader_resource_views[2].GetAddressOf());
            //TODO material実験
            immediate_context->PSSetShaderResources(3, 1, material.metalness_smoothness.GetAddressOf());
            immediate_context->PSSetShaderResources(4, 1, material.metalness.GetAddressOf());
            immediate_context->PSSetShaderResources(5, 1, material.roughness.GetAddressOf());
            immediate_context->PSSetShaderResources(6, 1, material.opacity.GetAddressOf());
            immediate_context->PSSetShaderResources(7, 1, material.height.GetAddressOf());


            // 使用するインデックスバッファの範囲を指定して描画する
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }

}

void SkinnedMesh::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, const Animation::keyframe* keyframe, ID3D11PixelShader** replaced_pixel_shader)
{
    for (const mesh& mesh : meshes)
    {
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        if (replaced_pixel_shader)
        {
            immediate_context->PSSetShader(*replaced_pixel_shader, nullptr, 0);
        }
        else
        {
            immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
        }

        constants data;
        if (keyframe && keyframe->nodes.size() > 0)
        {
            // メッシュ全体が動くように変更
            //XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
            const Animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

            const size_t bone_count{ mesh.bind_pose.bones.size() };
            _ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

            for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const Skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
                const Animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
                XMStoreFloat4x4(&data.bone_transforms[bone_index],
                    XMLoadFloat4x4(&bone.offset_transform) *
                    XMLoadFloat4x4(&bone_node.global_transform) *
                    XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
                );
            }
        }
        else
        {
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

            // Bind dummy bone transforms
            for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
            {
                data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
            }
        }

        for (const mesh::subset& subset : mesh.subsets)
        {
            const Material& material{ materials.at(subset.material_unique_id) };

            // material 構造体のメンバ変数 Kd と  material_color を合成する
            XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));;

            data.threshold = this->dissolve;

            // サブリソースにデータをコピー
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
            immediate_context->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

            // テクスチャを設定する
            immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
            // 法線マップのシェーダーリソースビューをバインド
            immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());
            //TODO EMISSIVE
            immediate_context->PSSetShaderResources(2, 1, material.shader_resource_views[2].GetAddressOf());
            //TODO material実験
            immediate_context->PSSetShaderResources(3, 1, material.metalness_smoothness.GetAddressOf());
            immediate_context->PSSetShaderResources(4, 1, material.metalness.GetAddressOf());
            immediate_context->PSSetShaderResources(5, 1, material.roughness.GetAddressOf());


            // 使用するインデックスバッファの範囲を指定して描画する
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}

HRESULT SkinnedMesh::LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color)
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
    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, wfilepath, resource.GetAddressOf(), srv);
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

void SkinnedMesh::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    for (const Scene::node& node : scene_view.nodes)
    {
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }
        // このノードから様々なデータにアクセスできる
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        // 頂点データやポリゴン情報、スキン情報を内包している
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        mesh& mesh{ meshes.emplace_back() };
        mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        mesh.name = fbx_mesh->GetNode()->GetName();
        mesh.node_index = scene_view.Indexof(mesh.unique_id);
        mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

        // 頂点が影響を受けるボーン番号とウエイト値をセットする準備
        std::vector<bone_influences_per_control_point> bone_influences;
        fetch_bone_influences(fbx_mesh, bone_influences);
        FetchSkeleton(fbx_mesh, mesh.bind_pose);

        // 各素材のサブセットを構築する
        std::vector<mesh::subset>& subsets{ mesh.subsets };
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        subsets.resize(material_count > 0 ? material_count : 1);
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
            subsets.at(material_index).material_name = fbx_material->GetName();
            subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
        }
        if (material_count > 0)
        {
            // 各素材の面を数える
            const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
            {
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                subsets.at(material_index).index_count += 3;
            }

            // オフセットを記録する（何頂点か）
            uint32_t offset{ 0 };
            for (mesh::subset& subset : subsets)
            {
                subset.start_index_location = offset;
                offset += subset.index_count;
                // これは次の手順でカウンターとして使用され、ゼロにリセットされます。
                subset.index_count = 0;
            }
        }

        // ポリゴン数を取得する
        const int polygon_count{ fbx_mesh->GetPolygonCount() };
        mesh.vertices.resize(polygon_count * 3LL);
        mesh.indices.resize(polygon_count * 3LL);
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);
        // コントロールポイント（頂点座標）を取得する
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };

        // サブセット毎の頂点バッファを作る
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };

            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                // インデックス
                const int vertex_index{ polygon_index * 3 + position_in_polygon };
                // 頂点データ
                vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
                vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                // 頂点が影響を受けるボーン番号とウエイト値をセット
                const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
                {
                    if (influence_index < MAX_BONE_INFLUENCES)
                    {
                        vertex.bone_weights[influence_index] = influences_per_control_point.at(influence_index).bone_weight;
                        vertex.bone_indices[influence_index] = influences_per_control_point.at(influence_index).bone_index;
                    }
#if 1
                    else
                    {
                        size_t minimum_value_index = 0;
                        float minimum_value = FLT_MAX;
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            if (minimum_value > vertex.bone_weights[i])
                            {
                                minimum_value = vertex.bone_weights[i];
                                minimum_value_index = i;
                            }
                        }
                        vertex.bone_weights[minimum_value_index] += influences_per_control_point.at(influence_index).bone_weight;
                        vertex.bone_indices[minimum_value_index] = influences_per_control_point.at(influence_index).bone_index;
                    }
                }
#endif

                //if (fbx_mesh->GetElementNormalCount() > 0)
                if (fbx_mesh->GenerateNormals(false))
                {
                    // ポリゴンを構成するための頂点データを取得する
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                // 法線ベクトルの値を取得
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    _ASSERT_EXPR(tangent->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
                        tangent->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eDirect,
                        L"Only supports a combination of these modes.");

                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }
                mesh.vertices.at(vertex_index) = std::move(vertex);
                mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
        for (const vertex& v : mesh.vertices)
        {
            mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
            mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
            mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
            mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
            mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
            mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
        }
    }
}

void SkinnedMesh::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        const Scene::node& node{ scene_view.nodes.at(node_index) };
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            // FbxSurfaceMaterialを取り出す
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            Material material;
            material.name = fbx_material->GetName();
            material.unique_id = fbx_material->GetUniqueID();
            // ディフューズ、アンビエント、スペキュラ(拡散と反射)などのプロパティを取得
            FbxProperty fbx_property;
            // 引数の指定で任意のプロパティ(カラーデータなどを取り出せる)を検索する
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (fbx_property.IsValid())
            {
                // プロパティから色やテクスチャファイルパスを取得
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;
                // テクスチャファイルパスを取り出せる
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }
            // 法線マップのファイル名を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[1] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            //TODO EMISSIVE
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[2] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }

            materials.emplace(material.unique_id, std::move(material));
        }
    }
#if 1
    // Append default(dummy) material
    materials.emplace();
#endif
}

void SkinnedMesh::FetchSkeleton(FbxMesh* fbx_mesh, Skeleton& bind_pose)
{
    const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        const int cluster_count = skin->GetClusterCount();
        bind_pose.bones.resize(cluster_count);
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            FbxCluster* cluster = skin->GetCluster(cluster_index);

            Skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
            bone.name = cluster->GetLink()->GetName();
            bone.unique_id = cluster->GetLink()->GetUniqueID();
            bone.parent_index = bind_pose.Indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.node_index = scene_view.Indexof(bone.unique_id);

            // モデル(メッシュ)のローカル空間からに変換するために使用される
            // シーンのグローバル空間
            FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);

            // ボーンのローカル空間からボーンのローカル空間への変換に使用
            // シーンのグローバル空間
            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);

            // 行列は列Major schemeを使用して定義される
            // FbxAMatrix が変換を表す場合
            // 平行移動、回転、スケール)、行列の最後の行は、次の平行移動部分を表します
            // the transformation.
            // メッシュ空間からボーン空間に位置を変換する「bone.offset_transform」マトリックスを作成
            // この行列はオフセット行列と呼ばれる
            bone.offset_transform = to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
        }
    }
}

void SkinnedMesh::FetchAnimations(FbxScene* fbx_scene, std::vector<Animation>& animation_clips, float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    const int animation_stack_count{ animation_stack_names.GetCount() };
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        Animation& animation_clip{ animation_clips.emplace_back() };
        animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        // １秒をFbxTimeに変換
        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);// 左から時、分、秒
        animation_clip.sampling_rate = sampling_rate > 0 ? sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
        const FbxTime sampling_interval{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };

        const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
        const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            Animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

            const size_t node_count{ scene_view.nodes.size() };
            keyframe.nodes.resize(node_count);
            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
                if (fbx_node)
                {
                    Animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
                    // シーンのグローバル座標系に関するノードの変換行列
                    node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

                    // 親のローカル座標系に関するノードの変換行列
                    const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
                    node.scaling = to_xmfloat3(local_transform.GetS());
                    node.rotation = to_xmfloat4(local_transform.GetQ());
                    node.translation = to_xmfloat3(local_transform.GetT());
                }
            }
        }
    }
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        delete animation_stack_names[animation_stack_index];
    }
}

void SkinnedMesh::DebugThreshold(float n)
{
    dissolve = n;
    //ImGui::SliderFloat("threshold", &this->dissolve, 0.0f, 1.0f);
}


bool SkinnedMesh::AppendAnimations(const char* animation_filename, float sampling_rate)
{
    // 別ファイルからアニメーションの抽出

    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(animation_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FetchAnimations(fbx_scene, animation_clips, sampling_rate/*0:use default value, less than 0:do not fetch*/);

    fbx_manager->Destroy();

    return true;
}

void SkinnedMesh::BlendAnimations(const Animation::keyframe* keyframes[2], float factor, Animation::keyframe& keyframe)
{
    _ASSERT_EXPR(keyframes[0]->nodes.size() == keyframes[1]->nodes.size(), "The size of the two node arrays must be the same.");

    size_t node_count{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling), XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
        XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{ XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation), XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
        XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation), XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
        XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
    }
}

// RAYCAST
#include "./Lemur/Ray/RayCast.h"
// The coordinate system of all function arguments is world space.
bool SkinnedMesh::RayCast(const XMFLOAT4& position, const XMFLOAT4& direction, const XMFLOAT4X4& world_transform, XMFLOAT4& closest_point, DirectX::XMFLOAT3& intersected_normal,
    std::string& intersected_mesh, std::string& intersected_material)
{
    float closest_distance{ FLT_MAX };

    for (const mesh& mesh : meshes)
    {
        XMFLOAT4 ray_position = position;
        XMFLOAT4 ray_direction = direction;
        ray_direction.w = 0;

        // Convert to model space.
        XMMATRIX concatenated_matrix{
            XMLoadFloat4x4(&mesh.default_global_transform) *
            XMLoadFloat4x4(&world_transform) };
        XMMATRIX inverse_concatenated_matrix{ XMMatrixInverse(nullptr, concatenated_matrix) };
        XMStoreFloat4(&ray_position, XMVector3TransformCoord(XMLoadFloat4(&ray_position), inverse_concatenated_matrix));
        XMStoreFloat4(&ray_direction, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&ray_direction), inverse_concatenated_matrix)));

#if 0
        const float* min{ reinterpret_cast<const float*>(&mesh.bounding_box[0]) };
        const float* max{ reinterpret_cast<const float*>(&mesh.bounding_box[1]) };
        if (!intersect_ray_aabb(reinterpret_cast<const float*>(&ray_position), reinterpret_cast<const float*>(&ray_direction), min, max))
        {
            continue;
        }
#endif

        float distance{ 1.0e+7f };
        XMFLOAT4 intersection{};
        const float* vertex_positions{ reinterpret_cast<const float*>(mesh.vertices.data()) };
        const uint32_t* indices{ mesh.indices.data() };
        const size_t index_count{ mesh.indices.size() };

        const int intersected_triangle_index{ intersect_ray_triangles(vertex_positions, 0L, sizeof(vertex), indices, index_count, ray_position, ray_direction, intersection, distance) };
        if (intersected_triangle_index >= 0)
        {
            if (closest_distance > distance)
            {
                closest_distance = distance;
                // Convert model space to original space.
                XMStoreFloat4(&closest_point, XMVector3TransformCoord(XMLoadFloat4(&intersection), concatenated_matrix));
                intersected_mesh = mesh.name;
                intersected_material = mesh.FindSubset(intersected_triangle_index * 3)->material_name;
                intersected_normal = mesh.vertices.at(indices[intersected_triangle_index + 0]).normal;
            }
        }
    }
    return closest_distance < FLT_MAX;
}

// STATIC_BATCHING
StaticMesh::StaticMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials);
    }
    else
    {
        FetchScene(fbx_filename, triangulate);

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scene_view, meshes, materials);
    }
    CreateComObjects(device, fbx_filename);
}
void StaticMesh::FetchScene(const char* fbx_filename, bool triangulate)
{
    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    // Serialize an entire scene graph into sequence container
    std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
#if 0
        if (fbx_node->GetNodeAttribute())
        {
            switch (fbx_node->GetNodeAttribute()->GetAttributeType())
            {
            case FbxNodeAttribute::EType::eNull:
            case FbxNodeAttribute::EType::eMesh:
            case FbxNodeAttribute::EType::eSkeleton:
            case FbxNodeAttribute::EType::eUnknown:
            case FbxNodeAttribute::EType::eMarker:
            case FbxNodeAttribute::EType::eCamera:
            case FbxNodeAttribute::EType::eLight:
                scene::node& node{ scene_view.nodes.emplace_back() };
                node.attribute = fbx_node->GetNodeAttribute()->GetAttributeType();
                node.name = fbx_node->GetName();
                node.unique_id = fbx_node->GetUniqueID();
                node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
                break;
            }
        }
#else
        Scene::node& node{ scene_view.nodes.emplace_back() };
        node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
        node.name = fbx_node->GetName();
        node.unique_id = fbx_node->GetUniqueID();
        node.parent_index = scene_view.Indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
#endif
        for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
        {
            traverse(fbx_node->GetChild(child_index));
        }
    } };
    traverse(fbx_scene->GetRootNode());

    FetchMeshes(fbx_scene, meshes);
    FetchMaterials(fbx_scene, materials);

    fbx_manager->Destroy();
}

void StaticMesh::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    // Fetch all meshes from the scene.
    for (const Scene::node& node : scene_view.nodes)
    {
        // Skip if node attribute is not eMesh.
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        mesh& mesh{ meshes.emplace_back() };
#if 0
        mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        mesh.name = fbx_mesh->GetNode()->GetName();
        mesh.node_index = scene_view.indexof(mesh.unique_id);
        mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
#else
        mesh.unique_id = fbx_node->GetUniqueID();
        mesh.name = fbx_node->GetName();
        mesh.node_index = scene_view.Indexof(mesh.unique_id);
        mesh.default_global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform());
#endif

        // Build subsets for each material
        std::vector<mesh::subset>& subsets{ mesh.subsets };
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        subsets.resize(material_count > 0 ? material_count : 1);
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
            subsets.at(material_index).material_name = fbx_material->GetName();
            subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
        }
        if (material_count > 0)
        {
            // Count the faces of each material
            const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
            {
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                subsets.at(material_index).index_count += 3;
            }

            // Record the offset (How many vertex)
            uint32_t offset{ 0 };
            for (mesh::subset& subset : subsets)
            {
                subset.start_index_location = offset;
                offset += subset.index_count;
                // This will be used as counter in the following procedures, reset to zero
                subset.index_count = 0;
            }
        }

        const int polygon_count{ fbx_mesh->GetPolygonCount() };
        mesh.vertices.resize(polygon_count * 3LL);
        mesh.indices.resize(polygon_count * 3LL);

        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };

            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
                vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                if (fbx_mesh->GenerateNormals(false))
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon, uv_names[0], uv, unmapped_uv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }

                mesh.vertices.at(vertex_index) = std::move(vertex);
#if 0
                mesh.indices.at(vertex_index) = vertex_index;
#else
                mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
                subset.index_count++;
#endif
            }
        }
        for (const vertex& v : mesh.vertices)
        {
            mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
            mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
            mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
            mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
            mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
            mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
        }
    }
}
void StaticMesh::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    for (mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }

    for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
        // EMISSIVE
        D3D11_TEXTURE2D_DESC texture2d_desc;
        // Diffuse
        if (iterator->second.texture_filenames[0].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[0]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 4);
        }
        // Normal
        if (iterator->second.texture_filenames[1].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[1]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 4);
        }
        // Emissive
        if (iterator->second.texture_filenames[2].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[2]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[2].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[2].GetAddressOf(), 0xFF000000, 4);
        }
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
size_t StaticMesh::Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color)
{
    size_t drawcall_count = 0;
    for (mesh& mesh : meshes)
    {
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        constants data;
        XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
        for (const mesh::subset& subset : mesh.subsets)
        {
            const Material& material{ materials.at(subset.material_unique_id) };

            XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

            immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
            immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());
            immediate_context->PSSetShaderResources(2, 1, material.shader_resource_views[2].GetAddressOf());
            immediate_context->PSSetShaderResources(3, 1, material.metalness_smoothness.GetAddressOf());
            immediate_context->PSSetShaderResources(4, 1, material.metalness.GetAddressOf());
            immediate_context->PSSetShaderResources(5, 1, material.roughness.GetAddressOf());

            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
            drawcall_count++;
        }
    }
    return drawcall_count;
}
void StaticMesh::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        const Scene::node& node{ scene_view.nodes.at(node_index) };
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            Material material;
            material.name = fbx_material->GetName();
            material.unique_id = fbx_material->GetUniqueID();
            FbxProperty fbx_property;
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[1] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[2] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }

            materials.emplace(material.unique_id, std::move(material));
        }
    }
#if 1
    // Append default(dummy) material
    materials.emplace();
#endif
}

// STATIC_BATCHING
StaticMeshBatch::StaticMeshBatch(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, materials, vertices/*STATIC_BATCHING*/);
    }
    else
    {
        FetchScene(fbx_filename, triangulate);

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scene_view, materials, vertices/*STATIC_BATCHING*/);
    }
    CreateComObjects(device, fbx_filename);
}
void StaticMeshBatch::FetchScene(const char* fbx_filename, bool triangulate)
{
    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    // Serialize an entire scene graph into sequence container
    std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
        Scene::node& node{ scene_view.nodes.emplace_back() };
        node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
        node.name = fbx_node->GetName();
        node.unique_id = fbx_node->GetUniqueID();
        node.parent_index = scene_view.Indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
        for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
        {
            traverse(fbx_node->GetChild(child_index));
        }
    } };
    traverse(fbx_scene->GetRootNode());

    FetchMeshes(fbx_scene);
    FetchMaterials(fbx_scene, materials);

    fbx_manager->Destroy();
}

void StaticMeshBatch::FetchMeshes(FbxScene* fbx_scene)
{
    // Fetch all meshes from the scene.
    for (const Scene::node& node : scene_view.nodes)
    {
        // Skip if node attribute is not eMesh.
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };

        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };

        const int polygon_count{ fbx_mesh->GetPolygonCount() };
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
                vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                if (fbx_mesh->GenerateNormals(false))
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon, uv_names[0], uv, unmapped_uv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }

                DirectX::XMFLOAT4X4 default_global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform());
                DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&default_global_transform);

                DirectX::XMStoreFloat3(&vertex.position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&vertex.position), M));

                DirectX::XMStoreFloat3(&vertex.normal, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&vertex.normal), M));

                const float sigma = vertex.tangent.w;
                DirectX::XMStoreFloat4(&vertex.tangent, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat4(&vertex.tangent), M));
                vertex.tangent.w = sigma;

                const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
                vertices[fbx_material->GetUniqueID()].emplace_back(vertex);
            }
        }
    }
}
void StaticMeshBatch::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    for (decltype(materials)::const_reference material : materials)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertices.at(material.first).size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = vertices.at(material.first).data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffers[material.first].ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    vertices.clear();

    for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
        // EMISSIVE
        D3D11_TEXTURE2D_DESC texture2d_desc;
        // Diffuse
        if (iterator->second.texture_filenames[0].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[0]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 4);
        }
        // Normal
        if (iterator->second.texture_filenames[1].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[1]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 4);
        }
        // Emissive
        if (iterator->second.texture_filenames[2].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[2]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[2].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shader_resource_views[2].GetAddressOf(), 0xFF000000, 4);
        }
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "./Shader/static_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "./Shader/static_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
size_t StaticMeshBatch::Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color)
{
    size_t drawcall_count = 0;
    for (decltype(materials)::const_reference material : materials)
    {
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, vertex_buffers.at(material.first).GetAddressOf(), &stride, &offset);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        constants data;
        XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&world));

        XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.second.Kd));
        immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
        immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

        immediate_context->PSSetShaderResources(0, 1, material.second.shader_resource_views[0].GetAddressOf());
        immediate_context->PSSetShaderResources(1, 1, material.second.shader_resource_views[1].GetAddressOf());
        immediate_context->PSSetShaderResources(2, 1, material.second.shader_resource_views[2].GetAddressOf());

        D3D11_BUFFER_DESC buffer_desc{};
        vertex_buffers.at(material.first)->GetDesc(&buffer_desc);
        immediate_context->Draw(buffer_desc.ByteWidth / sizeof(vertex), 0);
        drawcall_count++;
    }
    return drawcall_count;
}
void StaticMeshBatch::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        const Scene::node& node{ scene_view.nodes.at(node_index) };
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            Material material;
            material.name = fbx_material->GetName();
            material.unique_id = fbx_material->GetUniqueID();
            FbxProperty fbx_property;
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[1] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material.texture_filenames[2] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }

            materials.emplace(material.unique_id, std::move(material));
        }
    }
}