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
    std::string filename;    // ファイルパス
public:
    GltfModel(ID3D11Device* device, const std::string& filename,bool external_texture/*モデル外からのテクスチャ読み込みをするか*/);
    virtual ~GltfModel() = default;

    // sceneの基本情報
    struct scene
    {
        std::string name;
        std::vector<int> nodes; // Array of 'root' nodes
    };
    std::vector<scene> scenes;

    // ノード構造体
    struct node
    {
        std::string name;
        int skin{ -1 }; // このノードが参照するスキンのインデックス
        int mesh{ -1 }; // このノードが参照しているメッシュのインデックス

        std::vector<int> children; //このノードの子ノードのインデックスの配列

        // Local transforms
        DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
        DirectX::XMFLOAT3 scale{ 1, 1, 1 };
        DirectX::XMFLOAT3 translation{ 0, 0, 0 };

        DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    };
    std::vector<node> nodes;

    struct buffer_view
    {
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;// バッファ内のデータのフォーマット
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;// バッファへのポインタを格納
        size_t stride_in_bytes{ 0 };// バッファ内の1要素のサイズ
        size_t size_in_bytes{ 0 };// バッファ全体のサイズ
        size_t count() const// 要素数
        {
            return size_in_bytes / stride_in_bytes;
        }
    };
    // 3Dオブジェクトのジオメトリ情報
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

    // テクスチャ
    struct TextureInfo
    {
        int index = -1;   // テクスチャのインデックス
        int texcoord = 0; // テクスチャ座標のインデックス
    };
    // 法線マップ
    struct NormalTextureInfo
    {
        int index = -1;   // 法線マップのインデックス
        int texcoord = 0; // テクスチャ座標のインデックス
        float scale = 1;  // 法線マップのスケール
    };
    // オクルージョンテクスチャ
    struct OcclusionTextureInfo
    {
        int index = -1;     // オクルージョンテクスチャのインデックス
        int texcoord = 0;;  // テクスチャ座標のインデックス
        float strength = 1; // オクルージョンテクスチャの強度
    };
    // PBRテクスチャ
    struct PbrMetallicRoughness
    {
        float basecolor_factor[4] = { 1, 1, 1, 1 }; // ベースカラーのファクター
        TextureInfo basecolor_texture;             // ベースカラーテクスチャ
        float metallic_factor = 1;                  // メタリックファクター
        float roughness_factor = 1;                 // ラフネスファクター
        TextureInfo metallic_roughness_texture;    // メタリックとラフネスのテクスチャ
    };
    // マテリアル
    struct material {

        std::string name; // マテリアルの名前

        // 定数バッファー
        struct cbuffer
        {
            // エミッシブ
            float emissive_factor[3] = { 0, 0, 0 }; // エミッシブファクター
            int alpha_mode = 0;                     // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
            float alpha_cutoff = 0.5f;              // アルファカットオフ
            bool double_sided = false;              // 両面表示

            // 各テクスチャの値
            PbrMetallicRoughness pbr_metallic_roughness; // PBRテクスチャ情報
            NormalTextureInfo normal_texture;            // 法線マップ情報
            OcclusionTextureInfo occlusion_texture;      // オクルージョンテクスチャ情報
            TextureInfo emissive_texture;                // エミッシブテクスチャ情報
        };
        cbuffer data; // マテリアルの定数バッファーデータ
    };
    std::vector<material> materials;// マテリアルを格納するための配列
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;// マテリアルのシェーダーリソースビューを表すポインタ
    struct texture
    {
        std::string name;
        int source{ -1 };
    };
    std::vector<texture> textures;
    struct image
    {
        std::string name;
        int width{ -1 };      // 幅
        int height{ -1 };     // 高さ
        int component{ -1 };  // 色成分
        int bits{ -1 };       // ビット数
        int pixel_type{ -1 }; // ピクセルタイプ
        int buffer_view;      // 画像データが格納されているバッファビューに対する参照
        std::string mime_type;// MIMEタイプ
        std::string uri;      // URI（Uniform Resource Identifier）
        bool as_is{ false };  // 画像がそのままの形式で保存されているかどうか
    };
    std::vector<image> images;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;


    struct skin
    {
        std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
        std::vector<int> joints;
    };
    std::vector<skin> skins;

    // アニメーション情報
    struct animation
    {
        std::string name;

        // アニメーションのチャンネル情報
        struct channel
        {
            int sampler{ -1 };      // サンプラーのインデックス
            int target_node{ -1 };  // 対象ノードのインデックス
            std::string target_path;// 対象のプロパティパス（例えば、"translation"、"rotation"、"scale" など）
        };
        std::vector<channel> channels;

        // アニメーションのサンプラー情報
        struct sampler
        {
            int input{ -1 };// 入力データ
            int output{ -1 };// 出力データ
            std::string interpolation;// 補間方法
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

    // テクスチャ読み込み用
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> base_color;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness_smoothness;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
private:
    // ノード取得
    void FetchNodes(const tinygltf::Model& gltf_model);
    void CumulateTransforms(std::vector<node>& nodes);
    // アクセサを受け取り、それに基づいてバッファビューを作成
    buffer_view MakeBufferView(const tinygltf::Accessor& accessor);
    // メッシュ情報を取得
    void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // マテリアル取得
    void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // テクスチャ取得
    void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltf_model);
    // アニメーション取得
    void FetchAnimations(const tinygltf::Model& gltf_model);

    // 外部からのテクスチャ読み込み
    void FetchExternalTextures(ID3D11Device* device, const tinygltf::Model& gltf_model, const std::string& filename);

    void MaterialForGPU(ID3D11Device* device);

    // テクスチャ読み込み
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

    // 試し
    int index_count = 0;
};