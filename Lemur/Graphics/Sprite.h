#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>

class Sprite
{
private:
    // �����o�ϐ��iCOM�I�u�W�F�N�g�� ComPtr �X�}�[�g�|�C���^�[�e���v���[�g�Ɂj
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    D3D11_TEXTURE2D_DESC texture2d_desc;

public:
    bool is_anime_end = false;

    int anime_x_max = 0;
    int anime_y_max = 0;

    int anime_x = 0;
    int anime_y = 0;

    // ���_�t�H�[�}�b�g�i���R�ɍ���j
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    // �R���X�g���N�^�E�f�X�g���N�^
    Sprite(ID3D11Device* device, const wchar_t* filename);
    ~Sprite() {}

    // �����o�֐�

    void Render(ID3D11DeviceContext*,
        float dx, float dy,// ��`�̍���̍��W�i�X�N���[�����W�n�j
        float dw, float dh, // ��`�̃T�C�Y�i�X�N���[�����W�n�j
        float r, float g, float b, float a,
        float angle/*degree*/);
    // �I�[�o�[���[�h
    void Render(ID3D11DeviceContext*,
        float dx, float dy,// ��`�̍���̍��W�i�X�N���[�����W�n�j
        float dw, float dh, // ��`�̃T�C�Y�i�X�N���[�����W�n�j
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);

    void Render(ID3D11DeviceContext*,
        ID3D11PixelShader* replaced_pixel_shader,
        float dx, float dy,// ��`�̍���̍��W�i�X�N���[�����W�n�j
        float dw, float dh, // ��`�̃T�C�Y�i�X�N���[�����W�n�j
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);
    void Render(ID3D11DeviceContext* immediate_context,ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle);
    void RenderCenter(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle);


    void RenderRightDown(ID3D11DeviceContext*,
        float dx, float dy,// ��`�̍���̍��W�i�X�N���[�����W�n�j
        float dw, float dh, // ��`�̃T�C�Y�i�X�N���[�����W�n�j
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);

    void RenderRightUp(ID3D11DeviceContext*,
        float dx, float dy,// ��`�̍���̍��W�i�X�N���[�����W�n�j
        float dw, float dh, // ��`�̃T�C�Y�i�X�N���[�����W�n�j
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);

    void RenderRightDown(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle);
    void RenderRightUp(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle);

    void RenderCenter(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);
    void RenderCenter(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle);
    void Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);
    void Textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a);
    void Textout(ID3D11DeviceContext* immediate_context, int n, float x, float y, float w, float h, float r, float g, float b, float a);

    void Animation(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color,
        float angle, DirectX::XMFLOAT2 texsize, int last, bool loop = true, bool stop = false);

    float timer;
};