#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <unordered_map>
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../External/tinygltf-release/tiny_gltf.h"

class GltfModel
{
    std::string filename;    // �t�@�C���p�X
public:
    GltfModel(ID3D11Device* device, const std::string& filename,bool external_texture/*���f���O����̃e�N�X�`���ǂݍ��݂����邩*/);
    virtual ~GltfModel() = default;

    // scene�̊�{���
    struct scene
    {
        std::string name;
        std::vector<int> nodes; // Array of 'root' nodes
    };
    std::vector<scene> scenes;

    // �m�[�h�\����
    struct node
    {
        std::string name;
        int skin{ -1 }; // ���̃m�[�h���Q�Ƃ���X�L���̃C���f�b�N�X
        int mesh{ -1 }; // ���̃m�[�h���Q�Ƃ��Ă��郁�b�V���̃C���f�b�N�X

        std::vector<int> children; //���̃m�[�h�̎q�m�[�h�̃C���f�b�N�X�̔z��

        // Local transforms
        DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
        DirectX::XMFLOAT3 scale{ 1, 1, 1 };
        DirectX::XMFLOAT3 translation{ 0, 0, 0 };

        DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    };
    std::vector<node> nodes;

    struct buffer_view
    {
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;// �o�b�t�@���̃f�[�^�̃t�H�[�}�b�g
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;// �o�b�t�@�ւ̃|�C���^���i�[
        size_t stride_in_bytes{ 0 };// �o�b�t�@����1�v�f�̃T�C�Y
        size_t size_in_bytes{ 0 };// �o�b�t�@�S�̂̃T�C�Y
        size_t count() const// �v�f��
        {
            return size_in_bytes / stride_in_bytes;
        }
    };
    // 3D�I�u�W�F�N�g�̃W�I���g�����
    struct mesh
    {
        std::string name;

        struct primitive
        {
            int material;
            std::map<std::string, buffer_view> vertex_buffer_views;
            buffer_view index_buffer_view;
        };
        std::vector<primitive> primitives;
    };
    std::vector<mesh> meshes;

    // �e�N�X�`��
    struct TextureInfo
    {
        int index = -1;   // �e�N�X�`���̃C���f�b�N�X
        int texcoord = 0; // �e�N�X�`�����W�̃C���f�b�N�X
    };
    // �@���}�b�v
    struct NormalTextureInfo
    {
        int index = -1;   // �@���}�b�v�̃C���f�b�N�X
        int texcoord = 0; // �e�N�X�`�����W�̃C���f�b�N�X
        float scale = 1;  // �@���}�b�v�̃X�P�[��
    };
    // �I�N���[�W�����e�N�X�`��
    struct OcclusionTextureInfo
    {
        int index = -1;     // �I�N���[�W�����e�N�X�`���̃C���f�b�N�X
        int texcoord = 0;;  // �e�N�X�`�����W�̃C���f�b�N�X
        float strength = 1; // �I�N���[�W�����e�N�X�`���̋��x
    };
    // PBR�e�N�X�`��
    struct PbrMetallicRoughness
    {
        float basecolor_factor[4] = { 1, 1, 1, 1 }; // �x�[�X�J���[�̃t�@�N�^�[
        TextureInfo basecolor_texture;             // �x�[�X�J���[�e�N�X�`��
        float metallic_factor = 1;                  // ���^���b�N�t�@�N�^�[
        float roughness_factor = 1;                 // ���t�l�X�t�@�N�^�[
        TextureInfo metallic_roughness_texture;    // ���^���b�N�ƃ��t�l�X�̃e�N�X�`��
    };
    // �}�e���A��
    struct material {

        std::string name; // �}�e���A���̖��O

        // �萔�o�b�t�@�[
        struct cbuffer
        {
            // �G�~�b�V�u
            float emissive_factor[3] = { 0, 0, 0 }; // �G�~�b�V�u�t�@�N�^�[
            int alpha_mode = 0;                     // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
            float alpha_cutoff = 0.5f;              // �A���t�@�J�b�g�I�t
            bool double_sided = false;              // ���ʕ\��

