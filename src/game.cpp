#include <iostream>

#include <fstream>
#include <sstream>
#include <cmath>
#include <bitset>

#include "game.hpp"
#include "components.hpp"

void Cascade::Game::Run()
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  m_base_path = SDL_GetBasePath();

  // Initialize SLD_ttf
  TTF_Init();

  AddSystem<Graphics>("graphics");
  AddSystem<Audio>("audio");

  while (Continue())
  {
    StartFrame();
    EndFrame();
  }
}

void Cascade::Game::Quit()
{
  m_continue = false;
  DestroyAllEntities();
  SDL_Quit();
}

entt::entity Cascade::Game::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::Game::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}

void Cascade::Game::DestroyAllEntities()
{
  m_entt_registry.clear();
}

void Cascade::Game::LoadSpriteSheet(std::string sheet_name, std::string sheet_path)
{
  GetSystem<Graphics>("graphics")->LoadSpriteSheet(sheet_name, sheet_path);
}

std::vector<std::vector<int>> Cascade::Game::ReadTileFile(std::string tile_file)
{
  std::vector<std::vector<int>> tiles;

  std::ifstream file;
  file.open(tile_file);

  if (!file.is_open())
  {
    std::cerr << "Unable to open " << tile_file << "!\n";
    exit(1);
  }

  std::string line;
  std::string value_str;
  while (std::getline(file, line))
  {
    std::vector<int> tile_row;

    std::stringstream ss(line);
    while (ss.good())
    {
      // Get tile index
      std::getline(ss, value_str, ',');
      tile_row.push_back(std::stoi(value_str));
    }

    tiles.push_back(tile_row);
  }

  file.close();

  return tiles;
}

void Cascade::Game::LoadTileLayer(std::string tile_file, int tile_size, std::string sprite_sheet_name, int drawing_layer)
{
  std::vector<std::string> tile_layer_vect;
  tile_layer_vect.push_back(tile_file);

  LoadTileLayer(tile_layer_vect, tile_size, sprite_sheet_name, drawing_layer, 0);
}

