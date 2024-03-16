#include <engine.h>
#include <stdio.h>

#include <state.h>

#include <SDL.h>
//Screen dimension constants
int SCREEN_WIDTH = 400;
int SCREEN_HEIGHT = 400;

int bg_r = 0x01;
int bg_g = 0x01;
int bg_b = 0x01;
engine *l_engine = NULL;


SDL_Texture* background_far = NULL;
SDL_Texture* background_close = NULL;


sound_resource::~sound_resource() {
	this->id = "";
	this->file = "";
	Mix_FreeChunk(this->sound);
	this->sound = NULL;
}
sound_resource::sound_resource() {}
sound_resource::sound_resource(std::string id, std::string file) {
	this->id = id;
	this->file = file;
	this->sound = Mix_LoadWAV(file.c_str());
}

music_resource::~music_resource() {
	this->id = "";
	this->file = "";
	Mix_FreeMusic(this->sound);
	this->sound = NULL;
}
music_resource::music_resource() {}
music_resource::music_resource(std::string id, std::string file) {
	this->id = id;
	this->file = file;
	this->sound = Mix_LoadMUS(file.c_str());
}

int lua_load_background_far(lua_State *lua_instance) {
	const char *image_path = lua_tolstring(lua_instance, 1, NULL);
	SDL_Texture* loaded_image = l_engine->load_image(std::string(image_path));
	if (loaded_image != NULL) {
		if (background_far != NULL) {
			SDL_DestroyTexture(background_far);
		}
		background_far = loaded_image;
	}
	
	return 0;
}

int lua_render_background_far(lua_State *lua_instance) {
	
	l_engine->render_at(background_far, SCREEN_WIDTH/2 - l_engine->state_manager->entity_states[0]->x * 0.025, SCREEN_HEIGHT/2 - l_engine->state_manager->entity_states[0]->y * 0.025, 0, 0, true, 0);

	return 0;
}

int lua_register_sound(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	
	l_engine->register_sound(id, file);
	
	return 0;
}
int lua_play_sound(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	l_engine->play_sound(id);
	
	return 0;
}
int lua_play_player_sound(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	l_engine->play_player_sound(id);
	
	return 0;
}
int lua_is_player_channel_busy(lua_State *lua_instance) {
	lua_pushboolean(lua_instance, Mix_Playing(1));
	return 1;
}
int lua_stop_player_channel(lua_State *lua_instance) {
	Mix_HaltChannel(1);
	return 0;
}
int lua_stop_music(lua_State *lua_instance) {
	Mix_HaltMusic();
	return 0;
}

int lua_register_music(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	
	l_engine->register_music(id, file);
	
	return 0;
}
int lua_play_music(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	l_engine->play_music(id);
	
	return 0;
}

int lua_register_font(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	int size = lua_tonumber(lua_instance, 3);
	
	l_engine->register_font(id, file, size);
	
	return 0;
}

int lua_render_direct(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	int x = lua_tonumber(lua_instance, 2);
	int y = lua_tonumber(lua_instance, 3);
	double sx = lua_tonumber(lua_instance, 4);
	double sy = lua_tonumber(lua_instance, 5);
	bool centered = lua_toboolean(lua_instance, 6);
	double r = lua_tonumber(lua_instance, 7);
	
	l_engine->render_direct(id, x, y, sx, sy, centered, r);
	
	return 0;
}

int lua_get_screen_dimensions(lua_State *lua_instance) {
	lua_pushnumber(lua_instance, SCREEN_WIDTH);
	lua_pushnumber(lua_instance, SCREEN_HEIGHT);
	
	return 2;
}
int lua_set_screen_dimensions(lua_State *lua_instance) {
	int w = lua_tonumber(lua_instance, 1);
	int h = lua_tonumber(lua_instance, 2);
	int current_x = 0;
	int current_y = 0;
	
	SDL_GetWindowPosition(l_engine->window, &current_x, &current_y);
	SDL_SetWindowSize(l_engine->window, w, h);
	SDL_SetWindowPosition(l_engine->window, current_x+SCREEN_WIDTH/2-w/2, current_y+SCREEN_HEIGHT/2-h/2);
	SCREEN_HEIGHT = h;
	SCREEN_WIDTH = w;
	return 2;
}

