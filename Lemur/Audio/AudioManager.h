#pragma once

#include <wrl.h>
#include <memory>
#include <xaudio2.h>

#include "Audio.h"


namespace Lemur::Audio
{
    enum class BGM
    {
        BOSS,
        GAMBLE,
        CLEAR,
        OVER,
        PLAY,
        TITLE,
        BGM_Max        
    };
    enum class SE
    {
        ARROW,
        AVOID,
        CONTER,
        DAMAGE,
        DECISION,
        HIT,
        SENI,
        SWING,
        SE_Max
    };
    
    class AudioManager
    {
    private:
        AudioManager() {};
        ~AudioManager() {};
    public:
        static AudioManager& Instance()
        {
            static AudioManager instance;
            return instance;
        }

        void LoadAudio();

        void PlayBgm(
            const BGM& bgm,
            const bool isloop
        );

        // SE�Đ�
        void PlaySe(
            const SE& se,
            const bool isLoop
        );

        void StopBGM(const BGM& bgm);                   // BGM��~
        void StopSE(const SE& se);                      // SE��~

        void StopAllBGM();                              // �SBGM��~
        void StopAllSE();                               // �SSE��~
        void StopAllAudio();                            // �S���y��~

    public:
        std::unique_ptr< Lemur::Audio::Audio>& GetBGM(const BGM& bgm); //�@BGM�擾
        std::unique_ptr< Lemur::Audio::Audio>& GetSE(const SE& se);    //  SE�擾

    public:
        // XAUDIO2
        Microsoft::WRL::ComPtr<IXAudio2> xAudio2 = nullptr;
        IXAudio2MasteringVoice* masterVoice = nullptr;

    private:
        std::unique_ptr< Lemur::Audio::Audio> bgm[static_cast<int>(BGM::BGM_Max)] = {};
        std::unique_ptr< Lemur::Audio::Audio> se[static_cast<int>(SE::SE_Max)] = {};
    };
}