void Cascade::Game::LoadTileLayer(std::vector<std::string> tile_frame_paths, int tile_size, std::string sprite_sheet_name, int drawing_layer, int update_interval)
{
  // Set up temporary render target (texture) to draw the tiles to
  // This is done for efficiency purposes. Instead of having each drawable tile as a seperate entity that must be
  // individually re-drawn every frame, we blit them all to a single texture that is drawn a single time every frame.
  SDL_Renderer *renderer = GetSystem<Graphics>("graphics")->GetRenderer();
  SDL_Window *window = GetSystem<Graphics>("graphics")->GetWindow();

  // Get the width and height of the sprite sheet in both pixels and tiles
  float source_sheet_width, source_sheet_height;
  GetSystem<Graphics>("graphics")->GetSpriteSheetSize(sprite_sheet_name, source_sheet_width, source_sheet_height);
  int sheet_width_in_tiles = source_sheet_width / tile_size;
  int sheet_height_in_tiles = source_sheet_height / tile_size;

  // Setup tile layer entity
  entt::entity tile_layer = CreateEntity();

  // Extract filename from first tile frame file and use it as the name for the animation
  std::string tile_layer_filename = Cascade::ExtractFilenameFromPath(tile_frame_paths[0]);

  // Set up animation
  std::string animation_name = tile_layer_filename;
  std::string animated_tiles_sheet_name = tile_layer_filename;

  CreateAnimation(animation_name, animated_tiles_sheet_name, update_interval);
  SetCurrentAnimation(tile_layer, animation_name, 0);
  SetLayer(tile_layer, drawing_layer);

  // Get source sprite sheet for drawing tiles
  SDL_Texture *source_sprite_sheet = GetSystem<Graphics>("graphics")->GetSpriteSheet(sprite_sheet_name);

  // Create the destination sprite sheet for the tile layer frames
  std::vector<std::vector<int>> tiles = ReadTileFile(tile_frame_paths[0]);
  SDL_Texture *destination_sprite_sheet = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tiles[0].size() * tile_size * tile_frame_paths.size(), tiles.size() * tile_size);

  for (int file_idx = 0; file_idx < tile_frame_paths.size(); file_idx++) // for each tile animation frame
  {
    std::vector<std::vector<int>> tiles = ReadTileFile(tile_frame_paths[file_idx]);

    SDL_Texture *tile_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tiles[0].size() * tile_size, tiles.size() * tile_size);
    SDL_SetRenderTarget(renderer, tile_texture);

    for (int row = 0; row < tiles.size(); row++)
    {
      for (int col = 0; col < tiles[row].size(); col++)
      {
        if (tiles[row][col] != -1)
        {
          // If we're on tile file 0, create an entity for this tile
          // This is done so we can set some data to be used later to interact with the tile if necessary
          // An example of this is colliders
          if (file_idx == 0)
          {
            entt::entity tile = CreateEntity();

            Cascade::State state;
            state.X = col * tile_size;
            state.Y = -row * tile_size;
            AddComponent(tile, state);

            // Set tile filename data for later lookup
            Cascade::TileData data;
            data.source_file = tile_frame_paths[file_idx];
            AddComponent(tile, data);
          }

          // Determine X and Y coordinates of tile on sprite sheet
          int tile_row = floor(tiles[row][col] / sheet_width_in_tiles);
          int tile_col = tiles[row][col] - sheet_width_in_tiles * tile_row;

          SDL_FRect frame;
          frame.x = (tile_col) * tile_size;
          frame.y = (tile_row) * tile_size;
          frame.h = tile_size;
          frame.w = tile_size;

          SDL_FRect destination;
          destination.x = col * tile_size;
          destination.y = row * tile_size;
          destination.w = tile_size;
          destination.h = tile_size;

          SDL_RenderTextureRotated(renderer, source_sprite_sheet, &frame, &destination, 0, NULL, SDL_FLIP_NONE);
        }
      }
    }

    // Add the tile layer to the animation sprite sheet
    SDL_FRect destination;
    destination.x = tiles[0].size() * tile_size * file_idx;
    destination.y = 0;
    destination.w = tiles[0].size() * tile_size;
    destination.h = tiles.size() * tile_size;

    SDL_SetRenderTarget(renderer, destination_sprite_sheet);
    SDL_RenderTextureRotated(renderer, tile_texture, NULL, &destination, 0, NULL, SDL_FLIP_NONE);

    // Add frame to animation
    AddFrame(animation_name, destination.x, destination.y, destination.w, destination.h);
  }

  GetSystem<Graphics>("graphics")->StoreSpriteSheet(animated_tiles_sheet_name, destination_sprite_sheet);

  float frame_sheet_width = tiles[0].size() * tile_size;
  float frame_sheet_height = tiles.size() * tile_size;
  
  State tile_layer_state;
  tile_layer_state.X = frame_sheet_width / 2 - tile_size / 2;
  tile_layer_state.Y = -frame_sheet_height / 2 + tile_size / 2;
  AddComponent(tile_layer, tile_layer_state);

  // Return rendering back to the window
  SDL_SetRenderTarget(renderer, NULL);
}

entt::entity Cascade::Game::FindTileFromFileAndState(std::string tile_file, Cascade::State state_in)
{
  // Find entity for this tile based on tile file and state
  auto view = GetRegistry().view<const State, const TileData>();

  for (auto [entity, state, tile_data] : view.each())
  {
    if (tile_data.source_file == tile_file && state.X == state_in.X && state.Y == state_in.Y)
    {
      return entity;
    }
  }

  std::cerr << "Cannot find tile from " << tile_file << "\n";
  exit(1);
}

