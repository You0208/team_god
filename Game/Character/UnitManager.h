#pragma once
#pragma once

#include<vector>
#include<set>
#include "Unit.h"

// ���j�b�g�}�l�[�W���[
class UnitManager
{
private:
    UnitManager() {}
    ~UnitManager() {}
public:
    // �B��̃C���X�^���X�擾
    static UnitManager& Instance()
    {
        static UnitManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // ���j�b�g�o�^
    void Register(Unit* unit);

    // ���j�b�g�S�폜
    void Clear();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // ���j�b�g���擾
    int GetEnemyCount() const { return static_cast<int>(units.size()); }

    // ���j�b�g�擾
    Unit* GetEnemy(int index) { return units.at(index); }

    // ���j�b�g���m�̏Փ˔���
    void CollisionEnemyVsEnemis();

    // ���j�b�g�폜
    void Remove(Unit* units);

private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;
};