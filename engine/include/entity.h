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
		SDL_Texture *entity_image = NULL;
		
		engine *engine_target = NULL;
		double x;
		double y;
		double vx;
		double vy;
		double rotation = 0.0;
		entity();
		entity(std::string fileName, int x, int y, engine *engine_target);
		void render(void);
		void update(std::chrono::milliseconds time_delta);
	
};

class ProjectileEntity : public entity{
	public:
		virtual void _internal_render(void);
		virtual void _internal_update(std::chrono::milliseconds time_delta);
		ProjectileEntity();
		ProjectileEntity(std::string fileName, int x, int y, engine *engine_target);
		void render(void);
		void update(std::chrono::milliseconds time_delta);
		int id;
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
