#include <engine.h>
#include <stdio.h>

#include <state.h>

#include <SDL.h>
//Screen dimension constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

int bg_r = 0x01;
int bg_g = 0x01;
int bg_b = 0x01;
engine *l_engine = NULL;


SDL_Texture* background_far = NULL;
SDL_Texture* background_close = NULL;

texture_resource::~texture_resource() {
	this->id = "";
	this->file = "";
	SDL_DestroyTexture(this->texture);
	this->texture = NULL;
}
texture_resource::texture_resource() {}
texture_resource::texture_resource(std::string id, std::string file) {
	this->id = id;
	this->file = file;
	this->texture = l_engine->load_image(file);
}
sub_texture::sub_texture() {}
sub_texture::sub_texture(std::string id, std::string texture_resource, int w, int h, int x, int y) {
	this->id = id;
	this->texture_resource = texture_resource;
	this->w = w;
	this->h = h;
	this->s_x = x;
	this->s_y = y;
}

SDL_Point getsize(SDL_Texture *texture) {
    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return size;
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
	
	l_engine->render_at(background_far, SCREEN_WIDTH/2 - l_engine->state_manager->entity_states[0]->x * 0.025, SCREEN_HEIGHT/2 - l_engine->state_manager->entity_states[0]->y * 0.025, 0, 0, 0);

	return 0;
}
int lua_register_texture(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	std::string file = std::string(lua_tolstring(lua_instance, 2, NULL));
	
	l_engine->register_texture(id, file);
	
	return 0;
}
int lua_render_direct(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	int x = lua_tonumber(lua_instance, 2);
	int y = lua_tonumber(lua_instance, 3);
	double sx = lua_tonumber(lua_instance, 4);
	double sy = lua_tonumber(lua_instance, 5);
	double r = lua_tonumber(lua_instance, 6);
	
	l_engine->render_direct(id, x, y, sx, sy, r);
	
	return 0;
}
int lua_render_direct_scale(lua_State *lua_instance) {
	std::string id = std::string(lua_tolstring(lua_instance, 1, NULL));
	int x = lua_tonumber(lua_instance, 2);
	int y = lua_tonumber(lua_instance, 3);
	int sx = lua_tonumber(lua_instance, 4);
	int sy = lua_tonumber(lua_instance, 5);
	
	
	l_engine->render_at_scale(l_engine->get_texture(id), x, y, sx, sy, false);
	
	return 0;
}

int lua_get_screen_dimensions(lua_State *lua_instance) {
	lua_pushnumber(lua_instance, SCREEN_WIDTH);
	lua_pushnumber(lua_instance, SCREEN_HEIGHT);
	
	return 2;
}

int lua_render_text(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	const char *text = lua_tolstring(lua_instance, 1, NULL);
	
	int x = lua_tonumber(lua_instance, 2);
	int y = lua_tonumber(lua_instance, 3);
	int r = lua_tonumber(lua_instance, 4);
	int g = lua_tonumber(lua_instance, 5);
	int b = lua_tonumber(lua_instance, 6);
	
	l_engine->render_text(text, x, y, r, g, b);
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}
int lua_register_entity(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	const char *file = lua_tolstring(lua_instance, 1, NULL);
	const char *id = lua_tolstring(lua_instance, 2, NULL);
	
	int x = lua_tonumber(lua_instance, 3);
	int y = lua_tonumber(lua_instance, 4);
	
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

SDL_Texture* engine::load_image(std::string path) {
	SDL_Texture* loaded_texture = NULL;
	SDL_Surface* optimized_surface = NULL;
	
	SDL_Surface* loaded_surface = IMG_Load(path.c_str());
	
	if (loaded_surface == NULL) {
		printf("IMG_Load(%s) produced SDL_Image error: %s\n", path.c_str(), IMG_GetError());
	} else {
		
		loaded_texture = SDL_CreateTextureFromSurface(this->renderer, loaded_surface);
		SDL_FreeSurface(loaded_surface);
	}
	return loaded_texture;
}

void engine::register_texture(std::string name, std::string filepath) {
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] != NULL) {
			if (this->textures[i]->id == name) {
				printf("Texture %s already loaded\n", name.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] == NULL) {
			this->textures[i] = new texture_resource(name, filepath);
			if (this->textures[i]->texture == NULL) {
				this->textures[i] = NULL;
				printf("Failed to create texture %s from %s\n", name.c_str(), filepath.c_str());
			} else {
				printf("Created texture %s from %s\n", name.c_str(), filepath.c_str());
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
void engine::register_sub_texture(std::string id, std::string texture_id, int sub_w, int sub_h, int sub_x, int sub_y) {
	bool texture_found = false;
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] != NULL) {
			if (this->sub_textures[i]->id == id) {
				printf("Sub Texture %s already created.\n", texture_id.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] != NULL) {
			if (this->textures[i]->id == texture_id) {
				texture_found = true;
				break;
			}
		}
	}
	
	if (not texture_found) {
		printf("Texture '%s' for sub texture '%s' does not exist.\n", texture_id.c_str(), id.c_str());
		return;
	}
	SDL_Point texture_size = getsize(this->get_texture(texture_id));
	
	if (sub_x + sub_w > texture_size.x or sub_y + sub_h > texture_size.y or sub_x + sub_w < 0 or sub_y + sub_h < 0) {
		printf("Sub texture '%s' is outside of texture '%s'\n", id.c_str(), texture_id.c_str());
		return;
	}
	
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] == NULL) {
			this->sub_textures[i] = new sub_texture(id, texture_id, sub_w, sub_h, sub_x, sub_y);
			printf("Created sub texture %s from %s\n", id.c_str(), texture_id.c_str());
			return;
		}
	}
}
SDL_Texture* engine::get_texture(std::string name) {
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] != NULL) {
			if (this->textures[i]->id == name) {
				return this->textures[i]->texture;
			}
		}
	}
	printf("Texture '%s' could not be found\n", name.c_str());
	return NULL;
}