int lua_render_text(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	const char *font = lua_tolstring(lua_instance, 1, NULL);
	const char *text = lua_tolstring(lua_instance, 2, NULL);
	
	int x = lua_tonumber(lua_instance, 3);
	int y = lua_tonumber(lua_instance, 4);
	int r = lua_tonumber(lua_instance, 5);
	int g = lua_tonumber(lua_instance, 6);
	int b = lua_tonumber(lua_instance, 7);
	bool centered = lua_toboolean(lua_instance, 8);
	
	l_engine->render_text(font, text, x, y, r, g, b, centered);
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_register_entity(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	const char *file = lua_tolstring(lua_instance, 1, NULL);
	const char *id = lua_tolstring(lua_instance, 2, NULL);
	
	double x = lua_tonumber(lua_instance, 3);
	double y = lua_tonumber(lua_instance, 4);
	
	l_engine->state_manager->register_entity(file, id, x, y);
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_destroy_entity(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));

	l_engine->state_manager->destroy_entity(id);
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_render_entities(lua_State *lua_instance) {
	l_engine->state_manager->render_entities();
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_update_entities(lua_State *lua_instance) {
	l_engine->state_manager->update_entities();
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_get_mouse_position(lua_State *lua_instance) {
	int x = 0;
	int y = 0;
	
	SDL_GetMouseState(&x, &y);
	
	lua_pushnumber(lua_instance,x);
	lua_pushnumber(lua_instance,y);
	
	return 2;
}
int lua_get_mouse_pressed(lua_State *lua_instance) {
	std::string button = std::string(lua_tolstring(lua_instance, 1, NULL));
	bool pressed = false;
	if (button == "left") {
		pressed = l_engine->state_manager->mouse_state[0];
	} else if (button == "right") {
		pressed = l_engine->state_manager->mouse_state[2];
	} else if (button == "middle") {
		pressed = l_engine->state_manager->mouse_state[1];
	} else {
		printf("Mouse Button '%s' not found.\n", button.c_str());
	}
	
	lua_pushboolean(lua_instance,pressed);
	
	return 1;
}
int lua_was_mouse_released(lua_State *lua_instance) {
	std::string button = std::string(lua_tolstring(lua_instance, 1, NULL));
	bool pressed = false;
	if (button == "left") {
		pressed = l_engine->state_manager->mouse_release[0];
	} else if (button == "right") {
		pressed = l_engine->state_manager->mouse_release[2];
	} else if (button == "middle") {
		pressed = l_engine->state_manager->mouse_release[1];
	} else {
		printf("Mouse Button '%s' not found.\n", button.c_str());
	}
	
	lua_pushboolean(lua_instance,pressed);
	
	return 1;
}
int lua_was_mouse_pressed(lua_State *lua_instance) {
	std::string button = std::string(lua_tolstring(lua_instance, 1, NULL));
	bool pressed = false;
	if (button == "left") {
		pressed = l_engine->state_manager->mouse_pressed[0];
	} else if (button == "right") {
		pressed = l_engine->state_manager->mouse_pressed[2];
	} else if (button == "middle") {
		pressed = l_engine->state_manager->mouse_pressed[1];
	} else {
		printf("Mouse Button '%s' not found.\n", button.c_str());
	}
	
	lua_pushboolean(lua_instance,pressed);
	
	return 1;
}
int lua_get_key_pressed(lua_State *lua_instance) {
	SDL_Keycode check;
	const char *key = lua_tolstring(lua_instance, 1, NULL);
	check = SDL_GetKeyFromName(key);
	lua_pushboolean(lua_instance, l_engine->state_manager->keys_pressed[SDL_GetScancodeFromKey(check)]);
	
	return 1;
}

int lua_get_key_down(lua_State *lua_instance) {
	SDL_Keycode check;
	const char *key = lua_tolstring(lua_instance, 1, NULL);
	check = SDL_GetKeyFromName(key);
	lua_pushboolean(lua_instance, l_engine->state_manager->keyboard_state[SDL_GetScancodeFromKey(check)]);
	
	return 1;
}
int lua_destroy_sub_texture(lua_State *lua_instance) {
	std::string id = lua_tostring(lua_instance, 1);
	l_engine->destroy_sub_texture(id);
	return 0;
}
int lua_destroy_texture(lua_State *lua_instance) {
	std::string id = lua_tostring(lua_instance, 1);
	l_engine->destroy_texture(id);
	return 0;
}
int lua_destroy_font(lua_State *lua_instance) {
	std::string id = lua_tostring(lua_instance, 1);
	l_engine->destroy_font(id);
	return 0;
}
int lua_destroy_sound(lua_State *lua_instance) {
	std::string id = lua_tostring(lua_instance, 1);
	l_engine->destroy_sound(id);
	return 0;
}
int lua_destroy_music(lua_State *lua_instance) {
	std::string id = lua_tostring(lua_instance, 1);
	l_engine->destroy_music(id);
	return 0;
}

int lua_entity_get_position(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		lua_pushnumber(lua_instance, target->x);
		lua_pushnumber(lua_instance, target->y);
	} else {
		lua_pushnumber(lua_instance, 0);
		lua_pushnumber(lua_instance, 0);
	}
	
	return 2;
}

int lua_player_get_velocity(lua_State *lua_instance) {
	lua_pushnumber(lua_instance, l_engine->state_manager->entity_states[0]->vx);
	lua_pushnumber(lua_instance, l_engine->state_manager->entity_states[0]->vy);
	
	return 2;
}
int lua_entity_set_velocity(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	double vx = lua_tonumber(lua_instance, 2);
	double vy = lua_tonumber(lua_instance, 3);
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->vx = vx;
		target->vy = vy;
	}
	
	return 0;
}

int lua_entity_get_velocity(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		lua_pushnumber(lua_instance, target->vx);
		lua_pushnumber(lua_instance, target->vy);
	} else {
		lua_pushnumber(lua_instance, 0);
		lua_pushnumber(lua_instance, 0);
	}
	
	return 2;
}

