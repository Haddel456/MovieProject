#pragma once
#ifndef GUIMAIN_H
#define GUIMAIN_H
#include <d3d11.h>

using drawcallback = void(void*);
int GuiMain(drawcallback drawfunction, void* obj_ptr);
//bool LoadTexture(const char* filename, int& width, int& height);
ID3D11ShaderResourceView* LoadTexture(const char* filename, int& width, int& height);


#endif // GUIMAIN_H