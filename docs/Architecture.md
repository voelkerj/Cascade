# Architecture
## Libraries
### Entity Component System (ECS)
To feature a performant, data-oriented design, Cascade uses an ECS. Specifically it includes the [entt](https://github.com/skypjack/entt) library. For more information on the Cascade-specific employment of entities, see [Entities](#entities) below.

### SDL/Image/TTF
Cascade uses [SDL](https://github.com/libsdl-org/SDL), [SDL Image](https://github.com/libsdl-org/SDL_image), and [SDL TTF](https://github.com/libsdl-org/SDL_ttf) for managing windows, rendering sprites, rendering text, and playing sounds.

## Entities
Entities are created using Cascade::Game::CreateEntity. Components can then be added to them with Cascade::Game::AddComponent.

## Scenes
Scenes manage the loading and deletion of entities from the application. Multiple scenes can be active at once, each owning their own subset of active entities. To write a scene, you inherit from the base class Cascade::Scene and define the virtual functions Load() and Cleanup(). You can then activate the scene using Cascade::Game::AddScene().

Scenes also contain an EndScene() function that informs the application the scene is ending and automatically invokes the Cleanup() function.

There is also a way to create references to specific entities that works between function scopes using TrackEntity() and Get Entity().

**Load** is run when Cascade::Game::AddScene() is called.  
**Cleanup** is run when Cascade::Scene::EndScene() is called.

## Systems
Systems are a generalized way to do "things" within the framework (manipulate entities, manage inputs, play sounds, etc.). To write a system, you inherit from the base class Cascade::System and define the virtual functions Load(), Update(), and Cleanup(). You can then activate the system using Cascade::Game::AddSystem().

**Load** is run when Cascade::Game::AddSystem() is called.  
**Update** is run every frame.  
**Cleanup** is run when Cascade::Game::RemoveSystem is called.