int lua_player_get_rotation(lua_State *lua_instance) {
	lua_pushnumber(lua_instance, l_engine->state_manager->entity_states[0]->rotation);

	return 1;
}

int lua_entity_get_rotation(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		lua_pushnumber(lua_instance, target->rotation);
	} else {
		lua_pushnumber(lua_instance, -1200);
	}
	
	return 1;
}

int lua_entity_get_velocity_angle(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		lua_pushnumber(lua_instance, target->get_velocity_angle());
	} else {
		lua_pushnumber(lua_instance, -1200);
	}
	
	return 1;
}
		
int lua_player_apply_forward_force(lua_State *lua_instance) {
	l_engine->state_manager->entity_states[0]->apply_forward_force(lua_tonumber(lua_instance, 1));
	return 0;
}	
int lua_set_sfx_volume(lua_State *lua_instance) {
	l_engine->set_sfx_volume(lua_tonumber(lua_instance, 1));
	return 0;
}	
int lua_set_music_volume(lua_State *lua_instance) {
	l_engine->set_music_volume(lua_tonumber(lua_instance, 1));
	return 0;
}
	
int lua_entity_apply_forward_force(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->apply_forward_force(lua_tonumber(lua_instance, 2));
	}
	return 0;
}

int lua_register_lua_gamestate(lua_State *lua_instance) {
	std::string file = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string name = std::string(lua_tolstring(lua_instance, 2, NULL));
	l_engine->state_manager->register_state(file, name);
	return 0;
}

int lua_set_gamestate(lua_State *lua_instance) {
	std::string name = std::string(lua_tolstring(lua_instance, 1, NULL));
	l_engine->state_manager->set_current_state(name);
	return 0;
}

int lua_player_set_velocity(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double vx = lua_tonumber(lua_instance, 1);
	double vy = lua_tonumber(lua_instance, 2);
	
	l_engine->state_manager->entity_states[0]->vx = vx;
	l_engine->state_manager->entity_states[0]->vy = vy;
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_player_set_position(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double x = lua_tonumber(lua_instance, 1);
	double y = lua_tonumber(lua_instance, 2);
	
	l_engine->state_manager->entity_states[0]->x = x;
	l_engine->state_manager->entity_states[0]->y = y;
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_entity_set_position(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	double x = lua_tonumber(lua_instance, 2);
	double y = lua_tonumber(lua_instance, 3);
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->x = x;
		target->y = y;
	}
	
	return 0;
}

int lua_player_set_rotation(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double rotation = lua_tonumber(lua_instance, 1);
	l_engine->state_manager->entity_states[0]->rotation = rotation ;
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}

int lua_entity_set_rotation(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	double rotation = lua_tonumber(lua_instance, 2);
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->rotation = rotation;
	}
	
	return 0;
}

int lua_entity_set_texture(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string texture = std::string(lua_tolstring(lua_instance, 2, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->set_texture(texture);
	}
	
	return 0;
}

int lua_entity_reset_texture(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->reset_texture();
	}
	
	return 0;
}

