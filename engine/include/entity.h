#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <engine.h>
#include <chrono>
#include <state.h>
#include <string.h>

class OpenSpaceState;

class entity {
	public:
		virtual void _internal_render(void);
		virtual void _internal_update(std::chrono::milliseconds time_delta);
		
		engine *engine_target = NULL;
		double x = 0.0;
		double y = 0.0;
		double vx = 0.0;
		double vy = 0.0;
		double rotation = 0.0;
		entity();
		~entity();
		entity(std::string texture_id, std::string id, int x, int y, engine *engine_target);
		void render(void);
		void update(std::chrono::milliseconds time_delta);
		void apply_forward_force(double);
		
		double get_velocity_angle(void);
		std::string id;
		std::string texture_id, base_texture_id;
		
		void set_texture(std::string id);
		void reset_texture(void);
		
		bool direct_movement = false;
	
};

class ProjectileEntity : public entity{
	public:
		virtual void _internal_update(std::chrono::milliseconds time_delta);
		ProjectileEntity();
		ProjectileEntity(std::string fileName, std::string id, int x, int y, engine *engine_target);
		void render(void);
		void update(std::chrono::milliseconds time_delta);
};

class PlayerClass : public entity {
	public:
		void _internal_render(void);
		void _internal_update(std::chrono::milliseconds time_delta);
		PlayerClass(engine *engine_target);
		PlayerClass() {};
		void fire(void);
};

#endif