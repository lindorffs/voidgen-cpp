#include <engine.h>
#include <SDL.h>
#include <stdio.h>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern engine *l_engine;

font::font(std::string id, std::string filename, int size) {
	this->id = id;
	this->filename = filename;
	this->_font = l_engine->load_font(filename, size);
}
font::~font() {
	TTF_CloseFont(this->_font);
	this->id = "";
	this->filename = "";
	this->_font = NULL;
}

texture_resource::~texture_resource() {
	SDL_DestroyTexture(this->texture);
}
texture_resource::texture_resource() {}
texture_resource::texture_resource(std::string id, std::string file) {
	this->id = id;
	this->file = file;
	this->texture = l_engine->load_image(file);
}

sub_texture::~sub_texture() {
	this->id = "";
	this->texture_resource = "";
	this->w = 0;
	this->h = 0;
	this->s_x = 0;
	this->s_y = 0;
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

SDL_Texture* engine::load_image(std::string path) {
	SDL_Texture* loaded_texture = NULL;
	SDL_Surface* optimized_surface = NULL;
	
	SDL_Surface* loaded_surface = IMG_Load(path.c_str());
	
	if (loaded_surface == NULL) {
		printf("IMG_Load('%s') produced SDL_Image error: '%s.'\n", path.c_str(), IMG_GetError());
	} else {
		
		loaded_texture = SDL_CreateTextureFromSurface(this->renderer, loaded_surface);
		SDL_FreeSurface(loaded_surface);
	}
	return loaded_texture;
}


void engine::destroy_texture(std::string name) {
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] != NULL) {
			if (this->textures[i]->id == name) {
				delete this->textures[i];
				this->textures[i] = NULL;
				printf("Destroyed Texture '%s.'\n", name.c_str());
				return;
			}
		}
	}
	printf("Could not destroy Texture '%s.'\n", name.c_str());
}
void engine::register_texture(std::string name, std::string filepath) {
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] != NULL) {
			if (this->textures[i]->id == name) {
				printf("Texture '%s' already loaded.\n", name.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->textures[i] == NULL) {
			this->textures[i] = new texture_resource(name, filepath);
			if (this->textures[i]->texture == NULL) {
				delete this->textures[i];
				this->textures[i] = NULL;
				printf("Failed to create texture '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			} else {
				printf("Created texture '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			}
			return;
		}
	}
}

void engine::destroy_font(std::string name) {
	for (int i = 0; i < ENGINE_MAX_FONTS; i++) {
		if (this->fonts[i] != NULL) {
			if (this->fonts[i]->id == name) {
				delete this->fonts[i];
				this->fonts[i] = NULL;
				printf("Destroyed Font '%s.'\n", name.c_str());
				return;
			}
		}
	}
	printf("Could not destroy Font '%s.'\n", name.c_str());
}
void engine::register_font(std::string name, std::string filepath, int size) {
	for (int i = 0; i < ENGINE_MAX_FONTS; i++) {
		if (this->fonts[i] != NULL) {
			if (this->fonts[i]->id == name) {
				printf("Font '%s' already loaded.\n", name.c_str());
				return;
			}
		}
	}
	for (int i = 0; i < ENGINE_MAX_FONTS; i++) {
		if (this->fonts[i] == NULL) {
			this->fonts[i] = new font(name, filepath, size);
			if (this->fonts[i]->_font == NULL) {
				delete this->fonts[i];
				this->fonts[i] = NULL;
				printf("Failed to create Font '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			} else {
				printf("Created Font '%s' from '%s.'\n", name.c_str(), filepath.c_str());
			}
			return;
		}
	}
}


void engine::destroy_sub_texture(std::string name) {
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] != NULL) {
			if (this->sub_textures[i]->id == name) {
				delete this->sub_textures[i];
				this->sub_textures[i] = NULL;
				printf("Destroyed Sub Texture '%s.'\n", name.c_str());
				return;
			}
		}
	}
	printf("Could not destroy Sub Texture '%s.'\n", name.c_str());
}

void engine::register_sub_texture(std::string id, std::string texture_id, int sub_w, int sub_h, int sub_x, int sub_y) {
	bool texture_found = false;
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] != NULL) {
			if (this->sub_textures[i]->id == id) {
				printf("Sub Texture '%s' already created.\n", texture_id.c_str());
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
		printf("Sub texture '%s' is outside of texture '%s.'\n", id.c_str(), texture_id.c_str());
		return;
	}
	
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] == NULL) {
			this->sub_textures[i] = new sub_texture(id, texture_id, sub_w, sub_h, sub_x, sub_y);
			printf("Created sub texture '%s' from '%s.'\n", id.c_str(), texture_id.c_str());
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
	printf("Texture '%s' could not be found.\n", name.c_str());
	return NULL;
}

TTF_Font* engine::get_font(std::string name) {
	for (int i = 0; i < ENGINE_MAX_TEXTURES; i++) {
		if (this->fonts[i] != NULL) {
			if (this->fonts[i]->id == name) {
				return this->fonts[i]->_font;
			}
		}
	}
	printf("Font '%s' could not be found.\n", name.c_str());
	return NULL;
}

TTF_Font* engine::load_font(std::string path, int size) {
	TTF_Font* new_font = TTF_OpenFont( path.c_str(), size );
    if( new_font == NULL )
    {
        printf( "TTF_OpenFont('%s') produced SDL_ttf error: '%s.'\n", path.c_str(), TTF_GetError() );
    }
    return new_font;
}