int lua_entity_set_direct_movement(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	bool enabled = lua_toboolean(lua_instance, 2);
	
	entity *target = l_engine->state_manager->get_entity(id);
	if (target != NULL) {
		target->direct_movement = enabled;
	}
	
	return 0;
}

int lua_player_render(lua_State *lua_instance) {
	l_engine->state_manager->entity_states[0]->render();
	return 0;
}

int lua_player_update(lua_State *lua_instance) {
	l_engine->state_manager->entity_states[0]->update(std::chrono::milliseconds(30));
	return 0;
}

int lua_register_texture(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	
	l_engine->register_texture(id, file);
	
	return 0;
}



void engine::destroy_sound(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->sounds[i] != NULL) {
			if (this->sounds[i]->id == name) {
				delete this->sounds[i];
				this->sounds[i] = NULL;
				printf("Destroyed sound '%s.'\n", name.c_str());
				return;
			}
		}
	}
	printf("Could not destroy sound '%s.'\n", name.c_str());
}

void engine::register_sound(std::string name, std::string filepath) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->sounds[i] != NULL) {
			if (this->sounds[i]->id == name) {
				printf("Sound '%s' already loaded\n", name.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->sounds[i] == NULL) {
			this->sounds[i] = new sound_resource(name, filepath);
			if (this->sounds[i]->sound == NULL) {
				this->sounds[i] = NULL;
				printf("Failed to create sound '%s' from %s\n", name.c_str(), filepath.c_str());
			} else {
				printf("Created sound '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			}
			return;
		}
	}
}

void engine::destroy_music(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->musics[i] != NULL) {
			if (this->musics[i]->id == name) {
				delete this->musics[i];
				this->musics[i] = NULL;
				printf("Destroyed music '%s.'\n", name.c_str());
				return;
			}
		}
	}
	printf("Could not destroy music '%s.'\n", name.c_str());
}

void engine::register_music(std::string name, std::string filepath) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->musics[i] != NULL) {
			if (this->musics[i]->id == name) {
				printf("Music '%s' already loaded\n", name.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->musics[i] == NULL) {
			this->musics[i] = new music_resource(name, filepath);
			if (this->musics[i]->sound == NULL) {
				this->musics[i] = NULL;
				printf("Failed to create music '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			} else {
				printf("Created music '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			}
			return;
		}
	}
}



int lua_register_sub_texture(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	int w = lua_tonumber(lua_instance, 3);
	int h = lua_tonumber(lua_instance, 4);
	int x = lua_tonumber(lua_instance, 5);
	int y = lua_tonumber(lua_instance, 6);
	l_engine->register_sub_texture(id, file,w,h,x,y);
	
	return 0;
}


void engine::play_sound(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->sounds[i] != NULL) {
			if (this->sounds[i]->id == name) {
				Mix_PlayChannel(-1, this->sounds[i]->sound, 0);
			}
		}
	}
}
void engine::play_player_sound(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->sounds[i] != NULL) {
			if (this->sounds[i]->id == name) {
				Mix_PlayChannel(1, this->sounds[i]->sound, 0);
			}
		}
	}
}


void engine::play_music(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		if (this->musics[i] != NULL) {
			if (this->musics[i]->id == name) {
				Mix_FadeInMusic(this->musics[i]->sound, 250, 700);
			}
		}
	}
}



void engine::set_sfx_volume(double volume) {
	this->sfx_volume = volume;
	Mix_Volume(-1,volume * (MIX_MAX_VOLUME));
}
void engine::set_music_volume(double volume) {
	this->music_volume = volume;
	Mix_VolumeMusic(volume * (MIX_MAX_VOLUME - 28));
}

