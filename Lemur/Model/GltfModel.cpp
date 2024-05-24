#include "GltfModel.h"
#define TINYGLTF_IMPLEMENTATION
#include "../External/tinygltf-release/tiny_gltf.h"
#include "../Source/misc.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Graphics/Texture.h"
#include <stack>

#include <filesystem>
// �e�N�X�`���O���ǂݍ���
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
    // �摜�����[�h���邽�߂̃R�[���o�b�N��ݒ�
    tiny_gltf.SetImageLoader(null_load_image_data, nullptr);

    tinygltf::Model gltf_model;
    std::string error, warning;
    bool succeeded{ false };
    if (filename.find(".glb") != std::string::npos)
    {
        // �t�@�C�������[�h(glb�̂Ƃ�)
        succeeded = tiny_gltf.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
    }
    else if (filename.find(".gltf") != std::string::npos)
    {
        // �t�@�C�������[�h(gltf�̂Ƃ�)
        succeeded = tiny_gltf.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());      // �x��������Ε\��
    _ASSERT_EXPR_A(error.empty(), error.c_str());          // �G���[������Ε\��
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");// �t�@�C���ǂݍ��݂Ɏ��s�����Ƃ�

    // ���f������V�[������1�Â��o���i�V�[���̐��������[�v���񂷁j
    for (std::vector<tinygltf::Scene>::const_reference gltf_scene : gltf_model.scenes)
    {
        scene& scene{ scenes.emplace_back() };      // scenes�ɐV�������scene��ǉ����Ă��̎Q�Ƃ��i�[
        scene.name = gltf_model.scenes.at(0).name;  // ���ׂĂ�scene�I�u�W�F�N�g�ɓ������O��ݒ�
        scene.nodes = gltf_model.scenes.at(0).nodes;// ���ׂĂ� scene �I�u�W�F�N�g�ɓ����m�[�h����ݒ�
    }

    // �m�[�h�擾
    FetchNodes(gltf_model);
    // ���b�V���擾
    FetchMeshes(device, gltf_model);
    // �}�e���A���擾
    FetchMaterials(device, gltf_model);
    // �e�N�X�`���擾
    if(!external_texture)FetchTextures(device, gltf_model);
    // �O������̃e�N�X�`���ǂݍ���
    else FetchExternalTextures(device, gltf_model, filename);
    // �A�j���[�V�����擾
    FetchAnimations(gltf_model);
    // �}�e���A����GPU�ɓn��
    MaterialForGPU(device);

    // TODO ����͋����ȃv���O���~���O�ł���A�o�O�������N�����\��������B
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


    // �萔�o�b�t�@�[
    {
        HRESULT hr;
        D3D11_BUFFER_DESC buffer_desc{};

        buffer_desc.ByteWidth = sizeof(PrimitiveConstants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_cbuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // �{�[��
        buffer_desc.ByteWidth = sizeof(primitive_joint_constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, NULL, primitive_joint_cbuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

}

void GltfModel::FetchNodes(const tinygltf::Model& gltf_model)
{
    // ���f������m�[�h����1�Â��o���i�m�[�h�̐��������[�v���񂷁j
    for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
    {
        node& node{ nodes.emplace_back() };      // nodes�ɐV�������node��ǉ����Ă��̎Q�Ƃ��i�[
        // �e�������o���ēn��
        node.name = gltf_node.name;
        node.skin = gltf_node.skin;
        node.mesh = gltf_node.mesh;
        node.children = gltf_node.children;

        // �m�[�h����s�����ǂݎ���āAXMFLOAT4X4�s��ɕϊ�
        if (!gltf_node.matrix.empty())
        {
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; row++)// �s�irow�j��0����3�܂ŏ��Ԃɏ���
            {
                for (size_t column = 0; column < 4; column++)// �F��icolumn�j��0����3�܂ŏ��Ԃɏ���
                {
                    // GLTF�̍s���񂩂�A�w�肳�ꂽ�s�Ɨ�̗v�f���擾���ADirectX��matrix�̑Ή�����ʒu�ɐݒ�
                    // GLTF�̍s���1�����̔z��Ƃ��Ċi�[����Ă��邽�߁A�w�肳�ꂽ�s�Ɨ�̈ʒu���v�Z���ėv�f���擾
                    matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
                }
            }
            DirectX::XMVECTOR S, T, R;
            // �s�񂩂�scale,rotation,translation�����o��
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

            // Float3�ɕϊ�
            DirectX::XMStoreFloat3(&node.scale, S);
            DirectX::XMStoreFloat4(&node.rotation, R);
            DirectX::XMStoreFloat3(&node.translation, T);
        }
        else
        {
            // �s���񂪂Ȃ��ꍇ�́Ascale, translation, rotation�𒼐ڐݒ�
            // (gltf_node.scale, gltf_node.translation, gltf_node.rotation����l���擾���Đݒ�)
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

    // �e�m�[�h�̃O���[�o���ϊ��s��itransform�j��ێ�����X�^�b�N
    // �X�^�b�N�́A�Ō�ɒǉ����ꂽ�v�f�i�e�m�[�h�̕ϊ��s��j���ŏ��Ɏ��o�����f�[�^�\��
    std::stack<XMFLOAT4X4> parent_global_transforms;
    std::function<void(int)> traverse{ [&](int node_index)->void
    {
        node& node{nodes.at(node_index)};

        // �m�[�h�̃X�P�[���A��]�A���s�ړ����s��ɕϊ�
        XMMATRIX S{ XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
        XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

        // �m�[�h�̃��[�J���ϊ��s����v�Z���A�e�̃O���[�o���ϊ��s��Ɛς����
        XMStoreFloat4x4(&node.global_transform, S * R * T * XMLoadFloat4x4(&parent_global_transforms.top()));

        // �q�m�[�h���ċA�I�ɏ�������
        for (int child_index : node.children)
        {
            // �e�̃O���[�o���ϊ��s����X�^�b�N�ɒǉ����Ďq�m�[�h������
            parent_global_transforms.push(node.global_transform);
            traverse(child_index);
            parent_global_transforms.pop(); // �q�m�[�h�̏������I�������e�̍s����X�^�b�N������o��
        }
    } };

    // �m�[�h�K�w�\�����g���o�[�X���āA�e�m�[�h�̃O���[�o���ϊ��s����v�Z
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        parent_global_transforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
        traverse(node_index);
        parent_global_transforms.pop();
    }
}


GltfModel::buffer_view GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
    // �f�[�^�̃t�H�[�}�b�g��i�[���@
    buffer_view buffer_view;

    // �A�N�Z�T�̃^�C�v�i�f�[�^�̌`���j�ɉ����ď����𕪊�
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:// �A�N�Z�T�̃f�[�^�^���X�J���[

        // �A�N�Z�T�̃f�[�^���ǂ̂悤�ȃf�[�^�^�i���l�^�j�ł��邩�ɉ����ď����𕪊�
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:// �����Ȃ�16�r�b�g�����^

            buffer_view.format = DXGI_FORMAT_R16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT);

            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:// �����Ȃ�32�r�b�g�����^

            buffer_view.format = DXGI_FORMAT_R32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT);

            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC2:// �A�N�Z�T�̃f�[�^�^��2�����x�N�g��

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// ���������_���^�̃f�[�^
            buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC3:// �A�N�Z�T�̃f�[�^�^��3�����x�N�g��

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// ���������_���^�̃f�[�^
            buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }

        break;
    case TINYGLTF_TYPE_VEC4:// �A�N�Z�T�̃f�[�^�^��4�����x�N�g��

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:// �����Ȃ�16�r�b�g�����^
            buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:// �����Ȃ�32�r�b�g�����^
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:// ���������_���^�̃f�[�^
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

    // �o�b�t�@�r���[�S�̂̃T�C�Y���v�Z
    buffer_view.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view.stride_in_bytes);
    return buffer_view;
}