TTF_Font* engine::load_font(std::string path, int size) {
	TTF_Font* new_font = TTF_OpenFont( path.c_str(), size );
    if( new_font == NULL )
    {
        printf( "TTF_OpenFont(%s) produced SDL_ttf error: %s\n", TTF_GetError() );
    }
    return new_font;
}

void engine::render_text(std::string text, int x, int y, int r, int g, int b) {
	SDL_Color text_color = {r,g,b};
	SDL_Surface* textSurface = TTF_RenderText_Solid( this->global_font, text.c_str(), text_color);
	SDL_Texture* loaded_texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
	SDL_Point size = getsize(loaded_texture);
	this->render_at(loaded_texture, x, y, size.x, size.y, 0);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(loaded_texture);
}

engine::engine() {
	printf("Engine Initializing\n");
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
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
		this->global_font = this->load_font("AVA.ttf", 32);
		this->load_logo = this->load_image("./logo.png");
		this->render_logo();
		printf("Engine Initialized\n");
	} else {
		printf("Engine Initialization Failure\n");
	}
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		this->textures[i] = NULL;
	}
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		this->sub_textures[i] = NULL;
	}
}

void engine::update_function(void) {
	fflush(stdout);
	this->state_manager->update(std::chrono::milliseconds(30));
}
void engine::render_function(void) {
	SDL_RenderClear(this->renderer);
	this->state_manager->render();
	SDL_RenderPresent(this->renderer);
}
void engine::render_at(SDL_Texture* surface, int x, int y, double sx, double sy, const double angle) {
	SDL_Rect target_rect;
	SDL_Point size = getsize(surface);
	if (sx != 0) {
		target_rect.w = sx;
		target_rect.x = x - (sx) / 2;
	} else {
		target_rect.w = size.x;
		target_rect.x = x - (size.x) /2;
	}
	if (sy != 0) {
		target_rect.h = sy;
		target_rect.y = y - (sy) / 2;
	} else {
		target_rect.h = size.y;
		target_rect.y = y - (size.y) /2;
	}
	SDL_RenderCopyEx(this->renderer, surface, NULL, &target_rect, angle, NULL, SDL_FLIP_NONE);
}
void engine::render_sub_texture(sub_texture *texture, int x, int y, double sx, double sy, const double angle) {
	SDL_Rect source_rect;
	source_rect.x = texture->s_x;
	source_rect.y = texture->s_y;
	source_rect.w = texture->w;
	source_rect.h = texture->h;
	SDL_Rect target_rect;
	SDL_Point size = getsize(this->get_texture(texture->texture_resource));
	if (sx != 0) {
		target_rect.w = sx;
		target_rect.x = x - (sx) / 2;
	} else {
		target_rect.w = size.x;
		target_rect.x = x - (size.x) /2;
	}
	if (sy != 0) {
		target_rect.h = sy;
		target_rect.y = y - (sy) / 2;
	} else {
		target_rect.h = size.y;
		target_rect.y = y - (size.y) /2;
	}
	SDL_RenderCopyEx(this->renderer, this->get_texture(texture->texture_resource), &source_rect, &target_rect, angle, NULL, SDL_FLIP_NONE);
}