engine::engine() {
	printf("Engine Initializing\n");
	
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL_INIT_VIDEO produced SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Create window
		this->window = SDL_CreateWindow( "void.GEN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( this->window == NULL )
		{
			printf( "SDL_CreateWindow produced SDL_Error: %s\n", SDL_GetError() );
		} else {
			this->initialized = true;
		}
	}
	if (this->initialized) {
		this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
		if (this->renderer == NULL) {
			this->initialized = false;
		} else {
			SDL_SetRenderDrawColor(this->renderer, 0,  0, 0, 0);
		}
	}
	if (this->initialized) {
		if( Mix_OpenAudio( 44100, AUDIO_F32SYS, 8, 1024 ) < 0 )
		{
			printf( "Mix_OpenAudio produced SDL_mixer Error: %s\n", Mix_GetError() );
			this->initialized = false;
		} else {
			if (Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG != MIX_INIT_OGG) {
				printf("OGG Support not found\n");
			}
			this->set_sfx_volume(this->sfx_volume);
			this->set_music_volume(this->music_volume);
		}
	}
	
	if (this->initialized) {
		if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
			printf("IMG_Init produced SDL_image Error: %s\n", IMG_GetError());
			this->initialized = false;
		}
	}
	if (this->initialized) {
		if( TTF_Init() == -1 )
		{
			printf( "TTF_Init produced SDL_image Error: %s\n", TTF_GetError() );
			this->initialized = false;
		} else {
		}
	}
	if (this->initialized) {
		this->load_logo = this->load_image("./logo.png");
		this->render_logo();
		printf("Engine Initialized\n");
	} else {
		printf("Engine Initialization Failure\n");
		return;
	}
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		this->textures[i] = NULL;
	}
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		this->sub_textures[i] = NULL;
	}
	for (int i = 0; i < ENGINE_MAX_FONTS; i++) {
		this->fonts[i] = NULL;
	}
	for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
		this->sounds[i] = NULL;
		this->musics[i] = NULL;
	}
}

void engine::engine_loop(void) {
	int i = 0;
	this->current_tick = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);
	this->display_at = this->current_tick + std::chrono::milliseconds(500);
	this->sleeping_till = this->current_tick + std::chrono::milliseconds(1000);
	while (this->running) {
		this->current_tick = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()
		);
		if (this->current_tick >= this->sleeping_till) {
			this->update_function();
			this->sleeping_till = this->current_tick + std::chrono::milliseconds(1000/30);
		} else {
	    }
		if (this->current_tick >= this->display_at) {
			this->render_function();
			this->display_at = this->current_tick + std::chrono::milliseconds(1000/60);
		} else {
	    }
		SDL_Delay(1);
	}
}