void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr;

    // ���ׂẴ��b�V���ɂ��ă��[�v
    for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
    {
        mesh& mesh{ meshes.emplace_back() };// �V�������b�V���� meshes �x�N�^�[�ɒǉ����A���̎Q�Ƃ��擾���܂��B

        mesh.name = gltf_mesh.name;

        // �e���b�V���̃v���~�e�B�u�i���i�j�ɂ��ă��[�v
        for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
        {
            mesh::primitive& primitive{ mesh.primitives.emplace_back() };// ���b�V�����̃v���~�e�B�u��V�����v���~�e�B�u�Ƃ��Ēǉ����A���̎Q�Ƃ��擾
            primitive.material = gltf_primitive.material;

            // �C���f�b�N�X�o�b�t�@�̍쐬
            // �v���~�e�B�u�i���i�j�̃C���f�b�N�X���������A�N�Z�T�iAccessor�j���擾���܂��B
            // ���̃A�N�Z�T�ɂ́A�C���f�b�N�X�o�b�t�@�̊i�[����Ă���o�b�t�@�r���[�iBufferView�j�̃C���f�b�N�X���܂܂��
            const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_primitive.indices) };
            // �C���f�b�N�X�̃o�b�t�@�r���[���擾
            // �o�b�t�@�r���[�́A�o�b�t�@�̔z�u��t�H�[�}�b�g����\��
            const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // glTF �A�N�Z�T����C���f�b�N�X�̃o�b�t�@�r���[���쐬���āA�v���~�e�B�u��index_buffer_view�ɐݒ�
            // ����ɂ��A�o�b�t�@�̃t�H�[�}�b�g��T�C�Y���ݒ肳���
            primitive.index_buffer_view = MakeBufferView(gltf_accessor);

            // �o�b�t�@���쐬���邽�߂̐ݒ�
            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            // �o�b�t�@�Ɋi�[����f�[�^��ݒ�
            D3D11_SUBRESOURCE_DATA subresource_data{};
            subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
            // ���ۂ� DirectX 11 �̃o�b�t�@���쐬
            // ����ɂ��AGPU �Ŏg�p�\�ȃC���f�b�N�X�o�b�t�@���쐬�����
            hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());

            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


            // ���_�o�b�t�@�[���쐬
            for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
            {
                // �����ɑΉ�����A�N�Z�T�iAccessor�j���擾
                const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_attribute.second) };
                // �A�N�Z�T����o�b�t�@�r���[�iBufferView�j���擾
                const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

                // �o�b�t�@�r���[���璸�_�o�b�t�@�r���[���쐬
                buffer_view vertex_buffer_view{ MakeBufferView(gltf_accessor) };

                // DirectX 11 �p�̃o�b�t�@���쐬���邽�߂̐ݒ�
                D3D11_BUFFER_DESC buffer_desc{};
                buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
                buffer_desc.Usage = D3D11_USAGE_DEFAULT;
                buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

                // �o�b�t�@�ɃR�s�[����f�[�^��ݒ�
                D3D11_SUBRESOURCE_DATA subresource_data{};
                subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                    + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;

                // DirectX 11 �̃o�b�t�@���쐬
                hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                    vertex_buffer_view.buffer.ReleaseAndGetAddressOf());

                // �G���[�`�F�b�N
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                // �v���~�e�B�u�ɒ��_�o�b�t�@�r���[��ǉ�
                primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));
            }

            // �_�~�[�������Ȃ��ꍇ�͒ǉ�����
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
    // �e�}�e���A���ɑ΂��ď������s��
    for (std::vector<tinygltf::Material>::const_reference gltf_material : gltf_model.materials)
    {
        // material�\���̂� materials�ɒǉ����A���̎Q�Ƃ��擾
        std::vector<material>::reference material = materials.emplace_back();

        // �}�e���A���̖��O���ݒ�
        material.name = gltf_material.name;

        // �e���񂪁A material.data �̑Ή����郁���o�[�ɐݒ�
        material.data.emissive_factor[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
        material.data.emissive_factor[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
        material.data.emissive_factor[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

        material.data.alpha_mode = gltf_material.alphaMode == "OPAQUE" ? 0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
        material.data.alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
        material.data.double_sided = gltf_material.doubleSided ? 1 : 0;

        // �e�e�N�X�`�������ݒ�
        // ���ꂼ��̃e�N�X�`�����́A�\���̂�ʂ��Ċi�[
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

    // �e�N�X�`�������[�v
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
            // gltf�o�b�t�@����e�N�X�`���f�[�^�ǂݍ���
            const tinygltf::BufferView& buffer_view{ gltf_model.bufferViews.at(gltf_image.bufferView) };
            const tinygltf::Buffer& buffer{ gltf_model.buffers.at(buffer_view.buffer) };
            const byte* data = buffer.data.data() + buffer_view.byteOffset;

            ID3D11ShaderResourceView* texture_resource_view{};
            hr = LoadTextureFromMemory(device, data, buffer_view.byteLength, &texture_resource_view);
            if (hr == S_OK)
            {
                // �ǂݍ��񂾃e�N�X�`�����V�F�[�_�[���\�[�X�r���[�Ƃ��Ēǉ�
                texture_resource_views.emplace_back().Attach(texture_resource_view);
            }
        }
        else
        {
            // �o�b�t�@�r���[�����݂��Ȃ��ꍇ�́A�t�@�C������e�N�X�`���f�[�^��ǂݍ���
            const std::filesystem::path path(filename);
            ID3D11ShaderResourceView* shader_resource_view{};
            D3D11_TEXTURE2D_DESC texture2d_desc;
            std::wstring filename{
            path.parent_path().concat(L"/").wstring() +
             std::wstring(gltf_image.uri.begin(), gltf_image.uri.end()) };
            hr = LoadTextureFromFile(device, filename.c_str(), &shader_resource_view, &texture2d_desc);
            if (hr == S_OK)
            {
                // �ǂݍ��񂾃e�N�X�`�����V�F�[�_�[���\�[�X�r���[�Ƃ��Ēǉ�
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
        // �V���� skin �I�u�W�F�N�g���쐬���Askins �x�N�^�[�ɒǉ�����
        skin& skin{ skins.emplace_back() };

        // �X�L���Ɋ֘A�t����ꂽ�t�o�C���h�s�� (inverseBindMatrices) �� Accessor ���擾����
        const Accessor& gltf_accessor{ gltf_model.accessors.at(transmission_skin.inverseBindMatrices) };

        // �Ή����� BufferView ���擾����
        const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

        // �t�o�C���h�s��̃T�C�Y��ݒ肵�āA�K�v�ȃ��������m�ۂ���
        skin.inverse_bind_matrices.resize(gltf_accessor.count);

        // �R�s�[���f�[�^����t�o�C���h�s��f�[�^���R�s�[����
        memcpy(skin.inverse_bind_matrices.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4X4));

        // �X�L���Ɋ֘A�t����ꂽ�W���C���g�̃��X�g��ݒ肷��
        skin.joints = transmission_skin.joints;
    }

    for (vector<Animation>::const_reference gltf_animation : gltf_model.animations)
    {
        // �A�j���[�V������\���\���̂��쐬���āAanimations�x�N�^�[�ɒǉ�
        animation& animation{ animations.emplace_back() };
        animation.name = gltf_animation.name; // �A�j���[�V�����̖��O��ݒ�

        // �A�j���[�V�����̃T���v���[���Ƃɏ���
        for (vector<AnimationSampler>::const_reference gltf_sampler : gltf_animation.samplers)
        {
            animation::sampler& sampler{ animation.samplers.emplace_back() };
            sampler.input = gltf_sampler.input;// �T���v���[�̓��́i���Ԏ��j�̃C���f�b�N�X��ݒ�
            sampler.output = gltf_sampler.output;// �T���v���[�̏o�́i�l�j�̃C���f�b�N�X��ݒ�
            sampler.interpolation = gltf_sampler.interpolation; // ��ԕ��@��ݒ�

            // ���͂ɑΉ�����A�N�Z�T�����擾
            const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.input) };
            const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // �A�j���[�V�����̃^�C�����C����ǉ��i�܂��ǉ�����Ă��Ȃ��ꍇ�j
            // �^�C�����C���͓��́i���Ԏ��j�ɑΉ����A���̃^�C�����C���ɂ͒l�̔z�񂪕ۑ������
            pair<unordered_map<int, vector<float>>::iterator, bool>& timelines{
            animation.timelines.emplace(gltf_sampler.input, gltf_accessor.count) };
            if (timelines.second)
            {
                // �^�C�����C���̃f�[�^���o�b�t�@����R�s�[
                memcpy(timelines.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                    gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(FLOAT));
            }
        }

        // �A�j���[�V�����̃`�����l�����Ƃɏ���
        for (vector<AnimationChannel>::const_reference gltf_channel : gltf_animation.channels)
        {
            // �A�j���[�V�����̃`�����l����\���\���̂��쐬���āAchannels�x�N�^�[�ɒǉ�
            animation::channel& channel{ animation.channels.emplace_back() };

            channel.sampler = gltf_channel.sampler; // �`�����l�����g�p����T���v���[�̃C���f�b�N�X��ݒ�
            channel.target_node = gltf_channel.target_node;// �`�����l���̃^�[�Q�b�g�m�[�h�̃C���f�b�N�X��ݒ�
            channel.target_path = gltf_channel.target_path; // �`�����l���̃^�[�Q�b�g�p�X�iscale, rotation, translation�j��ݒ�

            // �`�����l�����w�肷��T���v���[�ɑΉ���������擾
            const AnimationSampler& gltf_sampler{ gltf_animation.samplers.at(gltf_channel.sampler) };
            const Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.output) };
            const BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            // �^�[�Q�b�g�p�X���Ƃɏ����𕪊�
            if (gltf_channel.target_path == "scale")
            {
                // �X�P�[������ǉ��i�܂��ǉ�����Ă��Ȃ��ꍇ�j
                pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& scales{
                animation.scales.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (scales.second)
                {
                    // �X�P�[���̃f�[�^���o�b�t�@����R�s�[
                    memcpy(scales.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
                }
            }
            else if (gltf_channel.target_path == "rotation")
            {
                // ��]����ǉ��i�܂��ǉ�����Ă��Ȃ��ꍇ�j
                pair<unordered_map<int, vector<XMFLOAT4>>::iterator, bool>& rotations{
                animation.rotations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (rotations.second)
                {
                    // ��]�̃f�[�^���o�b�t�@����R�s�[
                    memcpy(rotations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4));
                }
            }
            else if (gltf_channel.target_path == "translation")
            {
                // �ړ�����ǉ��i�܂��ǉ�����Ă��Ȃ��ꍇ�j
                pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& translations{
                animation.translations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (translations.second)
                {
                    // �ړ��̃f�[�^���o�b�t�@����R�s�[
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

    // �V�F�[�_�[����̓��C�A�E�g�̃Z�b�g
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());

    std::function<void(int)> traverse{ [&](int node_index)->void {

        //�m�[�h���擾
       const node& node{nodes.at(node_index)};

       // �m�[�h���X�L���������Ă��邩
       if (node.skin > -1)
       {
           // �m�[�h���Q�Ƃ���X�L�����擾
           const skin& skin{ skins.at(node.skin) };

           // �W���C���g�s����v�Z���邽�߂̒萔�o�b�t�@�[��������
           primitive_joint_constants primitive_joint_data{};

           // �X�L���Ɋ֘A����e�W���C���g�ɂ��ď���
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // �W���C���g�s��̌v�Z
               // 1. �t�o�C���h�s����擾����XMMatrix�ɕϊ�
               // 2. �W���C���g�̃O���[�o���ϊ��s����擾����XMMatrix�ɕϊ�
               // 3. �m�[�h�̃O���[�o���ϊ��s����t�s�񉻂���XMMatrix�ɕϊ�
               // 4. �W���C���g�s��̌v�Z�F�t�o�C���h�s�� * �W���C���g�̃O���[�o���ϊ��s�� * �m�[�h�̃O���[�o���ϊ��s��̋t�s��
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // �萔�o�b�t�@�[���X�V�F�W���C���g�s��f�[�^���V�F�[�_�[�̒萔�o�b�t�@�[�ɃR�s�[
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // ���_�V�F�[�_�[�ɒ萔�o�b�t�@�[��ݒ�
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // �m�[�h�����b�V���������Ă��邩
       if (node.mesh > -1)
       {
           // �m�[�h���Q�Ƃ��郁�b�V�����擾
           const mesh& mesh{ meshes.at(node.mesh) };

           // ���b�V�����̊e�v���~�e�B�u�ɑ΂��ă��[�v
           // �e�v���~�e�B�u�́A���_�o�b�t�@��C���f�b�N�X�o�b�t�@�A�}�e���A���Ȃǂ̏��������Ă���
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // �e�v���~�e�B�u�̒��_�o�b�t�@��z��Ƃ��Ď擾
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // ���_�o�b�t�@�̃T�C�Y���擾
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // ���_�o�b�t�@����͂Ƃ��Đݒ�
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // �v���~�e�B�u�̃C���f�b�N�X�o�b�t�@��ݒ�
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // �v���~�e�B�u�̃}�e���A����ϊ��s��Ȃǂ̏���ݒ�
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));


               // �萔�o�b�t�@�� primitive_data �̓��e���R�s�[
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // �}�e���A�����̊e�e�N�X�`���Ɋ֘A�t����ꂽ�C���f�b�N�X��z��Ƃ��Ď擾
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

               // �e�N�X�`���̃C���f�b�N�X�����[�v���A�e�e�N�X�`���ɑΉ�����V�F�[�_�[���\�[�X�r���[��ݒ�
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // �s�N�Z���V�F�[�_�[�ɃV�F�[�_�[���\�[�X�r���[��ݒ�
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // �v���~�e�B�u��`��
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // �e�m�[�h�������b�V����`��
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // �e�m�[�h�������b�V����`��
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader* replaced_pixel_shader)
{
    using namespace DirectX;

    // �V�F�[�_�[����̓��C�A�E�g�̃Z�b�g
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

        //�m�[�h���擾
       const node& node{nodes.at(node_index)};

       // �m�[�h���X�L���������Ă��邩
       if (node.skin > -1)
       {
           // �m�[�h���Q�Ƃ���X�L�����擾
           const skin& skin{ skins.at(node.skin) };

           // �W���C���g�s����v�Z���邽�߂̒萔�o�b�t�@�[��������
           primitive_joint_constants primitive_joint_data{};

           // �X�L���Ɋ֘A����e�W���C���g�ɂ��ď���
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // �W���C���g�s��̌v�Z
               // 1. �t�o�C���h�s����擾����XMMatrix�ɕϊ�
               // 2. �W���C���g�̃O���[�o���ϊ��s����擾����XMMatrix�ɕϊ�
               // 3. �m�[�h�̃O���[�o���ϊ��s����t�s�񉻂���XMMatrix�ɕϊ�
               // 4. �W���C���g�s��̌v�Z�F�t�o�C���h�s�� * �W���C���g�̃O���[�o���ϊ��s�� * �m�[�h�̃O���[�o���ϊ��s��̋t�s��
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // �萔�o�b�t�@�[���X�V�F�W���C���g�s��f�[�^���V�F�[�_�[�̒萔�o�b�t�@�[�ɃR�s�[
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // ���_�V�F�[�_�[�ɒ萔�o�b�t�@�[��ݒ�
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // �m�[�h�����b�V���������Ă��邩
       if (node.mesh > -1)
       {
           // �m�[�h���Q�Ƃ��郁�b�V�����擾
           const mesh& mesh{ meshes.at(node.mesh) };

           // ���b�V�����̊e�v���~�e�B�u�ɑ΂��ă��[�v
           // �e�v���~�e�B�u�́A���_�o�b�t�@��C���f�b�N�X�o�b�t�@�A�}�e���A���Ȃǂ̏��������Ă���
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // �e�v���~�e�B�u�̒��_�o�b�t�@��z��Ƃ��Ď擾
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // ���_�o�b�t�@�̃T�C�Y���擾
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // ���_�o�b�t�@����͂Ƃ��Đݒ�
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // �v���~�e�B�u�̃C���f�b�N�X�o�b�t�@��ݒ�
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // �v���~�e�B�u�̃}�e���A����ϊ��s��Ȃǂ̏���ݒ�
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));

               // �萔�o�b�t�@�� primitive_data �̓��e���R�s�[
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // �}�e���A�����̊e�e�N�X�`���Ɋ֘A�t����ꂽ�C���f�b�N�X��z��Ƃ��Ď擾
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

               // �e�N�X�`���̃C���f�b�N�X�����[�v���A�e�e�N�X�`���ɑΉ�����V�F�[�_�[���\�[�X�r���[��ݒ�
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // �s�N�Z���V�F�[�_�[�ɃV�F�[�_�[���\�[�X�r���[��ݒ�
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // �v���~�e�B�u��`��
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // �e�m�[�h�������b�V����`��
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // �e�m�[�h�������b�V����`��
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader** replaced_pixel_shader)
{
    using namespace DirectX;

    // �V�F�[�_�[����̓��C�A�E�g�̃Z�b�g
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

        //�m�[�h���擾
       const node& node{nodes.at(node_index)};

       // �m�[�h���X�L���������Ă��邩
       if (node.skin > -1)
       {
           // �m�[�h���Q�Ƃ���X�L�����擾
           const skin& skin{ skins.at(node.skin) };

           // �W���C���g�s����v�Z���邽�߂̒萔�o�b�t�@�[��������
           primitive_joint_constants primitive_joint_data{};

           // �X�L���Ɋ֘A����e�W���C���g�ɂ��ď���
           for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
           {
               // �W���C���g�s��̌v�Z
               // 1. �t�o�C���h�s����擾����XMMatrix�ɕϊ�
               // 2. �W���C���g�̃O���[�o���ϊ��s����擾����XMMatrix�ɕϊ�
               // 3. �m�[�h�̃O���[�o���ϊ��s����t�s�񉻂���XMMatrix�ɕϊ�
               // 4. �W���C���g�s��̌v�Z�F�t�o�C���h�s�� * �W���C���g�̃O���[�o���ϊ��s�� * �m�[�h�̃O���[�o���ϊ��s��̋t�s��
               XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                   XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                   XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
                   XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
               );
           }
           // �萔�o�b�t�@�[���X�V�F�W���C���g�s��f�[�^���V�F�[�_�[�̒萔�o�b�t�@�[�ɃR�s�[
           immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);

           // ���_�V�F�[�_�[�ɒ萔�o�b�t�@�[��ݒ�
           immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
       }

       // �m�[�h�����b�V���������Ă��邩
       if (node.mesh > -1)
       {
           // �m�[�h���Q�Ƃ��郁�b�V�����擾
           const mesh& mesh{ meshes.at(node.mesh) };

           // ���b�V�����̊e�v���~�e�B�u�ɑ΂��ă��[�v
           // �e�v���~�e�B�u�́A���_�o�b�t�@��C���f�b�N�X�o�b�t�@�A�}�e���A���Ȃǂ̏��������Ă���
           for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
           {
               // �e�v���~�e�B�u�̒��_�o�b�t�@��z��Ƃ��Ď擾
               ID3D11Buffer* vertex_buffers[]{
                 primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                 primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                 primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                 primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                 primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
               };

               // ���_�o�b�t�@�̃T�C�Y���擾
               UINT strides[]{
                 static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                 static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
               };

               UINT offsets[_countof(vertex_buffers)]{ 0 };
               // ���_�o�b�t�@����͂Ƃ��Đݒ�
               immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
               // �v���~�e�B�u�̃C���f�b�N�X�o�b�t�@��ݒ�
               immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

               // �v���~�e�B�u�̃}�e���A����ϊ��s��Ȃǂ̏���ݒ�
               PrimitiveConstants primitive_data{};
               primitive_data.material = primitive.material;
               primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
               primitive_data.skin = node.skin;
               primitive_data.threshold = threshold;
               XMStoreFloat4x4(&primitive_data.world, XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));

               // �萔�o�b�t�@�� primitive_data �̓��e���R�s�[
               immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
               immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
               immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());


               const material& material{ materials.at(primitive.material) };
               // �}�e���A�����̊e�e�N�X�`���Ɋ֘A�t����ꂽ�C���f�b�N�X��z��Ƃ��Ď擾
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

               // �e�N�X�`���̃C���f�b�N�X�����[�v���A�e�e�N�X�`���ɑΉ�����V�F�[�_�[���\�[�X�r���[��ݒ�
               for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
               {
                   shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                     texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                     null_shader_resource_view;
               }
               // �s�N�Z���V�F�[�_�[�ɃV�F�[�_�[���\�[�X�r���[��ݒ�
               immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

               // �v���~�e�B�u��`��
               immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
           }
       }

       // �e�m�[�h�������b�V����`��
       for (std::vector<int>::value_type child_index : node.children)
       {
           traverse(child_index);
       }
      } };

    // �e�m�[�h�������b�V����`��
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
        // �w�肳�ꂽ�A�j���[�V�����C���f�b�N�X�ɑΉ�����A�j���[�V�������擾
        const animation& animation{ animations.at(animation_index) };

        // �A�j���[�V�����̊e�`�����l���ɑ΂��ď������s��
        for (vector<animation::channel>::const_reference channel : animation.channels)
        {
            // �`�����l���ɑΉ�����T���v���[���擾
            const animation::sampler& sampler{ animation.samplers.at(channel.sampler) };

            // �T���v���[�̓��͂ƂȂ�^�C�����C�����擾
            const vector<float>& timeline{ animation.timelines.at(sampler.input) };

            // �^�C�����C���̃T�C�Y��0�̏ꍇ�̓X�L�b�v�i�s���ȃf�[�^�̉\�������邽�߁j
            if (timeline.size() == 0)
            {
                continue;
            }

            // �^�C�����C������w�肳�ꂽ���ԂɑΉ�����L�[�t���[�����擾
            float interpolation_factor{};
            size_t keyframe_index{ IndexOf(timeline, time, interpolation_factor, loopback) };

            // �`�����l���̃^�[�Q�b�g�p�X�Ɋ�Â��ăm�[�h�̕ϊ����v�Z
            if (channel.target_path == "scale")
            {
                // �X�P�[���ɑΉ�����L�[�t���[�����擾
                const vector<XMFLOAT3>& scales{ animation.scales.at(sampler.output) };

                // ��Ԃ��g���ăX�P�[�����v�Z���A�A�j���[�V�����m�[�h�ɐݒ�
                XMStoreFloat3(&animated_nodes.at(channel.target_node).scale, 
                    XMVectorLerp(XMLoadFloat3(&scales.at(keyframe_index + 0)),
                        XMLoadFloat3(&scales.at(keyframe_index + 1)), interpolation_factor));
            }
            else if (channel.target_path == "rotation")
            {
                // ��]�ɑΉ�����L�[�t���[�����擾
                const vector<XMFLOAT4>& rotations{ animation.rotations.at(sampler.output) };

                // ��Ԃ��g���ĉ�]���v�Z���A�A�j���[�V�����m�[�h�ɐݒ�
                XMStoreFloat4(&animated_nodes.at(channel.target_node).rotation,
                    XMQuaternionNormalize(XMQuaternionSlerp(XMLoadFloat4(&rotations.at(keyframe_index + 0)),
                        XMLoadFloat4(&rotations.at(keyframe_index + 1)), interpolation_factor)));
            }
            else if (channel.target_path == "translation")
            {
                // �ʒu�ɑΉ�����L�[�t���[�����擾
                const vector<XMFLOAT3>& translations{ animation.translations.at(sampler.output) };

                // ��Ԃ��g���Ĉʒu���v�Z���A�A�j���[�V�����m�[�h�ɐݒ�
                XMStoreFloat3(&animated_nodes.at(channel.target_node).translation,
                    XMVectorLerp(XMLoadFloat3(&translations.at(keyframe_index + 0)),
                        XMLoadFloat3(&translations.at(keyframe_index + 1)), interpolation_factor));
            }
        }
        // �A�j���[�V������̃m�[�h�̕ϊ���ݐς���
        CumulateTransforms(animated_nodes);
    }
    else
    {
        // �A�j���[�V�������Ȃ��ꍇ�́A���̃m�[�h�̏�Ԃ����̂܂܎g��
        animated_nodes = nodes;
    }
}