            // �e�e�N�X�`���̒l
            PbrMetallicRoughness pbr_metallic_roughness; // PBR�e�N�X�`�����
            NormalTextureInfo normal_texture;            // �@���}�b�v���
            OcclusionTextureInfo occlusion_texture;      // �I�N���[�W�����e�N�X�`�����
            TextureInfo emissive_texture;                // �G�~�b�V�u�e�N�X�`�����
        };
        cbuffer data; // �}�e���A���̒萔�o�b�t�@�[�f�[�^
    };
    std::vector<material> materials;// �}�e���A�����i�[���邽�߂̔z��
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;// �}�e���A���̃V�F�[�_�[���\�[�X�r���[��\���|�C���^
    struct texture
    {
        std::string name;
        int source{ -1 };
    };
    std::vector<texture> textures;
    struct image
    {
        std::string name;
        int width{ -1 };      // ��
        int height{ -1 };     // ����
        int component{ -1 };  // �F����
        int bits{ -1 };       // �r�b�g��
        int pixel_type{ -1 }; // �s�N�Z���^�C�v
        int buffer_view;      // �摜�f�[�^���i�[����Ă���o�b�t�@�r���[�ɑ΂���Q��
        std::string mime_type;// MIME�^�C�v
        std::string uri;      // URI�iUniform Resource Identifier�j
        bool as_is{ false };  // �摜�����̂܂܂̌`���ŕۑ�����Ă��邩�ǂ���
    };
    std::vector<image> images;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;


    struct skin
    {
        std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
        std::vector<int> joints;
    };
    std::vector<skin> skins;

    // �A�j���[�V�������
    struct animation
    {
        std::string name;

        // �A�j���[�V�����̃`�����l�����
        struct channel
        {
            int sampler{ -1 };      // �T���v���[�̃C���f�b�N�X
            int target_node{ -1 };  // �Ώۃm�[�h�̃C���f�b�N�X
            std::string target_path;// �Ώۂ̃v���p�e�B�p�X�i�Ⴆ�΁A"translation"�A"rotation"�A"scale" �Ȃǁj
        };
        std::vector<channel> channels;

        // �A�j���[�V�����̃T���v���[���
        struct sampler
        {
            int input{ -1 };// ���̓f�[�^
            int output{ -1 };// �o�̓f�[�^
            std::string interpolation;// ��ԕ��@
        };
        std::vector<sampler> samplers;

        std::unordered_map<int/*sampler.input*/, std::vector<float>> timelines;
        std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT3>> scales;
        std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT4>> rotations;
        std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT3>> translations;
    };
    std::vector<animation> animations;

    static const size_t PRIMITIVE_MAX_JOINTS = 512;
    struct primitive_joint_constants
    {
        DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_joint_cbuffer;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

    struct PrimitiveConstants
    {
        DirectX::XMFLOAT4X4 world;
        int material{ -1 };
        int has_tangent{ 0 };
        int skin{ -1 };
        float threshold = 1.0f;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;
    float threshold = 1.0f;

    // �e�N�X�`���ǂݍ��ݗp
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> base_color;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness_smoothness;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
private:
    // �m�[�h�擾
    void FetchNodes(const tinygltf::Model& gltf_model);
    void CumulateTransforms(std::vector<node>& nodes);
    // �A�N�Z�T���󂯎��A����Ɋ�Â��ăo�b�t�@�r���[���쐬
    buffer_view MakeBufferView(const tinygltf::Accessor& accessor);
    // ���b�V�������擾
    void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // �}�e���A���擾
    void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // �e�N�X�`���擾
    void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // �A�j���[�V�����擾
    void FetchAnimations(const tinygltf::Model& gltf_model);

    // �O������̃e�N�X�`���ǂݍ���
    void FetchExternalTextures(ID3D11Device* device, const tinygltf::Model& gltf_model, const std::string& filename);

    void MaterialForGPU(ID3D11Device* device);

    // �e�N�X�`���ǂݍ���
    HRESULT LoadTextures(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color = 0xFFFFFFFF);

public:
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes);
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader* replaced_pixel_shader);
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader** replaced_pixel_shader);
    void Animate(size_t animation_index, float time, std::vector<node>& animated_nodes, bool loopback);

    void BlendAnimation(const std::vector<node>* nodes[2], float factor, std::vector<GltfModel::node>* node);

    size_t IndexOf(const std::vector<float>& timelines, float time, float& interpolation_factor, bool loopback);

    float GetThreshold() { return threshold; }
    float SetThreshold(float threshold_) {  threshold= threshold_; }

    void DebugThreshold();

    // ����
    int index_count = 0;
};