#pragma once
#include <vector>
#include "SyngineGraphics.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

namespace Syngine {
struct App {
    Graphics* graphics;
    SynModelLoader* synModels;
    std::vector<GameObject*> gameObjects;
    Phys* physicsManager;
};

class Core {
    public:
    App* app;

    Core();
    ~Core();
    int AttachGraphics(Graphics* graphics);
    int DetachGraphics();

    int SyngineEventLoop();

    GameObject* FindGameobjectByID(long id);
    GameObject* FindGameobjectByName(std::string name);
    int CreateGameobject(GameObject* gameobject);
    int DeleteGameobject(GameObject* gameobject);
};

} // namespace Syngine