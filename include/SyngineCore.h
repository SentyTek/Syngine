#pragma once
#include <vector>
#include "SyngineGraphics.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"

struct SyngineApp {
    SyngineGraphics* graphics;
    SynModelLoader* synModels;
    std::vector<GameObject*> gameObjects;
};

class SyngineCore {
    public:
    SyngineApp* app;

    SyngineCore();
    ~SyngineCore();
    int AttachGraphics(SyngineGraphics* graphics);
    int DetachGraphics();

    int SyngineEventLoop();

    GameObject* FindGameobjectByID(long id);
    GameObject* FindGameobjectByName(std::string name);
    int CreateGameobject(GameObject* gameobject);
    int DeleteGameobject(GameObject* gameobject);
};