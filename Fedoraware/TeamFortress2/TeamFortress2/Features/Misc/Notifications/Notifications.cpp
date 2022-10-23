#include "Notifications.h"

// Myzarfin added this
void CNotifications::Think()
{
	constexpr int x{ 1 };
	int y{ 1 };
	constexpr int size{ 20 };

	if (NotificationTexts.size() > (MAX_NOTIFY_SIZE + 1))
	{
		NotificationTexts.erase(NotificationTexts.begin());
	}

	for (size_t i{}; i < NotificationTexts.size(); ++i)
	{
		const auto notify = NotificationTexts[i];

		notify->Time -= I::GlobalVars->absoluteframetime;

		if (notify->Time <= 0.f)
		{
			NotificationTexts.erase(NotificationTexts.begin() + i);
		}
	}

	if (NotificationTexts.empty())
	{
		return;
	}

	for (size_t i{}; i < NotificationTexts.size(); ++i)
	{
		const auto notify = NotificationTexts[i];

		const float left = notify->Time;
		Color_t color = notify->Color;

		if (left < .5f)
		{
			float f = left;
			Math::Clamp(f, 0.f, .5f);

			f /= .5f;

			color.a = static_cast<int>(f * 255.f);

			if (i == 0 && f < 0.2f)
			{
				y -= size * (1.f - f / 0.2f);
			}
		}

		else
		{
			color.a = 255;
		}

		const size_t cSize = strlen(notify->Text.c_str()) + 1;
		const auto wc = new wchar_t[cSize];
		mbstowcs(wc, notify->Text.c_str(), cSize);

		int w, h;

		I::VGuiSurface->GetTextSize(FONT_INDICATORS, wc, w, h);

		delete[] wc; // Memory leak

		g_Draw.Line(x, y, x, y + 19, { Colors::NotifOutline.r, Colors::NotifOutline.g, Colors::NotifOutline.b, color.a });
		g_Draw.GradientRectA(x + 1, y, w / 3 + 9, y + 19,
							 { Colors::NotifBG.r, Colors::NotifBG.g, Colors::NotifBG.b, color.a },
							 {
								 Colors::NotifBG.r, Colors::NotifBG.g, Colors::NotifBG.b, 1
							 }, true);
		g_Draw.String(FONT_INDICATORS, x + 6, y + 2,
					  { Colors::NotifText.r, Colors::NotifText.g, Colors::NotifText.b, color.a },
					  ALIGN_DEFAULT, notify->Text.c_str());

		y += size;
	}
}