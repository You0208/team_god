#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Model/FbxModelManager.h"
#include "Lemur/Effekseer/Effect.h"
class Seed :public Character
{
public:
    Seed();
    ~Seed()override;

    void Update(float elapsedTime);

    // �`�揈��
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    void DrawDebugGUI();

    float GetTimer() { return timer; };
    int GetNumber() { return number; };
    bool GetBorn() { return born; };
    void SetNumber(int number_) { number= number_; };
    void SetCategory(int category_) { category = category_; };
    void DecNumber() { number--; }

private:
    float timer;// �킪�z�u����Ă���̎���
    int number;// ��ԍ�
    int category;// ��̎��
    bool born;// ���j�b�g�����܂�邩�ǂ���
};


