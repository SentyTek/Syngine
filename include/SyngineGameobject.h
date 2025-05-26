#pragma once
#include <memory>
#include <string>
#include <map>

#include "Components.h"

using namespace std;

/*
    Syngine Gameobjects are the base class for all game objects used within a game.
    They are used to represent objects in the game world, such as players, enemies, and items.
    They are also used to represent objects that are not directly related to the game world, such as UI elements and menus.

    Gameobjects can then have various components attached to them, such as a mesh, transform, physics, AI, PlayerController, etc.
*/
class GameObject {
    long id; // Unique ID for the gameobject
    map<SynComponents, unique_ptr<SynComponent>> components; //Map of components attached to the gameobject
    public:
        string name; //Name of the gameoject
        string type; //Type of object, used for shader selection.
        GameObject(string name, string type);
        ~GameObject();
        long GetID(); //Get the ID of the gameobject
        int AddComponent(SynComponents type); //Add a component to the gameobject
        int RemoveComponent(SynComponents type); //Remove a component from the gameobject
        bool HasComponent(SynComponents type); //Check if the gameobject has a component

        template<typename T>
        T* GetComponent(); //Get a component of the gameobject
};

template<typename T>
T* GameObject::GetComponent() {
    auto it = this->components.find(T::componentType);
    if (it == this->components.end()) {
        return nullptr; // Component not found
    }
    
    return dynamic_cast<T*>(it->second.get());
}