# Architecture
## Libraries
### Entity Component System (ECS)
To feature a performant, data-oriented design, Cascade uses an ECS. Specifically it includes the [entt](https://github.com/skypjack/entt) library. For more information on the Cascade-specific employment of entities, see [Entities](#entities) below.

### SDL/Image/TTF
Cascade uses [SDL](https://github.com/libsdl-org/SDL), [SDL Image](https://github.com/libsdl-org/SDL_image), and [SDL TTF](https://github.com/libsdl-org/SDL_ttf) for managing windows, rendering sprites, rendering text, and playing sounds.

## Entities
Entities are created using Cascade::Game::CreateEntity. Components can then be added to them with Cascade::Game::AddComponent.

## Systems
Cascade features "systems" which are a generalized way to do "things" within the framework. To write a system, you inherit from the base class Cascade::System and define the virtual functions Load(), Update(), and Cleanup(). You can then register the system for use using Cascade::Game::AddSystem().

**Load** is run when the system is registered.  
**Update** is run every frame.  
**Cleanup** is run when Cascade::Game::RemoveSystem is called.

## Scenes

**Scenes** also contain Load(), Update(), and Cleanup() functions. They are meant to manage loading of entities and updating based on user input.

There actually isn't much of a difference in these behind the scenes, they can pretty much be used interchangeably. I'm probably going to change that.