void Cascade::Game::SetColliderTiles(std::string tile_file, int tile_size, std::vector<int> collider_tiles)
{
  std::vector<std::vector<int>> tiles = ReadTileFile(tile_file);

  // For each tile
  for (int row = 0; row < tiles.size(); row++)
  {
    for (int col = 0; col < tiles[row].size(); col++)
    {
      auto it = std::find(collider_tiles.begin(), collider_tiles.end(), tiles[row][col]);

      if (it != collider_tiles.end())
      {

        bool is_collider{false};

        // Check North
        if (row > 0 && !is_collider)
        {
          auto it = std::find(collider_tiles.begin(), collider_tiles.end(), tiles[row - 1][col]);

          if (it == collider_tiles.end())
          {
            is_collider = true;
          }
        }

        // Check South
        if (row < tiles.size() - 1 && !is_collider)
        {
          auto it = std::find(collider_tiles.begin(), collider_tiles.end(), tiles[row + 1][col]);

          if (it == collider_tiles.end())
          {
            is_collider = true;
          }
        }

        // Check East
        if (col < tiles[row].size() - 1 && !is_collider)
        {
          auto it = std::find(collider_tiles.begin(), collider_tiles.end(), tiles[row][col + 1]);

          if (it == collider_tiles.end())
          {
            is_collider = true;
          }
        }

        // Check West
        if (col > 0 && !is_collider)
        {
          auto it = std::find(collider_tiles.begin(), collider_tiles.end(), tiles[row][col - 1]);

          if (it == collider_tiles.end())
          {
            is_collider = true;
          }
        }

        if (is_collider)
        {
          Cascade::State collider_state;
          collider_state.X = col * tile_size;
          collider_state.Y = -row * tile_size;

          entt::entity entity = FindTileFromFileAndState(tile_file, collider_state);

          NonRotatingCollider collider;
          collider.static_collider = true;
          collider.X = collider_state.X;
          collider.Y = collider_state.Y;
          collider.width = tile_size;
          collider.height = tile_size;
          AddComponent<NonRotatingCollider>(entity, collider);
        }
      }
    }
  }
}

void Cascade::Game::CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval)
{
  GetSystem<Graphics>("graphics")->CreateAnimation(animation_name, sheet_name, update_interval);
}

void Cascade::Game::AddFrame(std::string animation_name, int x, int y, int w, int h)
{
  GetSystem<Graphics>("graphics")->AddFrame(animation_name, x, y, w, h);
}

void Cascade::Game::SetAnimationOffset(std::string animation_name, int dx, int dy)
{
  GetSystem<Graphics>("graphics")->SetAnimationOffset(animation_name, dx, dy);
}

void Cascade::Game::SetCurrentAnimation(entt::entity entity, std::string animation_name, int end_behavior)
{
  GetSystem<Graphics>("graphics")->SetCurrentAnimation(m_entt_registry, entity, animation_name, end_behavior);
}

std::string Cascade::Game::GetCurrentAnimation(entt::entity entity)
{
  return GetSystem<Graphics>("graphics")->GetCurrentAnimation(m_entt_registry, entity);
}

void Cascade::Game::SetColor(entt::entity entity, int color[3])
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  drawing_state.enable_tint = true;
  drawing_state.color[0] = color[0];
  drawing_state.color[1] = color[1];
  drawing_state.color[2] = color[2];
}

void Cascade::Game::FlipHorizontal(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  if (drawing_state.flip == SDL_FLIP_NONE)
    drawing_state.flip = SDL_FLIP_HORIZONTAL;
  if (drawing_state.flip == SDL_FLIP_VERTICAL)
    drawing_state.flip = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
}

void Cascade::Game::FlipVertical(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  if (drawing_state.flip == SDL_FLIP_NONE)
    drawing_state.flip = SDL_FLIP_VERTICAL;
  if (drawing_state.flip == SDL_FLIP_HORIZONTAL)
    drawing_state.flip = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
}

void Cascade::Game::ResetFlipHorizontal(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  if (drawing_state.flip == SDL_FLIP_HORIZONTAL)
    drawing_state.flip = SDL_FLIP_NONE;
  if (drawing_state.flip == SDL_FLIP_HORIZONTAL_AND_VERTICAL)
    drawing_state.flip = SDL_FLIP_VERTICAL;
}

void Cascade::Game::ResetFlipVertical(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  if (drawing_state.flip == SDL_FLIP_VERTICAL)
    drawing_state.flip = SDL_FLIP_NONE;
  if (drawing_state.flip == SDL_FLIP_HORIZONTAL_AND_VERTICAL)
    drawing_state.flip = SDL_FLIP_HORIZONTAL;
}

void Cascade::Game::ResetColor(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  drawing_state.enable_tint = false;
}

void Cascade::Game::SetHoverAnimation(entt::entity button, std::string animation_name)
{
  if (auto ui_element = m_entt_registry.try_get<UIElement>(button))
  {
    ui_element->hover_animation = animation_name;
    return;
  }

  std::cerr << "Cannot set hover animation, not a UI element!\n";
  exit(1);
}

