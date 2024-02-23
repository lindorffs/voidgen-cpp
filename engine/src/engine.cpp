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
SDL_Point getsize(SDL_Texture *texture) {
    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return size;
}
int lua_render_text(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	const char *text = lua_tolstring(lua_instance, 1, NULL);
	
	int x = lua_tonumber(lua_instance, 2);
	int y = lua_tonumber(lua_instance, 3);
	int r = lua_tonumber(lua_instance, 4);
	int g = lua_tonumber(lua_instance, 5);
	int b = lua_tonumber(lua_instance, 5);
	
	l_engine->render_text(text, x, y, r, g, b);
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}
int lua_player_set_velocity(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double vx = lua_tonumber(lua_instance, 1);
	double vy = lua_tonumber(lua_instance, 2);
	
	l_engine->state_manager->openSpaceState->entities[0]->vx = vx;
	l_engine->state_manager->openSpaceState->entities[0]->vy = vy;
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}
int lua_player_set_position(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double x = lua_tonumber(lua_instance, 1);
	double y = lua_tonumber(lua_instance, 2);
	
	l_engine->state_manager->openSpaceState->entities[0]->x = x;
	l_engine->state_manager->openSpaceState->entities[0]->y = y;
	
	//this->render_text(text, x, y, r, g, b);
	return 0;
}
int lua_player_set_rotation(lua_State *lua_instance) {
	int args = lua_gettop(lua_instance);
	
	double rotation = lua_tonumber(lua_instance, 1);
	l_engine->state_manager->openSpaceState->entities[0]->rotation = rotation ;
	
	//this->render_text(text, x, y, r, g, b);
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
	this->render_at(loaded_texture, x, y);
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
void engine::render_at(SDL_Texture* surface, int x, int y) {
	SDL_Rect target_rect;
	SDL_Point size = getsize(surface);
	target_rect.x = x - size.x / 2;
	target_rect.y = y - size.y / 2;
	target_rect.w = size.x;
	target_rect.h = size.y;
	SDL_RenderCopy(this->renderer, surface, NULL, &target_rect);
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
	this->render_at(this->load_logo, SCREEN_WIDTH /2 , SCREEN_HEIGHT /2);
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
		
		lua_register(this->lua_instance, "render_text", lua_render_text);
		lua_register(this->lua_instance, "set_player_velocity", lua_player_set_velocity);
		lua_register(this->lua_instance, "set_player_position", lua_player_set_position);
		lua_register(this->lua_instance, "set_player_rotation", lua_player_set_rotation);
		
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
