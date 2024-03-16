#include <engine.h>
#include <SDL.h>
#include <stdio.h>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern SDL_Point getsize(SDL_Texture *texture);

void engine::render_sub_texture(sub_texture *texture, int x, int y, double sx, double sy, bool centered, const double angle) {
	SDL_Rect source_rect;
	source_rect.x = texture->s_x;
	source_rect.y = texture->s_y;
	source_rect.w = texture->w;
	source_rect.h = texture->h;
	SDL_Rect target_rect;
	SDL_Point size = getsize(this->get_texture(texture->texture_resource));
	if (sx != 0) {
		target_rect.w = sx;
		if (centered) {
			target_rect.x = x - (sx) / 2;
		} else {
			target_rect.x = x;
		}
	} else {
		target_rect.w = size.x;
		if (centered) {
			target_rect.x = x - (size.x) /2;
		} else {
			target_rect.x = x;
		}
	}
	if (sy != 0) {
		target_rect.h = sy;
		if (centered) {
			target_rect.y = y - (sy) / 2;
		} else {
			target_rect.y = y;
		}
	} else {
		target_rect.h = size.y;
		if (centered) {
			target_rect.y = y - (size.y) /2;
		} else {
			target_rect.y = y;
		}
	}
	SDL_RenderCopyEx(this->renderer, this->get_texture(texture->texture_resource), &source_rect, &target_rect, angle, NULL, SDL_FLIP_NONE);
}
void engine::render_at(SDL_Texture* surface, int x, int y, double sx, double sy, bool centered, const double angle) {
	SDL_Rect target_rect;
	SDL_Point size = getsize(surface);
	if (sx != 0) {
		target_rect.w = sx;
		if (centered) {
			target_rect.x = x - (sx) / 2;
		} else {
			target_rect.x = x;
		}
	} else {
		target_rect.w = size.x;
		if (centered) {
			target_rect.x = x - (size.x) /2;
		} else {
			target_rect.x = x;
		}
	}
	if (sy != 0) {
		target_rect.h = sy;
		if (centered) {
			target_rect.y = y - (sy) / 2;
		} else {
			target_rect.y = y;
		}
	} else {
		target_rect.h = size.y;
		if (centered) {
			target_rect.y = y - (size.y) /2;
		} else {
			target_rect.y = y;
		}
	}
	SDL_RenderCopyEx(this->renderer, surface, NULL, &target_rect, angle, NULL, SDL_FLIP_NONE);
}



void engine::render_direct(std::string id, int x, int y, double sx, double sy, bool centered, const double angle) {
	for (int i = 0; i < ENGINE_MAX_SUB_TEXTURES; i++) {
		if (this->sub_textures[i] != NULL) {
			if (this->sub_textures[i]->id == id) {
				this->render_sub_texture(this->sub_textures[i], x, y, sx, sy, centered, angle);
				return;
			}
		}
	}
	
	SDL_Texture *texture = NULL;
	texture = this->get_texture(id);
	
	if (texture != NULL) {
		this->render_at(texture, x, y, sx, sy, centered, angle);
	}
}

void engine::render_logo(void) {
	SDL_RenderClear(this->renderer);
	this->render_at(this->load_logo, SCREEN_WIDTH /2 , SCREEN_HEIGHT /2, 256, 256, true, 0);
	SDL_RenderPresent(this->renderer);
}


void engine::render_text(std::string font_id, std::string text, int x, int y, int r, int g, int b, bool centered) {
	SDL_Color text_color = {r,g,b};
	SDL_Surface* textSurface = TTF_RenderText_Solid( this->get_font(font_id), text.c_str(), text_color);
	SDL_Texture* loaded_texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
	SDL_Point size = getsize(loaded_texture);
	this->render_at(loaded_texture, x, y, size.x, size.y, centered, 0);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(loaded_texture);
}