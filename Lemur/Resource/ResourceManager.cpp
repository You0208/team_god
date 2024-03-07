#include "ResourceManager.h"
#include <locale>
#include <string>

const std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(
    ID3D11Device* device,
    const char* const       fbx_filename,
    const bool              triangulate,
    const float             sampling_rate)
{
    // モデルを検索
    const ModelMap::iterator it = models.find(fbx_filename);
    if (it != models.end())
    {
        // リンク(寿命)が切れていないか確認
        if (it->second.expired() == false)
        {
            // 検索したモデルと同じモデルが見つかったらそれを返す
            return it->second.lock();
        }
    }

    // 見つからなかったら新規モデルリソースを作成・読み込み
    const std::shared_ptr<SkinnedMesh> model = {
        std::make_shared<SkinnedMesh>(
            device,
            fbx_filename,
            triangulate,
            sampling_rate)
    };

    // マップに登録
    models[fbx_filename] = model;

    return model;
}

const std::shared_ptr<Sprite> ResourceManager::load_sprite_resource(ID3D11Device* device, const wchar_t* const filename)
{
    // この文字列を使用して他のタイプの文字列を作成します。
    const wchar_t* orig = filename;

    // wchar_t 文字列を char* 文字列に変換します。 記録
    // 元の文字列の長さに 1 を加えます。
    // 終端の null 文字を考慮します。
    size_t origsize = wcslen(orig) + 1;
    size_t convertedChars = 0;

    // マルチバイト文字列を使用して文字列の種類を追加します
    // 結果を表示する前に新しい文字列に変換します
    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;

    // マルチバイト出力文字列にワイドごとに 2 バイトを割り当てます。
    // 入力文字列内の文字 (ワイド文字を含む)
    // ヌル）。 マルチバイト文字は 1 バイトまたは 2 バイトであるため、
    // 各文字に 2 バイトを割り当てる必要があります。 余分に持っている
    // 新しい文字列のスペースはエラーではありませんが、
    // スペース不足は潜在的なセキュリティ上の問題です。
    const size_t newsize = origsize * 2;
    // 新しい文字列には、元の文字列の変換されたコピーが含まれます。
         // 文字列とそれに追加される文字列のタイプ。
    char* nstring = new char[newsize + strConcatsize];

    // 変換された文字列のコピーを nstring に配置します
    wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);


    const sprite_map::iterator it = sprites.find(nstring);
    if (it != sprites.end())
    {
        // リンク(寿命)が切れていないか確認
        if (it->second.expired() == false)
        {
            // 検索したモデルと同じモデルが見つかったらそれを返す
            return it->second.lock();
        }
    }

    // 見つからなかったら新規モデルリソースを作成・読み込み
    const std::shared_ptr<Sprite> sprite_ = {
        std::make_shared<Sprite>(
            device,
            filename)
    };

    // マップに登録
    sprites[nstring] = sprite_;

    return sprite_;
}


const std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(
    ID3D11Device* device,
    const char* const                  fbx_filename,
    std::vector<std::string> animation_filenames,
    const bool                         triangulate,
    const float                        sampling_rate)
{
    // モデルを検索
    const ModelMap::iterator it = models.find(fbx_filename);
    if (it != models.end())
    {
        // リンク(寿命)が切れていないか確認
        if (it->second.expired() == false)
        {
            // 検索したモデルと同じモデルが見つかったらそれを返す
            return it->second.lock();
        }
    }

    // 見つからなかったら新規モデルリソースを作成・読み込み
    const std::shared_ptr<SkinnedMesh> model = {
        std::make_shared<SkinnedMesh>(
            device,
            fbx_filename,
            animation_filenames,
            triangulate,
            sampling_rate)
    };

    // マップに登録
    models[fbx_filename] = model;

    return model;
}