void engine::begin(void) {
	if (this->initialized) {
		printf("Engine Begin\n");
		l_engine = this;
		this->lua_instance = luaL_newstate();
		luaL_openlibs(this->lua_instance);
		
		lua_register(this->lua_instance, "register_state", lua_register_lua_gamestate);
		lua_register(this->lua_instance, "set_state", lua_set_gamestate);
		lua_register(this->lua_instance, "render_text", lua_render_text);
		lua_register(this->lua_instance, "render_player", lua_player_render);
		lua_register(this->lua_instance, "render_direct", lua_render_direct);
		lua_register(this->lua_instance, "register_font", lua_register_font);
		lua_register(this->lua_instance, "register_texture", lua_register_texture);
		lua_register(this->lua_instance, "register_sound", lua_register_sound);
		lua_register(this->lua_instance, "register_music", lua_register_music);
		lua_register(this->lua_instance, "play_music", lua_play_music);
		lua_register(this->lua_instance, "play_sound", lua_play_sound);
		lua_register(this->lua_instance, "play_player_sound", lua_play_player_sound);
		lua_register(this->lua_instance, "stop_player_channel", lua_stop_player_channel);
		lua_register(this->lua_instance, "is_player_channel_busy", lua_is_player_channel_busy);
		lua_register(this->lua_instance, "music_volume", lua_set_music_volume);
		lua_register(this->lua_instance, "sfx_volume", lua_set_sfx_volume);
		lua_register(this->lua_instance, "stop_music", lua_stop_music);
		lua_register(this->lua_instance, "register_sub_texture", lua_register_sub_texture);
		lua_register(this->lua_instance, "update_player", lua_player_update);
		lua_register(this->lua_instance, "set_player_velocity", lua_player_set_velocity);
		lua_register(this->lua_instance, "get_player_velocity", lua_player_get_velocity);
		lua_register(this->lua_instance, "set_player_position", lua_player_set_position);
		//lua_register(this->lua_instance, "get_player_position", lua_player_get_position);
		lua_register(this->lua_instance, "set_player_rotation", lua_player_set_rotation);
		lua_register(this->lua_instance, "get_player_rotation", lua_player_get_rotation);
		lua_register(this->lua_instance, "player_force_forward", lua_player_apply_forward_force);
		lua_register(this->lua_instance, "is_key_pressed", lua_get_key_pressed);
		lua_register(this->lua_instance, "is_key_down", lua_get_key_down);
		lua_register(this->lua_instance, "get_mouse_position", lua_get_mouse_position);
		lua_register(this->lua_instance, "is_mouse_pressed", lua_get_mouse_pressed);
		lua_register(this->lua_instance, "was_mouse_pressed", lua_was_mouse_pressed);
		lua_register(this->lua_instance, "was_mouse_released", lua_was_mouse_released);
		lua_register(this->lua_instance, "get_screen_dimensions", lua_get_screen_dimensions);
		lua_register(this->lua_instance, "set_screen_dimensions", lua_set_screen_dimensions);
		lua_register(this->lua_instance, "set_far_background", lua_load_background_far);
		lua_register(this->lua_instance, "render_far_background", lua_render_background_far);
		lua_register(this->lua_instance, "register_entity", lua_register_entity);
		lua_register(this->lua_instance, "destroy_entity", lua_destroy_entity);
		lua_register(this->lua_instance, "render_entities", lua_render_entities);
		lua_register(this->lua_instance, "update_entities", lua_update_entities);
		
		lua_register(this->lua_instance, "set_entity_velocity", lua_entity_set_velocity);
		lua_register(this->lua_instance, "get_entity_velocity_angle", lua_entity_get_velocity_angle);
		lua_register(this->lua_instance, "get_entity_velocity", lua_entity_get_velocity);
		lua_register(this->lua_instance, "set_entity_position", lua_entity_set_position);
		lua_register(this->lua_instance, "get_entity_position", lua_entity_get_position);
		lua_register(this->lua_instance, "set_entity_rotation", lua_entity_set_rotation);
		lua_register(this->lua_instance, "get_entity_rotation", lua_entity_get_rotation);
		lua_register(this->lua_instance, "set_entity_direct_movement", lua_entity_set_direct_movement);
		lua_register(this->lua_instance, "entity_force_forward", lua_entity_apply_forward_force);
		lua_register(this->lua_instance, "entity_set_texture", lua_entity_set_texture);
		lua_register(this->lua_instance, "entity_reset_texture", lua_entity_reset_texture);
		lua_register(this->lua_instance, "destroy_texture", lua_destroy_texture);
		lua_register(this->lua_instance, "destroy_sub_texture", lua_destroy_sub_texture);
		lua_register(this->lua_instance, "destroy_sound", lua_destroy_sound);
		lua_register(this->lua_instance, "destroy_music", lua_destroy_music);
		lua_register(this->lua_instance, "destroy_font", lua_destroy_font);
		
		this->state_manager = new state_machine(this);
		
		// TODO: Add protection here. This call seg-faults if pre_init.lua does not exist, or if lua interpreter fails.
		luaL_dofile(this->lua_instance, "pre_init.lua");
		this->render_logo();
		this->running = true;
		this->engine_loop();
		
		lua_close(this->lua_instance);
		
		for (int i = 0; i < ENGINE_MAX_ENTITIES; i++) {
			if (this->state_manager->entity_states[i] != NULL) {
				printf("Destroying entity '%s'.\n", this->state_manager->entity_states[i]->id.c_str());
				delete this->state_manager->entity_states[i];
				this->state_manager->entity_states[i] = NULL;
			}
		}
		
		for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
			if (this->textures[i] != NULL) {
				printf("Destroying texture '%s'.\n", this->textures[i]->id.c_str());
				delete this->textures[i];
				this->textures[i] = NULL;
			}
		}
		for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
			
		}
		for (int i = 0; i < ENGINE_MAX_FONTS; i++) {
			if (this->fonts[i] != NULL) {
				printf("Destroying font '%s'.\n", this->fonts[i]->id.c_str());
				delete this->fonts[i];
				this->fonts[i] = NULL;
			}
		}
		for (int i = 0; i < ENGINE_MAX_SOUNDS; i++) {
			if (this->sounds[i] != NULL) {
				printf("Destroying sound '%s'.\n", this->sounds[i]->id.c_str());
				delete this->sounds[i];
				this->sounds[i] = NULL;
			}
			if (this->musics[i] != NULL) {
				printf("Destroying music '%s'.\n", this->musics[i]->id.c_str());
				delete this->musics[i];
				this->musics[i] = NULL;
			}
		}
		
		SDL_DestroyRenderer(this->renderer);
		SDL_DestroyWindow( this->window );
		
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		printf("Engine Finished\n");
	}
}


void engine::update_function(void) {
	this->state_manager->update(std::chrono::milliseconds(30));
	fflush(stdout);
}

void engine::render_function(void) {
	SDL_RenderClear(this->renderer);
	this->state_manager->render();
	SDL_RenderPresent(this->renderer);
}

void engine::stop(void) {
	this->running = false;
}