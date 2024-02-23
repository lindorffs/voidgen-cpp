#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <chrono>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

class state;
class state_machine;

class engine {
	private:
		bool running = false;
		SDL_Window* window = NULL;
		
		bool initialized = false;
		
		void engine_loop(void);
		void render_function(void);
		void render_logo(void);
		void update_function(void);
		
		std::chrono::milliseconds current_tick = std::chrono::milliseconds(0);
		std::chrono::milliseconds sleeping_till = std::chrono::milliseconds(0);
		std::chrono::milliseconds display_at = std::chrono::milliseconds(0);
		
		SDL_Texture* load_logo = NULL;
	public:
		TTF_Font *global_font = NULL;
		SDL_Renderer* renderer = NULL;
		
		lua_State *lua_instance = NULL;
		state_machine *state_manager;
	engine();
	
	void begin(void);
	void stop(void);
	void render_fill(SDL_Texture* surface);
	void render_at(SDL_Texture*, int x, int y);
	void render_left(SDL_Texture*, int x, int y);
	void render_at_scale(SDL_Texture*, int x, int y, double sx, double sy, bool divide);
	void render_at_scale_rotate(SDL_Texture*, int x, int y, double sx, double sy, bool divide, const double angle);
	
	void render_text(std::string string, int x, int y, int r, int g, int b);
	
	SDL_Texture* load_image(std::string path);
	TTF_Font *load_font(std::string path, int size);
};


#endif
