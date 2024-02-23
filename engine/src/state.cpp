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
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;


void State::render(void) {
	this->_internal_render();
}
void State::update(std::chrono::milliseconds time_delta) {
	this->_internal_update(time_delta);
}
void State::_internal_render(void) {
}
void State::_internal_update(std::chrono::milliseconds time_delta) {
}

void IdleState::_internal_render(void) {
	this->engine_target->render_at_scale(this->controls_b, SCREEN_WIDTH/2,SCREEN_HEIGHT/2, 3 ,4, false);
	this->engine_target->render_at_scale_rotate(this->logo, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 2, 2, true, this->logo_rotation);
	this->engine_target->render_text("Press", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-92, 0,255,0);
	this->engine_target->render_text("Void.GEN Initialized... Ready...", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-224, 0,255,0);
	this->engine_target->render_text("To Begin", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2+92, 0,255,0);
}
void IdleState::_internal_update(std::chrono::milliseconds time_delta) {
	SDL_Event e;
	while( SDL_PollEvent( &e ) ){
		if( e.type == SDL_QUIT ) this->engine_target->stop();
		else if (e.type == SDL_KEYDOWN) {
		}
	}
	const Uint8 *key_state = this->engine_target->state_manager->keyboard_state;
	if (key_state[SDL_SCANCODE_V] and key_state[SDL_SCANCODE_G]) {
			this->engine_target->state_manager->set_current_state("openSpaceState");
		
	}
	this->logo_rotation += 1;
}


IdleState::IdleState(engine *engine_target) {
	this->engine_target = engine_target;
	this->controls_b = engine_target->load_image("menu.png");
	this->logo = engine_target->load_image("logo.png");
}
void ControlsState::_internal_render(void) {
	this->engine_target->render_at_scale(this->controls_b, SCREEN_WIDTH/2,SCREEN_HEIGHT/2, 3 ,4, false);
	this->engine_target->render_text("CONTROLS", SCREEN_WIDTH /2, SCREEN_HEIGHT /2-224, 0,255,0);
	this->engine_target->render_text("F1 - View Controls", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-128, 0,255,0);
	this->engine_target->render_text("ESC - Pause", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-96, 0,255,0);
	this->engine_target->render_text("W - Forward", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-64, 0,255,0);
	this->engine_target->render_text("S - Reverse", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2-32, 0,255,0);
	this->engine_target->render_text("Q - Rotate Left", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2, 0,255,0);
	this->engine_target->render_text("E - Rotate Right", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2 + 32, 0,255,0);
	this->engine_target->render_text("X - Stop", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2 + 64, 0,255,0);
	this->engine_target->render_text("V - Return To (0,0)", SCREEN_WIDTH /2 , SCREEN_HEIGHT /2 + 96, 0,255,0);
}
void ControlsState::_internal_update(std::chrono::milliseconds time_delta) {
	SDL_Event e;
	while( SDL_PollEvent( &e ) ){
		if( e.type == SDL_QUIT ) this->engine_target->stop();
		else if (e.type == SDL_KEYDOWN) {
			this->engine_target->state_manager->set_current_state("openSpaceState");
		}
	}
}


ControlsState::ControlsState(engine *engine_target) {
	this->engine_target = engine_target;
	this->controls_b = engine_target->load_image("menu.png");
}
void OpenSpaceState::_internal_render(void) {
	this->engine_target->render_at(this->background_far, SCREEN_WIDTH/2 - this->entities[0]->x * 0.025, SCREEN_HEIGHT/2 - this->entities[0]->y * 0.025);
	this->engine_target->render_at(this->background_close, SCREEN_WIDTH/2 - this->entities[0]->x * 0.05, SCREEN_HEIGHT/2 - this->entities[0]->y * 0.05);
	
	for (int i = 1; i < 32; i++) {
		if (this->entities[i] == NULL) {
			
		} else {
			this->entities[i]->render();
		}
	}
	
	this->entities[0]->render();
	
	
	lua_getglobal(this->engine_target->lua_instance, "openspace_render");
	lua_call(this->engine_target->lua_instance, 0, 0);
}
void OpenSpaceState::_internal_update(std::chrono::milliseconds time_delta) {
	SDL_Event e;
	const Uint8 *key_state = this->engine_target->state_manager->keyboard_state;
	while( SDL_PollEvent( &e ) ){
		if( e.type == SDL_QUIT ) this->engine_target->stop();
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				this->engine_target->state_manager->set_current_state("idleState");
			}
			if (e.key.keysym.sym == SDLK_F1) {
				this->engine_target->state_manager->set_current_state("controlsState");
			}
			if (e.key.keysym.sym == SDLK_F10) {
				this->render_ui = !this->render_ui;
			}
		}
	}
	
	for (int i = 0; i < 32; i++) {
		if (this->entities[i] == NULL) {
			
		} else {
			this->entities[i]->update(time_delta);
		}
	}
	
	lua_getglobal(this->engine_target->lua_instance, "openspace_update");
	lua_call(this->engine_target->lua_instance, 0, 0);
}


OpenSpaceState::OpenSpaceState(engine *engine_target) {
	this->engine_target = engine_target;
	this->entities[0] = new PlayerClass(engine_target, this);
	for (int i = 1; i < 32; i++) {
		this->entities[i] = NULL;
	}
	this->entities[1] = new entity("pytheran_planet_1.png", 100, 100, engine_target, this);
	this->controls_a = engine_target->load_image("controls_b.png");
	this->controls_b = engine_target->load_image("controls_a.png");
	this->background_close = engine_target->load_image("close.png");
	this->background_far = engine_target->load_image("far.png");
	luaL_dofile(this->engine_target->lua_instance, "openspace_init.lua");
}
state_machine::state_machine(engine *engine_target) {
	this->engine_target = engine_target;
	this->openSpaceState = new OpenSpaceState(engine_target);
	this->idleState = new IdleState(engine_target);
	this->controlsState = new ControlsState(engine_target);
	this->statename = "idleState";
}

void state_machine::render(void) {
	if (this->statename == "openSpaceState") {
		this->openSpaceState->render();
	} else if (this->statename == "controlsState") {
		this->openSpaceState->render();
		this->controlsState->render();
	} else if (this->statename == "idleState") {
		this->openSpaceState->render();
		this->idleState->render();
	}
}

void state_machine::update(std::chrono::milliseconds time_delta) {
	this->keyboard_state = SDL_GetKeyboardState(NULL);
	if (this->statename == "openSpaceState") {
		this->openSpaceState->update(time_delta);
	} else if (this->statename == "controlsState") {
		this->controlsState->update(time_delta);
	} else if (this->statename == "idleState") {
		this->idleState->update(time_delta);
	}
}

void state_machine::set_current_state(std::string state) {
	this->statename = state;
	if (this->statename == "openSpaceState") {
		this->openSpaceState->render_ui = false;
	} else if (this->statename == "controlsState") {
		this->openSpaceState->render_ui = false;
	} else if (this->statename == "idleState") {
		this->openSpaceState->render_ui = false;
	}
}
