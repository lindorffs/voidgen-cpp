#include <state.h>

#include <engine.h>
#include <entity.h>
#include <cmath>


extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#define PI 3.1

SDL_Surface* player_image = NULL;
int player_x = 0;
int player_y = 0;

//Screen dimension constants
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;


void State::render(void) {
	this->_internal_render();
}
void State::update(std::chrono::milliseconds time_delta) {
	this->_internal_update(time_delta);
}
void State::_internal_render(void) {
}
void State::_internal_unload(void) {
}
void State::_internal_load(void) {
}
void State::unload(void) {
	this->_internal_unload();
}
void State::load(void) {
	this->_internal_load();
}
void State::_internal_update(std::chrono::milliseconds time_delta) {
}
LuaState::LuaState(engine *engine_target, std::string lua_source, std::string state_prefix) {
	this->engine_target = engine_target;
	this->lua_source = lua_source;
	this->state_prefix = state_prefix;
	luaL_dofile(this->engine_target->lua_instance,lua_source.c_str());
}
void LuaState::_internal_unload(void) {
	lua_getglobal(this->engine_target->lua_instance, (state_prefix+std::string("_unload")).c_str());
	lua_call(this->engine_target->lua_instance, 0, 0);
	
}
void LuaState::_internal_load(void) {
	lua_getglobal(this->engine_target->lua_instance, (state_prefix+std::string("_load")).c_str());
	lua_call(this->engine_target->lua_instance, 0, 0);
	
}
void LuaState::_internal_render(void) {
	lua_getglobal(this->engine_target->lua_instance, (state_prefix+std::string("_render")).c_str());
	lua_call(this->engine_target->lua_instance, 0, 0);
}
void LuaState::_internal_update(std::chrono::milliseconds time_delta) {
	lua_getglobal(this->engine_target->lua_instance, (state_prefix+std::string("_update")).c_str());
	lua_call(this->engine_target->lua_instance, 0, 0);
}


state_machine::state_machine(engine *engine_target) {
	this->current_state = NULL;
	this->keyboard_state = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < 256; i++) {
		this->last_keys[i] = false;
	}
	for (int i = 0; i < 256; i++) {
		this->keys_pressed[i] = false;
	}
	this->engine_target = engine_target;
	for (int i = 0; i < MAX_AVAILABLE_STATES; i++) {
		this->states[i] = NULL;
	}
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		this->entity_states[i] = NULL;
	}
	this->entity_states[0] = new PlayerClass(engine_target);
	this->mouse_state[0] = false;
	this->mouse_state[1] = false;
	this->mouse_state[2] = false;
	this->mouse_pressed[0] = false;
	this->mouse_pressed[1] = false;
	this->mouse_pressed[2] = false;
	this->mouse_release[0] = false;
	this->mouse_release[1] = false;
	this->mouse_release[2] = false;
	this->last_mouse_press[0] = false;
	this->last_mouse_press[1] = false;
	this->last_mouse_press[2] = false;
}

void state_machine::register_entity(std::string fileName, std::string id, double x, double y) {
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
			if (this->entity_states[i]->id == id) {
				printf("Entity '%s' already exists.\n", id.c_str());
				return;
			}
		}
	}
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] == NULL) {
			this->entity_states[i] = new entity(fileName, id, x, y, this->engine_target);
				printf("Created entity '%s' '%s'.\n", this->entity_states[i]->id.c_str(), this->entity_states[i]->base_texture_id.c_str());
			return;
		}
	}
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
				printf("Existing entity '%s'.\n", this->entity_states[i]->id.c_str());
			return;
		}
	}
}
void state_machine::destroy_entity(std::string id) {
	for (int i = 0; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
			if (this->entity_states[i]->id == id) {
				printf("Destroyed entity '%s'.\n", id.c_str());
				delete this->entity_states[i];
				this->entity_states[i] = NULL;
				return;
			}
		}
	}
}
entity *state_machine::get_entity(std::string id) {
	for (int i = 0; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
			if (this->entity_states[i]->id == id) {
				return this->entity_states[i];
			}
		}
	}
	return NULL;
}
void state_machine::update_entities(void) {
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
			this->entity_states[i]->update(this->last_time_delta);
		}
	}
}
void state_machine::render_entities(void) {
	for (int i = 1; i < ENGINE_MAX_ENTITIES; i++) {
		if (this->entity_states[i] != NULL) {
			this->entity_states[i]->render();
		}
	}
}

void state_machine::update(std::chrono::milliseconds time_delta) {
	SDL_Event e;
	while( SDL_PollEvent( &e ) ){
		if( e.type == SDL_QUIT ) this->engine_target->stop();
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				this->set_current_state(this->states[0]->state_prefix);
			}
		}
	}
	this->last_time_delta = time_delta;
	this->keyboard_state = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < 256; i++) {
		this->keys_pressed[i] = false;
		this->keys_released[i] = false;
		if (this->keyboard_state[i] != this->last_keys[i]) {
			if (this->last_keys[i]) {
				this->keys_released[i] = true;
			} else {
				this->keys_pressed[i] = true;
			}
		}
		this->last_keys[i] = this->keyboard_state[i];
	}
	
	Uint32 button_press = SDL_GetMouseState(NULL, NULL);
	if (button_press & SDL_BUTTON(1)) {
		this->mouse_state[0] = true;
	} else {
		this->mouse_state[0] = false;
	}
	if (button_press & SDL_BUTTON(3)) {
		this->mouse_state[2] = true;
	} else {
		this->mouse_state[2] = false;
	}
	if (button_press & SDL_BUTTON(2)) {
		this->mouse_state[1] = true;
	} else {
		this->mouse_state[1] = false;
	}

	for (int i = 0; i <= 2; i++) {
		this->mouse_release[i] = false;
		this->mouse_pressed[i] = false;
		if (this->mouse_state[i] != this->last_mouse_press[i]) {
			if (this->last_mouse_press[i]) {
				this->mouse_release[i] = true;
			} else {
				this->mouse_pressed[i] = true;
			}
			this->last_mouse_press[i] = this->mouse_state[i];
		}
	}
	this->current_state->update(time_delta);
}

void state_machine::render(void) {
	this->current_state->render();
}

void state_machine::set_current_state(std::string id) {
	for (int i = 0; i < MAX_AVAILABLE_STATES; i++) {
		if (this->states[i] != NULL) {
			if (this->states[i]->state_prefix == id) {
				if (this->current_state != NULL) {
					this->current_state->unload();
				}
				this->current_state = this->states[i];
				this->current_state->load();
				return;
			}
		}
	}
}

void state_machine::register_state(std::string file, std::string name) {
	for (int i = 0; i < MAX_AVAILABLE_STATES; i++) {
		if (this->states[i] != NULL) {
			if (this->states[i]->state_prefix == name) {
				return;
			}
		}
	}
	for (int i = 0; i < MAX_AVAILABLE_STATES; i++) {
		if (this->states[i] == NULL) {
			this->states[i] = new LuaState(this->engine_target, file, name);
			return;
		}
	}
}