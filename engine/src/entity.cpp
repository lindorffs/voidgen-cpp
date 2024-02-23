#include <entity.h>
#include <cmath>
#define PI 3.1

//Screen dimension constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
void entity::_internal_render(void) {
	int render_x = SCREEN_WIDTH/2 + this->x - this->state_target->entities[0]->x;
	int render_y = SCREEN_HEIGHT/2 + this->y - this->state_target->entities[0]->y;
	
	this->engine_target->render_at_scale_rotate(this->entity_image, render_x, render_y, 0.5, 0.5, false, this->rotation);
}
void entity::_internal_update(std::chrono::milliseconds time_delta) {
	this->x += this->vx / time_delta.count();
	this->y += this->vy / time_delta.count();
}

entity::entity() {
}

entity::entity(std::string fileName, int x, int y, engine *engine_target, OpenSpaceState *state_target) {
	this->engine_target = engine_target;
	this->state_target = state_target;
	this->entity_image = engine_target->load_image(fileName);
	
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vx = 0;
}
void  entity::render(void) {
	this->_internal_render();
}
void  entity::update(std::chrono::milliseconds time_delta) {
	this->_internal_update(time_delta);
}
void ProjectileEntity::_internal_render(void) {
	int render_x = SCREEN_WIDTH/2 + this->x - this->state_target->entities[0]->x;
	int render_y = SCREEN_HEIGHT/2 + this->y - this->state_target->entities[0]->y;
	
	this->engine_target->render_at_scale_rotate(this->entity_image, render_x, render_y, 0.5, 0.5, false, this->rotation);
}
void ProjectileEntity::_internal_update(std::chrono::milliseconds time_delta) {
	this->x += this->vx / time_delta.count();
	this->y += this->vy / time_delta.count();
	printf("%d\n", this->id);
}

ProjectileEntity::ProjectileEntity() {
}

ProjectileEntity::ProjectileEntity(std::string fileName, int x, int y, engine *engine_target, OpenSpaceState *state_target, int id) {
	this->engine_target = engine_target;
	this->state_target = state_target;
	this->entity_image = engine_target->load_image(fileName);
	
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vx = 0;
	this->id = id;
}
void  ProjectileEntity::render(void) {
	this->_internal_render();
}
void  ProjectileEntity::update(std::chrono::milliseconds time_delta) {
	this->_internal_update(time_delta);
}

PlayerClass::PlayerClass(engine *engine_target, OpenSpaceState *state_target) {
	this->engine_target = engine_target;
	this->state_target = state_target;
	this->entity_image = engine_target->load_image("player.png");
	this->x = 0;
	this->y = 0;
	this->vy = 0;
	this->vx = 0;
}

void  PlayerClass::_internal_render(void) {
	this->engine_target->render_at_scale_rotate(this->entity_image, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 2, 2, true, this->rotation);
}
void  PlayerClass::_internal_update(std::chrono::milliseconds time_delta) {
	const Uint8 *key_state = this->engine_target->state_manager->keyboard_state;
	this->x += this->vx / time_delta.count();
	this->y += this->vy / time_delta.count();
	if (this->rotation >= 360.0) {
		this->rotation = 0.0;
	} else if (this->rotation <= -360.0) {
		this->rotation = 0.0;
	}
	if (key_state[SDL_SCANCODE_W]) {
		this->vx -= 10 * cos((this->rotation + 90) * (PI / 180));
		this->vy -= 10 * sin((this->rotation + 90) * (PI / 180));
	}
	if (key_state[SDL_SCANCODE_S]) {
		this->vx += 5 * cos((this->rotation + 90) * (PI / 180));
		this->vy += 5 * sin((this->rotation + 90) * (PI / 180));
	}
	if (key_state[SDL_SCANCODE_Q]) {
		this->rotation -= 1.5;
	}

	if (key_state[SDL_SCANCODE_E]) {
		this->rotation += 1.5;
	}
	if (key_state[SDL_SCANCODE_X]) {
		this->vx = 0;
		this->vy = 0;
	}
	if (key_state[SDL_SCANCODE_V]) {
		this->x = 0;
		this->y = 0;
	}

}

void PlayerClass::fire(void) {
}