void Cascade::Game::SetClickAnimation(entt::entity button, std::string animation_name)
{
  if (auto ui_element = m_entt_registry.try_get<UIElement>(button))
  {
    ui_element->click_animation = animation_name;
    return;
  }

  std::cerr << "Cannot set click animation, not a UI element!\n";
  exit(1);
}

void Cascade::Game::LoadSound(std::string sound_name, std::string sound_path)
{
  GetSystem<Audio>("audio")->LoadSound(sound_name, sound_path);
}

void Cascade::Game::PlaySound(std::string sound_name, bool loop_sound)
{
  GetSystem<Audio>("audio")->PlaySound(sound_name, loop_sound);
}

void Cascade::Game::SetFrequencyRatio(std::string sound_name, float ratio)
{
  GetSystem<Audio>("audio")->SetFrequencyRatio(sound_name, ratio);
}

bool Cascade::Game::IsSoundPlaying(std::string sound_name)
{
  return GetSystem<Audio>("audio")->IsSoundPlaying(sound_name);
}

void Cascade::Game::StopSound(std::string sound_name)
{
  GetSystem<Audio>("audio")->StopSound(sound_name);
}

void Cascade::Game::StopAllSounds()
{
  GetSystem<Audio>("audio")->StopAllSounds();
}

void Cascade::Game::LoadFont(std::string font_name, std::string font_path, float font_size)
{
  GetSystem<Graphics>("graphics")->LoadFont(font_name, font_path,font_size);
}

void Cascade::Game::WriteText(std::string text, std::string font_name, float position[2], Cascade::Color color, int layer)
{
  GetSystem<Graphics>("graphics")->WriteText(text, font_name, position, color, layer);
}

void Cascade::Game::SetCameraZoom(float zoom)
{
  GetSystem<Graphics>("graphics")->SetCameraZoom(zoom);
}

void Cascade::Game::SetCameraPosition(float position[2])
{
  GetSystem<Graphics>("graphics")->SetCameraPosition(position);
}

void Cascade::Game::UpdateCollider(entt::entity entity)
{
  // Make sure this entity has a collider
  if (auto collider = m_entt_registry.try_get<NonRotatingCollider>(entity))
  {
    // Make sure this entity has a state
    if (auto state = m_entt_registry.try_get<State>(entity))
    {
      collider->X = state->X;
      collider->Y = state->Y;
    } else {
      std::cerr << "Requested entity does not have a state!\n";
      exit(1);
    }
  } else {
    std::cerr << "Requested entity does not have a collider!\n";
    exit(1);
  }
}

std::bitset<4> Cascade::Game::GetAABBCollisions(entt::entity entity_1)
{  
  std::bitset<4> return_value{0000};
  // Make sure this entity has a collider
  if (auto collider_1 = m_entt_registry.try_get<NonRotatingCollider>(entity_1))
  {
    UpdateCollider(entity_1);

    auto view = m_entt_registry.view<NonRotatingCollider>();

    for (auto [entity_2, collider_2] : view.each())
    {
      if (entity_1 != entity_2)
      {
        if (collider_1->X - collider_1->width / 2  < (collider_2.X + collider_2.width / 2) &&   // C1 left wall is to the left of C2 right wall
            collider_1->X + collider_1->width / 2  > (collider_2.X - collider_2.width / 2) &&   // C1 right wall is to the right of C2 left wall
            collider_1->Y + collider_1->height / 2 > (collider_2.Y - collider_2.height / 2) && // C1 bottom wall is below C2 top wall
            collider_1->Y - collider_1->height / 2 < (collider_2.Y + collider_2.height / 2))   // C1 top wall is above C2 bottom wall
        {
          // Collision detected
          
          // Determine X and Y overlap
          float x_overlap = (collider_1->width / 2 + collider_2.width / 2) - std::abs(collider_1->X - collider_2.X);
          float y_overlap = (collider_1->height / 2 + collider_2.height / 2) - std::abs(collider_1->Y - collider_2.Y);

          State& state_1 = m_entt_registry.get<State>(entity_1);

          if (x_overlap < y_overlap)
          {
            // Horizontal Collision
            if (!return_value[0] && collider_1->X > collider_2.X) // West Collision
            {

              return_value[0] = true;
              state_1.X = collider_2.X + collider_2.width / 2 + (collider_1->width / 2);

            } else if (!return_value[1]) { // East Collision

              return_value[1] = true;
              state_1.X = collider_2.X - collider_2.width / 2 - (collider_1->width / 2);

            }
          } else {
            // Vertical Collision
            if (!return_value[2] && collider_1->Y < collider_2.Y) // South Collision
            {

              return_value[2] = true;
              state_1.Y = collider_2.Y - collider_2.height / 2 - (collider_1->height / 2);
              
            } else if (!return_value[3]) { // North Collision

              return_value[3] = true;
              state_1.Y = collider_2.Y + collider_2.height / 2 + (collider_1->height / 2);
              
            }
          }
          UpdateCollider(entity_1);
        }
      }
    }
  } else {
    std::cerr << "Requested entity does not have a collider!\n";
    exit(1);
  }

  return return_value;
}