void engine::render_direct(std::string id, int x, int y, double sx, double sy, const double angle) {
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (l_engine->sub_textures[i] != NULL) {
			if (l_engine->sub_textures[i]->id == id) {
				l_engine->render_sub_texture(l_engine->sub_textures[i], x, y, sx, sy, angle);
				return;
			}
		}
	}
	
	SDL_Texture *texture = NULL;
	texture = l_engine->get_texture(id);
	
	if (texture != NULL) {
		l_engine->render_at(texture, x, y, sx, sy, angle);
	}
}

void engine::render_fill(SDL_Texture* surface) {
	SDL_RenderCopy(this->renderer, surface, NULL, NULL);
}
void engine::render_at_scale(SDL_Texture* surface, int x, int y, double sx, double sy, bool divide) {
	SDL_Rect target_rect;
	SDL_Point size = getsize(surface);
	if (divide){
		target_rect.x = x - (size.x / sx) / 2;
		target_rect.y = y - (size.y / sy) / 2;
		target_rect.w = size.x / sx;
		target_rect.h = size.y / sy;
	} else {
		target_rect.x = x - (size.x * sx) / 2;
		target_rect.y = y - (size.y * sy) / 2;
		target_rect.w = size.x * sx;
		target_rect.h = size.y * sy;
	}
	SDL_RenderCopy(this->renderer, surface, NULL, &target_rect);
}
void engine::render_at_scale_rotate(SDL_Texture* surface, int x, int y, double sx, double sy, bool divide, const double angle) {
	SDL_Rect target_rect;
	SDL_Point size = getsize(surface);
	if (divide){
		target_rect.x = x - (size.x / sx) / 2;
		target_rect.y = y - (size.y / sy) / 2;
		target_rect.w = size.x / sx;
		target_rect.h = size.y / sy;
	} else {
		target_rect.x = x - (size.x * sx) / 2;
		target_rect.y = y - (size.y * sy) / 2;
		target_rect.w = size.x * sx;
		target_rect.h = size.y * sy;
	}
	SDL_RenderCopyEx(this->renderer, surface, NULL, &target_rect, angle, NULL, SDL_FLIP_NONE);
}
void engine::render_logo(void) {
	SDL_RenderClear(this->renderer);
	this->render_at(this->load_logo, SCREEN_WIDTH /2 , SCREEN_HEIGHT /2, 256, 256, 0);
	this->render_text("Loading Rabbits", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2+132, 0,255,0);
	SDL_RenderPresent(this->renderer);
}

void engine::engine_loop(void) {
	int i = 0;
	this->current_tick = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);
	this->display_at = this->current_tick + std::chrono::milliseconds(2000);
	this->sleeping_till = this->current_tick + std::chrono::milliseconds(2000);
	while (this->running) {
		this->current_tick = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()
		);
		if (this->current_tick >= this->sleeping_till) {
			this->update_function();
			this->sleeping_till = this->current_tick + std::chrono::milliseconds(60);
		} else {
	    }
		if (this->current_tick >= this->display_at) {
			this->render_function();
			this->display_at = this->current_tick + std::chrono::milliseconds(10);
		} else {
	    }
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
		lua_register(this->lua_instance, "render_direct_scale", lua_render_direct_scale);
		lua_register(this->lua_instance, "render_direct", lua_render_direct);
		lua_register(this->lua_instance, "register_texture", lua_register_texture);
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
		lua_register(this->lua_instance, "get_screen_dimensions", lua_get_screen_dimensions);
		lua_register(this->lua_instance, "set_far_background", lua_load_background_far);
		lua_register(this->lua_instance, "render_far_background", lua_render_background_far);
		lua_register(this->lua_instance, "register_entity", lua_register_entity);
		lua_register(this->lua_instance, "destroy_entity", lua_destroy_entity);
		lua_register(this->lua_instance, "render_entities", lua_render_entities);
		lua_register(this->lua_instance, "update_entities", lua_update_entities);
		
		//lua_register(this->lua_instance, "set_entity_velocity", lua_entity_set_velocity);
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
		
		this->state_manager = new state_machine(this);
		luaL_dofile(this->lua_instance, "pre_init.lua");
		this->running = true;
		this->engine_loop();
		
		lua_close(this->lua_instance);
		
		SDL_DestroyRenderer(this->renderer);
		SDL_DestroyWindow( this->window );
		this->renderer = NULL;
		this->window = NULL;
		IMG_Quit();
		SDL_Quit();
		printf("Engine Finished\n");
	}
}

void engine::stop(void) {
	this->running = false;
}