HRESULT GltfModel::LoadTextures(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color)
{
    // �p�X�𕪉�
    char drive[256], dirname[256], fname[256], ext[256];
    ::_splitpath_s(filename, drive, sizeof(drive), dirname, sizeof(dirname), fname, sizeof(fname), ext, sizeof(ext));

    // ����������ǉ�
    if (suffix != nullptr)
    {
        ::strcat_s(fname, sizeof(fname), suffix);
    }
    // �p�X������
    char filepath[256];
    ::_makepath_s(filepath, 256, drive, dirname, fname, ext);

    // �}���`�o�C�g�������烏�C�h�����֕ϊ�
    wchar_t wfilepath[256];
    ::MultiByteToWideChar(CP_ACP, 0, filepath, -1, wfilepath, 256);

    // �e�N�X�`���ǂݍ���
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilepath, resource.GetAddressOf(), srv);
    if (FAILED(hr))
    {
        // WIC�ŃT�|�[�g����Ă��Ȃ��t�H�[�}�b�g�̏ꍇ�iTGA�Ȃǁj��
        // STB�ŉ摜�ǂݍ��݂����ăe�N�X�`���𐶐�����
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

            // ��n��
            stbi_image_free(pixels);
        }
        else if (dummy)
        {
            // �ǂݍ��ݎ��s������_�~�[�e�N�X�`�������
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
    // �^�C�����C���̃L�[�t���[�������擾
    const size_t keyframe_count{ timelines.size() };

    // ���Ԃ��Ō�̃L�[�t���[���𒴂����ꍇ�̏���
    if (time > timelines.at(keyframe_count - 1))
    {
        if (loopback)
        {
            // ���[�v����ꍇ�͎��Ԃ����[�v������
            time = fmodf(time, timelines.at(keyframe_count - 1));
        }
        else
        {
            // ���[�v���Ȃ��ꍇ�͍Ō�̃L�[�t���[���̒��O�̈ʒu��Ԃ�
            interpolation_factor = 1.0f;
            end_flag = true;
            return keyframe_count - 2;  // �Ō�̃L�[�t���[���̒��O�̃C���f�b�N�X��Ԃ�
        }
    }
    // ���Ԃ��ŏ��̃L�[�t���[�������O�̏ꍇ�̏���
    else if (time < timelines.at(0))
    {
        // �ŏ��̃L�[�t���[���̈ʒu��Ԃ�
        interpolation_factor = 0.0f;
        return 0;
    }

    // �ł��߂��L�[�t���[����T��
    size_t keyframe_index{ 0 };
    for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
    {
        if (time < timelines.at(time_index))
        {
            // ���Ԃ����݂̃L�[�t���[�������������ꍇ�A���̒��O�̃L�[�t���[����I��
            keyframe_index = std::max<size_t>(0LL, time_index - 1);
            break;
        }
    }

    // ��ԌW�����v�Z
    interpolation_factor = (time - timelines.at(keyframe_index + 0)) /
        (timelines.at(keyframe_index + 1) - timelines.at(keyframe_index + 0));

    // �ł��߂��L�[�t���[���̃C���f�b�N�X��Ԃ�
    return keyframe_index;
}

