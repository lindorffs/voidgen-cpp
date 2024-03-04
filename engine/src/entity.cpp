#include <entity.h>
#include <cmath>
#define PI 3.1413
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}
constexpr double radToDeg(double rad) { return rad*(180/PI); }
extern engine *l_engine;
extern SDL_Point getsize(SDL_Texture*);
//Screen dimension constants
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
void entity::_internal_render(void) {
	SDL_Texture *texture = this->engine_target->get_texture(this->texture_id);
	int x = SCREEN_WIDTH/2 + this->x - this->engine_target->state_manager->entity_states[0]->x;
	int y = SCREEN_HEIGHT/2 + this->y - this->engine_target->state_manager->entity_states[0]->y;
	SDL_Point size = getsize(texture);

	if (this->direct_movement){
		this->engine_target->render_direct(this->texture_id, x, y, size.x, size.y, true, 0);	
	} else {
		this->engine_target->render_direct(this->texture_id, x, y, size.x, size.y, true, this->rotation);
	}
}
void entity::_internal_update(std::chrono::milliseconds time_delta) {
	this->x += this->vx / time_delta.count();
	this->y += this->vy / time_delta.count();
}

entity::entity() {
}
entity::~entity() {
}

double entity::get_velocity_angle() {
    if (this->vx == 0) // special cases
        return (this->vy > 0)? 90
            : (this->vy == 0)? 0
            : 270;
    else if (this->vy == 0) // special cases
        return (this->vx >= 0)? 0
            : 180;
    double ret = radToDeg(atanf(this->vy/this->vx));
    if (this->vx < 0 && this->vy < 0) // quadrant Ⅲ
        ret = 180 + ret;
    else if (this->vx < 0) // quadrant Ⅱ
        ret = 180 + ret; // it actually substracts
    else if (this->vy < 0) // quadrant Ⅳ
        ret = 270 + (90 + ret); // it actually substracts
	ret = ret + 90;
	if (ret > 360) {
		ret = ret - 360;
	}
    return ret+0.1;
}

void entity::apply_forward_force(double force) {
	double rotation_radian = (this->rotation + 90) * (PI / 180);
	this->vx -= cos(rotation_radian) * force;
	this->vy -= sin(rotation_radian) * force;
}

entity::entity(std::string texture_id, std::string id, int x, int y, engine *engine_target) {
	this->engine_target = engine_target;
	this->texture_id = texture_id;
	this->base_texture_id = texture_id;
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vx = 0;
	
	this->id = id;
}
void  entity::render(void) {
	this->_internal_render();
}
void  entity::set_texture(std::string id) {
	this->texture_id = id;
}
void  entity::reset_texture(void) {
	this->texture_id = this->base_texture_id;
}
void  entity::update(std::chrono::milliseconds time_delta) {
	this->_internal_update(time_delta);
}
void ProjectileEntity::_internal_update(std::chrono::milliseconds time_delta) {
	this->x += this->vx / time_delta.count();
	this->y += this->vy / time_delta.count();
	printf("%d\n", this->id);
}

ProjectileEntity::ProjectileEntity() {
}

ProjectileEntity::ProjectileEntity(std::string texture_id, std::string id, int x, int y, engine *engine_target) {
	this->engine_target = engine_target;
	this->texture_id = texture_id;
	
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

PlayerClass::PlayerClass(engine *engine_target) {
	this->engine_target = engine_target;
	this->x = 0;
	this->y = 0;
	this->vy = 0;
	this->vx = 0;
	this->id = "player";
	this->texture_id = "player";
}

void  PlayerClass::_internal_render(void) {
	SDL_Texture *texture = this->engine_target->get_texture(this->texture_id);
	SDL_Point size = getsize(texture);
	if (this->direct_movement){
		this->engine_target->render_direct(this->texture_id, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, size.x, size.y, true, 0);	
	} else {
		this->engine_target->render_direct(this->texture_id, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, size.x, size.y, true, this->rotation);
	}
}
void  PlayerClass::_internal_update(std::chrono::milliseconds time_delta) {
	const Uint8 *key_state = this->engine_target->state_manager->keyboard_state;
	if (this->rotation > 360.0) {
		this->rotation = 0.0;
	} else if (this->rotation < 0) {
		this->rotation = 360;
	}
	if (this->direct_movement) {
		if (key_state[SDL_SCANCODE_W]) {
			this->y -= 10;
		}
		if (key_state[SDL_SCANCODE_A]) {
			this->x -= 10;
		}
		if (key_state[SDL_SCANCODE_S]) {
			this->y += 10;
		}
		if (key_state[SDL_SCANCODE_D]) {
			this->x += 10;
		}
	} else {
		this->x += this->vx / time_delta.count();
		this->y += this->vy / time_delta.count();
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
	}
}

void PlayerClass::fire(void) {
}