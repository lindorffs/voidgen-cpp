#ifndef STATE_H
#define STATE_H

#include <SDL.h>
#include <entity.h>
#include <string>
#include <chrono>

#define MAX_AVAILABLE_STATES 15

class engine;
class entity;
class PlayerClass;
class ProjectileEntity;

class State {
	public:
		engine* engine_target = NULL;
		std::string state_prefix;
		State(engine* engine_target);
		State() {} ;
		void render(void);
		void unload(void);
		void load(void);
		void update(std::chrono::milliseconds time_delta);
	private:
		virtual void _internal_render(void);
		virtual void _internal_unload(void);
		virtual void _internal_load(void);
		virtual void _internal_update(std::chrono::milliseconds time_delta);
};

class LuaState : public State {
	public:
		std::string lua_source;
		LuaState(engine* engine_target, std::string, std::string);
		LuaState() {} ;
	private:
		void _internal_render(void);
		void _internal_unload(void);
		void _internal_load(void);
		void _internal_update(std::chrono::milliseconds time_delta);
};
class state_machine {
public:
    state_machine(engine* engine_target);
    ~state_machine();
	std::chrono::milliseconds last_time_delta;

    void render(void);
    void update(std::chrono::milliseconds time_delta);
    void update_entities(void);
    void render_entities(void);

	void set_current_state(std::string state_id);
	bool last_keys[256];
	bool keys_pressed[256];
	bool keys_released[256];
	const Uint8 *keyboard_state = NULL;
	std::string statename;
	entity *entity_states[ENGINE_MAX_ENTITIES];
	
	State *states[MAX_AVAILABLE_STATES];
	State *current_state= NULL;
	
	entity *get_entity(std::string id);
	
	void register_entity(std::string fileName, std::string id, double x, double y);
	void destroy_entity(std::string id);
	void register_state(std::string file, std::string name);
private:
	engine* engine_target = NULL;
};

#endif // STATE_H
