#pragma once
#ifndef GUIMAIN_H
#define GUIMAIN_H


using drawcallback = void(void*);
int GuiMain(drawcallback drawfunction, void* obj_ptr);




#endif // GUIMAIN_H