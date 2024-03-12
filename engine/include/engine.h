#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <chrono>

#define ENGINE_MAX_ENTITIES 2048
#define ENGINE_MAX_TEXTURES 2048
#define ENGINE_MAX_FONTS 2048
#define ENGINE_MAX_SUB_TEXTURES 2048
#define ENGINE_MAX_SOUNDS 2048

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

class state;
class state_machine;

class sound_resource {
public:
		std::string id = "";
		std::string file = "";
		Mix_Chunk *sound = NULL;
	~sound_resource();
	sound_resource();
	sound_resource(std::string id, std::string file);
};
class music_resource {
public:
		std::string id = "";
		std::string file = "";
		Mix_Music *sound = NULL;
	~music_resource();
	music_resource();
	music_resource(std::string id, std::string file);
};

class texture_resource {
public:
	std::string id = "";
	std::string file = "";
	SDL_Texture* texture = NULL;
	~texture_resource();
	texture_resource();
	texture_resource(std::string id, std::string file);
};

class sub_texture {
public:
	std::string id = "";
	std::string texture_resource = "";
	int w;
	int h;
	int s_x;
	int s_y;
	SDL_Rect source_rect;
	sub_texture();
	~sub_texture();
	sub_texture(std::string id, std::string texture_resource, int w, int h, int x, int y);
};

class font {
	public:
	std::string id = "";
	std::string filename = "";
	TTF_Font* _font = NULL;
	~font();
	font();
	font(std::string id, std::string filename, int);
};

class engine {
	private:
		bool running = false;
		
		bool initialized = false;
		
		void engine_loop(void);
		void render_function(void);
		void render_logo(void);
		void update_function(void);
		
		std::chrono::milliseconds current_tick = std::chrono::milliseconds(0);
		std::chrono::milliseconds sleeping_till = std::chrono::milliseconds(0);
		std::chrono::milliseconds display_at = std::chrono::milliseconds(0);
		
		SDL_Texture* load_logo = NULL;
		texture_resource *textures[ENGINE_MAX_TEXTURES];
		font *fonts[ENGINE_MAX_FONTS];
		sound_resource *sounds[ENGINE_MAX_SOUNDS];
		music_resource *musics[ENGINE_MAX_SOUNDS];
	public:
		SDL_Window* window = NULL;
		sub_texture *sub_textures[ENGINE_MAX_SUB_TEXTURES];
		TTF_Font *global_font = NULL;
		SDL_Renderer* renderer = NULL;
		
		lua_State *lua_instance = NULL;
		state_machine *state_manager;
		
		double music_volume = 0.5;
		double sfx_volume = 0.7;
		
	engine();
	
	void begin(void);
	void stop(void);
	void render_at(SDL_Texture*, int x, int y, double sx, double sy, bool centered, const double angle);
	void render_sub_texture(sub_texture*, int x, int y, double sx, double sy, bool centered, const double angle);
	void render_direct(std::string texture_id, int x, int y, double sx, double sy, bool centered, const double angle);
	
	void destroy_texture(std::string name);
	void register_texture(std::string name, std::string filepath);
	void destroy_sound(std::string name);
	void register_sound(std::string name, std::string filepath);
	void play_sound(std::string name);
	void destroy_music(std::string name);
	void register_music(std::string name, std::string filepath);
	void play_music(std::string name);
	void play_player_sound(std::string name);
	void destroy_font(std::string name);
	void register_font(std::string name, std::string filepath, int size);
	void destroy_sub_texture(std::string id);
	void register_sub_texture(std::string id, std::string texture_id, int sub_w, int sub_h, int sub_x, int sub_y);
	SDL_Texture* get_texture(std::string id);
	TTF_Font* get_font(std::string);
	void render_text(std::string, std::string string, int x, int y, int r, int g, int b, bool centered);
	void set_music_volume(double volume);
	void set_sfx_volume(double volume);
	SDL_Texture* load_image(std::string path);
	TTF_Font *load_font(std::string path, int size);
};


#endif