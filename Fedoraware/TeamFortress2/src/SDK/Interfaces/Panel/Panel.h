#pragma once
#include "../../Includes/Includes.h"

class CPanel
{
public:
	const char *GetName(unsigned int vgui_panel)
	{
		typedef const char *(__thiscall *FN)(PVOID, unsigned int);
		return GetVFunc<FN>(this, 36)(this, vgui_panel);
	}

	void SetMouseInputEnabled(unsigned int panel, bool state)
	{
		typedef void(__thiscall *FN)(void *, int, bool);
		GetVFunc<FN>(this, 32)(this, panel, state);
	}

	void SetTopmostPopup(unsigned int panel, bool state)
	{
		typedef void(__thiscall *FN)(void *, int, bool);
		GetVFunc<FN>(this, 59)(this, panel, state);
	}
};

#define VGUI_PANEL_INTERFACE_VERSION "VGUI_Panel009"

class IImage
{
public:
	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint() = 0;

	// Set the position of the image
	virtual void SetPos(int x, int y) = 0;

	// Gets the size of the content
	virtual void GetContentSize(int &wide, int &tall) = 0;

	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int &wide, int &tall) = 0;

	// Sets the size of the image
	virtual void SetSize(int wide, int tall) = 0;

	// Set the draw color 
	virtual void SetColor(Color_t col) = 0;

	// virtual destructor
	virtual ~IImage() {}

	// not for general purpose use
	// evicts the underlying image from memory if refcounts permit, otherwise ignored
	// returns true if eviction occurred, otherwise false
	virtual bool Evict() = 0;

	virtual int GetNumFrames() = 0;
	virtual void SetFrame(int nFrame) = 0;
	virtual DWORD GetID() = 0;

	virtual void SetRotation(int iRotation) = 0;
};