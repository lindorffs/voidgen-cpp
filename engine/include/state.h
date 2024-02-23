#ifndef STATE_H
#define STATE_H

#include <SDL.h>
#include <entity.h>
#include <string>
#include <chrono>

class engine;
class entity;
class PlayerClass;
class ProjectileEntity;

class State {
	public:
		State(engine* engine_target);
		State() {} ;
		void render(void);
		void update(std::chrono::milliseconds time_delta);
	private:
		virtual void _internal_render(void);
		virtual void _internal_update(std::chrono::milliseconds time_delta);
};

class IdleState : public State {
	public:
		IdleState(engine* engine_target);
		IdleState();
		SDL_Texture *controls_b = NULL;
		SDL_Texture *planet = NULL;
		SDL_Texture *player = NULL;
		double logo_rotation = 0;
	private:
		engine* engine_target = NULL;
		SDL_Texture *logo = NULL;
		void _internal_render(void);
		void _internal_update(std::chrono::milliseconds time_delta);
		
};
class ControlsState : public State {
	public:
		ControlsState(engine* engine_target);
		ControlsState();
		SDL_Texture *controls_b = NULL;
	private:
		engine* engine_target = NULL;
		void _internal_render(void);
		void _internal_update(std::chrono::milliseconds time_delta);
		
};

class OpenSpaceState : public State {
	public:
		OpenSpaceState(engine* engine_target);
		OpenSpaceState();
		SDL_Texture *background_far = NULL;
		SDL_Texture *background_close = NULL;
		SDL_Texture *controls_a = NULL;
		SDL_Texture *controls_b = NULL;
		bool render_ui = false;
	private:
		engine* engine_target = NULL;

		void _internal_render(void);
		void _internal_update(std::chrono::milliseconds time_delta);
		
		bool player_moving_up;
		bool player_moving_down;
		bool player_moving_left;
		bool player_moving_right;
};

class state_machine {
public:
    state_machine(engine* engine_target);
    ~state_machine();

    void render(void);
    void update(std::chrono::milliseconds time_delta);

	void set_current_state(std::string statename);
	const Uint8 *keyboard_state = NULL;
	std::string statename;
	entity *entity_states[ENGINE_MAX_ENTITIES];
    OpenSpaceState* openSpaceState = NULL;
	IdleState* idleState = NULL;
	ControlsState* controlsState = NULL;
private:
	engine* engine_target = NULL;
};

#endif // STATE_H
