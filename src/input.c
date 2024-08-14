#include "puzzle.h"

// Keep track of instance IDs
struct instance_to_controller {
	SDL_JoystickID instance;
	SDL_GameController *controller;
	struct instance_to_controller *prev, *next;
};
struct instance_to_controller *controllers = NULL;

int controller_keys = 0;

// Simplified down a lot to keep it to the point
int common_event_handler(SDL_Event *e) {
	SDL_GameController* controller;
	struct instance_to_controller *controller_mapping;
	int game_key = 0;

	switch(e->type) {
		case SDL_CONTROLLERDEVICEADDED:
			controller = SDL_GameControllerOpen(e->cdevice.which);
			if(controller) {
				SDL_Joystick *joy = SDL_GameControllerGetJoystick(controller);
				controller_mapping = calloc(1, sizeof(struct instance_to_controller));
				controller_mapping->instance = SDL_JoystickInstanceID(joy);
				controller_mapping->controller = controller;
				controller_mapping->next = controllers;
				if(controllers) {
					controllers->prev = controller_mapping;
				}
				controller_mapping->prev = NULL;
				controllers = controller_mapping;
			}
			return 1;
		case SDL_CONTROLLERDEVICEREMOVED:
			controller_keys = 0;
			for(controller_mapping = controllers; controller_mapping; controller_mapping=controller_mapping->next) {
				if(controller_mapping->instance == e->cdevice.which) {
					if(controller_mapping == controllers)
						controllers = controller_mapping->next;
					if(controller_mapping->next)
						controller_mapping->next->prev = controller_mapping->prev;
					if(controller_mapping->prev)
						controller_mapping->prev->next = controller_mapping->next;
					free(controller_mapping);
					break;
				}
			}
			return 1;

		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERBUTTONDOWN:
			switch(e->cbutton.button) {
				case SDL_CONTROLLER_BUTTON_A:
					game_key = KEY_A;
					break;
				case SDL_CONTROLLER_BUTTON_B:
					game_key = 0;
					break;
				case SDL_CONTROLLER_BUTTON_X:
					game_key = KEY_B;
					break;
				case SDL_CONTROLLER_BUTTON_Y:
					game_key = KEY_DEBUG;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					game_key = KEY_UP;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					game_key = KEY_DOWN;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					game_key = KEY_LEFT;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					game_key = KEY_RIGHT;
					break;
			}
			if(e->cbutton.state == SDL_PRESSED) {
				controller_keys |= game_key;
			} else {
				controller_keys &= ~game_key;
			}
			return 1;
	}
	return 0;
}
