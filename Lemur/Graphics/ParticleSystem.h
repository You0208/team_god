#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include "Texture.h"
#include "Shader.h"

class ParticleSystem
{
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 position; // �ʒu
        DirectX::XMFLOAT2 texcoord; // UV
        DirectX::XMFLOAT4 color; // ���_�F
        DirectX::XMFLOAT4 param; // �ėp�p�����[�^
    };
    struct ParticleData
    {
        float x, y, z;
        float w, h;
        float aw, ah;
        float vx, vy, vz;
        float ax, ay, az;
        float alpha;
        float timer;
        float anime_timer;
        float type;
        bool anime;
        float anime_speed;
    };
    // �萔�o�b�t�@�̃f�[�^��`
    struct ConstantBufferForPerFrame
    {
        DirectX::XMFLOAT2 size;
        DirectX::XMFLOAT2 dummy;
    };

public:
    ParticleSystem(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
        shader_resource_view, int komax = 1, int komay = 1, int num = 1000);
    ~ParticleSystem();
    void Update(float elapsed_time);
    void Render(ID3D11DeviceContext* immediate_context);
    void Set(
        int type,
        float timer,
        DirectX::XMFLOAT3 p,
        DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
        DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
        DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f),
        bool anime = false,
        float anime_speed = 24.0f
    );

private:
    ParticleData* data; // �p�[�e�B�N�����
    Vertex* v; // ���_�o�b�t�@�������ݏ��
    int num_particles = 0;// �p�[�e�B�N����
    int komax, komay; // Texture�̏c��������

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
};