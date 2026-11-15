# Special Systems
## Graphics
The Graphics system is baked into the Cascade library.

### Notes on Layers
Entities are drawn in *layers*. Higher layers are drawn first, meaning lower layers are drawn on **top** of them.  
The function `Cascade::Game::SortDrawingLayers()` should be called every time an entity is created or destroyed, as interacting with the registry results in an implicit re-shuffle of entity order.

## Inputs