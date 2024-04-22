#pragma once
#define NOMINMAX
#include<d3d11.h>
#include<wrl.h>
#include <DirectXMath.h>
#include <unordered_map>
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../External/tinygltf-release/tiny_gltf.h"

class GltfModel
{
    std::string filename;
public:
    GltfModel(ID3D11Device* device, const std::string& filename);

    virtual ~GltfModel() = default;
    struct scene
    {
        std::string name;
        std::vector<int> nodes; // Array of 'root' nodes
    };
    std::vector<scene> scenes;

    struct node
    {
        std::string name;
        int skin{ -1 }; // index of skin referenced by this node
        int mesh{ -1 }; // index of mesh referenced by this node

        std::vector<int> children; // An array of indices of child nodes of this node

        // Local transforms
        DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
        DirectX::XMFLOAT3 scale{ 1, 1, 1 };
        DirectX::XMFLOAT3 translation{ 0, 0, 0 };

        DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    };
    std::vector<node> nodes;

    struct buffer_view
    {
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
        size_t stride_in_bytes{ 0 };
        size_t size_in_bytes{ 0 };
        size_t count() const
        {
            return size_in_bytes / stride_in_bytes;
        }
    };
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

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    struct primitive_constants
    {
        DirectX::XMFLOAT4X4 world;
        int material{ -1 };
        int has_tangent{ 0 };
        int skin{ -1 };
        int pad;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;

    struct texture_info
    {
        int index = -1;
        int texcoord = 0;
    };
    struct normal_texture_info
    {
        int index = -1;
        int texcoord = 0;
        float scale = 1;
    };
    struct occlusion_texture_info
    {
        int index = -1;
        int texcoord = 0;
        float strength = 1;
    };
    struct pbr_metallic_roughness
    {
        float basecolor_factor[4] = { 1, 1, 1, 1 };
        texture_info basecolor_texture;
        float metallic_factor = 1;
        float roughness_factor = 1;
        texture_info metallic_roughness_texture;
    };
    struct material
    {
        std::string name;
        struct cbuffer
        {
            float emissive_factor[3] = { 0, 0, 0 };
            int alpha_mode = 0; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
            float alpha_cutoff = 0.5f;
            bool double_sided = false;

            pbr_metallic_roughness pbr_metallic_roughness;

            normal_texture_info normal_texture;
            occlusion_texture_info occlusion_texture;
            texture_info emissive_texture;
        };
        cbuffer data;

    };
    std::vector<material> materials;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;
    struct texture
    {
        std::string name;
        int source{ -1 };
    };
    std::vector<texture> textures;
    struct image
    {
        std::string name;
        int width{ -1 };
        int height{ -1 };
        int component{ -1 };
        int bits{ -1 };
        int pixel_type{ -1 };
        int buffer_view;
        std::string mime_type;
        std::string uri;
        bool as_is{ false };
    };
    std::vector<image> images;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;
    
    struct skin
    {
        std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
        std::vector<int> joints;
    };
   std::vector<skin> skins;
   
   struct animation
   {
       std::string name;

       struct channel
       {
           int sampler{ -1 };
           int target_node{ -1 };
           std::string target_path;
       };
       std::vector<channel> channels;

       struct sampler
       {
           int input{ -1 };
           int output{ -1 };
           std::string interpolation;
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
public:
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes);
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader* replaced_pixel_shader);
    void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader** replaced_pixel_shader);
    void Animate(size_t animation_index, float time, std::vector<node>& animated_nodes, bool loopback);
private:
    void FetchAnimations(const tinygltf::Model& gltf_model);
    void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltf_model);
    void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model);
    void FetchNodes(const tinygltf::Model& gltf_model);
    void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model);
    void CumulateTransforms(std::vector<node>& nodes);
    buffer_view GltfModel::MakeBufferView(const tinygltf::Accessor& accessor);
};