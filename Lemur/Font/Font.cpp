#include "Font.h"

namespace Lemur::Graphics
{
	Font* Font::fontInstance = nullptr;

	void Font::initialize()
	{
		HRESULT hr{ S_OK };
#ifdef ENABLE_DIRECT2D
		//Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
		//g_pD2DFactory = graphics.Getg_pD2DF();
		//g_pBackBuffer =graphics.Getg_pBackBuffer();
		//g_pDWriteFactory =graphics.Getg_pDWriteFactory();
		//g_pTextFormat =graphics.Getg_pTextFormat();
		//g_pRT =graphics.Getg_pRT();
		//g_pSolidBrush =graphics.Getg_pSolidBrush();
#endif

	}
	void Font::render()
	{
#ifdef ENABLE_DIRECT2D
		Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

		//--------------------------š’Ç‰Á«--------------------------
		// ƒeƒLƒXƒg‚Ì•`‰æ
		WCHAR wcText1[] = L"e÷‚è‚Ì–³“S–C‚Å¬‹Ÿ‚ÌŽž‚©‚ç‘¹‚Î‚©‚è‚µ‚Ä‚¢‚éB¬ŠwZ‚É‹‚éŽž•ªŠwZ‚Ì“ñŠK‚©‚ç";
		WCHAR wcText2[] = L"”ò‚Ñ~‚è‚ÄˆêTŠÔ‚Ù‚Ç˜‚ð”²‚©‚µ‚½Ž–‚ª‚ ‚éB‚È‚º‚»‚ñ‚È–³ˆÅ‚ð‚µ‚½‚Æ•·‚­l‚ª‚ ‚é‚©";
		WCHAR wcText3[] = L"‚à’m‚ê‚ÊB•Ê’i[‚¢——R‚Å‚à‚È‚¢BV’z‚Ì“ñŠK‚©‚çŽñ‚ðo‚µ‚Ä‚¢‚½‚çA“¯‹‰¶‚Ìˆêl‚ª";
		WCHAR wcText4[] = L"ç’k‚ÉA‚¢‚­‚çˆÐ’£‚Á‚Ä‚àA‚»‚±‚©‚ç”ò‚Ñ~‚è‚éŽ–‚Ío—ˆ‚Ü‚¢BŽã’Ž‚â[‚¢B‚Æš’‚µ‚½";
		WCHAR wcText5[] = L"‚©‚ç‚Å‚ ‚éB¬Žg‚É•‰‚Ô‚³‚Á‚Ä‹A‚Á‚Ä—ˆ‚½ŽžA‚¨‚â‚¶‚ª‘å‚«‚ÈŠá‚ð‚µ‚Ä“ñŠK‚®‚ç‚¢‚©‚ç";
		WCHAR wcText6[] = L"”ò‚Ñ~‚è‚Ä˜‚ð”²‚©‚·“z‚ª‚ ‚é‚©‚Æ‰]‚Á‚½‚©‚çA‚±‚ÌŽŸ‚Í”²‚©‚³‚¸‚É”ò‚ñ‚ÅŒ©‚¹‚Ü‚·‚Æ";
		WCHAR wcText7[] = L"“š‚¦‚½B";

		graphics.g_pRT->BeginDraw();
		graphics.g_pRT->DrawText(wcText1, ARRAYSIZE(wcText1) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 0, 800, 20), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText2, ARRAYSIZE(wcText2) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 20, 800, 40), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText3, ARRAYSIZE(wcText3) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 40, 800, 60), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText4, ARRAYSIZE(wcText4) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 60, 800, 80), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText5, ARRAYSIZE(wcText5) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 80, 800, 100), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText6, ARRAYSIZE(wcText6) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 100, 800, 120), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->DrawText(wcText7, ARRAYSIZE(wcText7) - 1, graphics.g_pTextFormat, D2D1::RectF(0, 120, 800, 140), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->EndDraw();
		//--------------------------š’Ç‰Áª--------------------------
#endif
	}

	void Font::render(const WCHAR wcText1[],int TextNum,DirectX::XMFLOAT2 pos, float width, float hight)
	{
#ifdef ENABLE_DIRECT2D
		Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

		// ƒeƒLƒXƒg‚Ì•`‰æ
		graphics.g_pRT->BeginDraw();
		graphics.g_pRT->DrawText(wcText1, TextNum - 1, graphics.TextFormat_1, D2D1::RectF(pos.x, pos.y, pos.x + width, pos.y + hight), graphics.g_pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		graphics.g_pRT->EndDraw();
#endif
	}

};