void GltfModel::FetchExternalTextures(ID3D11Device* device, const tinygltf::Model& gltf_model, const std::string& gltf_filename)
{
    for (auto& gltf_material : materials)
    {
        // �f�B���N�g���p�X�擾
        char drive[32], dir[256], dirname[256], fname[256];
        ::_splitpath_s(gltf_filename.c_str(), drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), nullptr, 0);

        // �t�@�C���K�w��ǉ�
        ::_makepath_s(dirname, sizeof(dirname), drive, dir, fname, nullptr);

        // �}�e���A���̖��O
        char tname[256] = {};
        // std::string�igltf_material.name�j��char�z��itname�j�ɃR�s�[
        std::copy(gltf_material.name.begin(), gltf_material.name.end(), tname);

        // ����������ǉ�
        ::strcat_s(tname, sizeof(tname), ".png");

        // ���΃p�X�̉���
        char filename[256];
        ::_makepath_s(filename, 256, nullptr, dirname, tname, nullptr);


        // �x�[�X�J���[
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

        // �m�[�}��
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

        //TODO ���񃉃t�l�X�݂̂Ȃ̂Ń��t�l�X
        // ���^���l�X����Ƃ��͏�������
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
    // GPU��̃V�F�[�_�[�E���\�[�X�E�r���[�Ƃ��ă}�e���A���E�f�[�^���쐬����
    std::vector<material::cbuffer> material_data;
    for (std::vector<material>::const_reference material : materials)
    {
        material_data.emplace_back(material.data);
    }

    HRESULT hr;
    // GPU ��Ń}�e���A���f�[�^���i�[���邽�߂̃o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;

    D3D11_BUFFER_DESC buffer_desc{};
    // �o�b�t�@�̃T�C�Y���o�C�g�P��
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(material::cbuffer) * material_data.size());
    // �v�f�̍\���̃T�C�Y
    buffer_desc.StructureByteStride = sizeof(material::cbuffer);
    // �o�b�t�@�̎g�p���@
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    // �o�b�t�@�̃o�C���h���@
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;// �V�F�[�_�[���\�[�X�Ƃ��ăo�C���h
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = material_data.data();
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // �V�F�[�_�[���\�[�X�r���[�̐ݒ���s�����߂̍\����
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    // �o�b�t�@�̃t�H�[�}�b�g
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    // �r���[�̎���
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    // �o�b�t�@���̗v�f��
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
    // �A�j���[�V������̃m�[�h�̕ϊ���ݐς���
    CumulateTransforms(*node_);
}