void Cascade::Game::StartFrame()
{
  // Load scenes if needed
  if (m_scene_loading_needed)
  {
    for (const auto &pair : m_active_scenes)
    {
      // if scene not loaded, load it
      if (!pair.second)
      {
        GetScene<Scene>(pair.first)->Load();
      }
    }

    m_scene_loading_needed = false;
  }

  m_frame_start_ticks = SDL_GetTicks();
  SDL_RenderClear(GetSystem<Graphics>("graphics")->GetRenderer());

  m_inputs.StartFrame(m_entt_registry, GetScreenWidth(), GetScreenHeight());
  UpdateInputEvents();
}

void Cascade::Game::EndFrame()
{
  if (!m_continue)
  {
    return;
  }

  // Update all active scenes
  for (const auto &pair : m_active_scenes)
  {
    GetScene<Scene>(pair.first)->Update();
  }

  // Update all systems
  for (const auto& pair : m_systems)
  {
    pair.second->Update();
  }

  // End scenes
  if (m_scene_ending_needed)
  {
    for (const auto &pair : m_active_scenes)
    {
      if (m_scenes[pair.first]->m_end_scene)
      {
        RemoveActiveScene(pair.first);
      }
    }

    m_scene_ending_needed = false;
  }

  m_frame_end_ticks = SDL_GetTicks();
  EnforceFPS();
}

void Cascade::Game::EnforceFPS()
{
  float elapsed_time = (m_frame_end_ticks - m_frame_start_ticks) / 1000.f;

  if (elapsed_time < (1.0 / m_fps))
  {
    SDL_Delay((1.0 / m_fps) - elapsed_time);
    elapsed_time = 1.0 / m_fps;
  }
}

void Cascade::Game::UpdateUIAnimations()
{
  GetSystem<Graphics>("graphics")->UpdateUIAnimations(m_entt_registry);
}

void Cascade::Game::UpdateInputEvents()
{
  if (SDL_PollEvent(&m_event))
  {
    if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP)
    {
      m_inputs.HandleKeyboardEvent(m_event);
    }
    else if (m_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || m_event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
      m_inputs.HandleMouseEvent(m_event, m_entt_registry, GetScreenWidth(), GetScreenHeight());
    }
  }
}

bool Cascade::Game::WasPressed(entt::entity entity, int mouse_button)
{
  UIElement& ui_element = m_entt_registry.get<UIElement>(entity);
  return ui_element.click_type[mouse_button];
}

void Cascade::Game::AddActiveScene(std::string scene_name)
{
  // Enforce scene can only be added once
  if (m_active_scenes.find(scene_name) != m_active_scenes.end())
  {
    std::cerr << "Cannot add same scene to active scenes more than once!\n";
    exit(1);
  }

  // Add scene
  m_active_scenes[scene_name] = false;

  // Tell game we have a new scene to load
  m_scene_loading_needed = true;
}

void Cascade::Game::RemoveActiveScene(std::string scene_name)
{
  GetScene<Scene>(scene_name)->Cleanup();
  GetScene<Scene>(scene_name)->m_end_scene = false;
  m_active_scenes.erase(scene_name);
}