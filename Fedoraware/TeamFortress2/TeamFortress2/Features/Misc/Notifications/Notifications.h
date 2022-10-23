#pragma once
#include "../../SDK/SDK.h"

constexpr int MAX_NOTIFY_SIZE = 8;

class NotifyText
{
public:
	std::string Text;
	Color_t Color;
	float Time;

	NotifyText() {}
	NotifyText(std::string text, Color_t color, float time) : Text{ std::move(text) }, Color{ color }, Time{ time } { }
};

class CNotifications
{
	std::vector<std::shared_ptr<NotifyText>> NotificationTexts;

public:
	CNotifications() = default;

	__forceinline void Add(const std::string& text, Color_t color = { 255, 255, 255, 255 },
						   float time = Vars::Visuals::NotificationLifetime.Value)
	{
		NotificationTexts.push_back(std::make_shared<NotifyText>(text, color, time));
	}

	void Think();
};

ADD_FEATURE(CNotifications, Notifications)