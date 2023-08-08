#include <stdio.h>
#include <SDL2/SDL.h>

#include "kinematics.h"


const int width = 1280;
const int height = 16;
const uint16_t delay = 25;



uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;



uint32_t x = 0;
uint32_t newx = 0;


	
double u = 70.0; //initial velocity
double v = 0.0; //velocity
double v_2 = 0.0; //velocity^2

//must be constant for kinematics
const double a = -2.0; //acceleration 

uint32_t t = 0; //time
double s = 0.0; //displacement
double si = 0.0; //initial displacement



int main() {
	
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, 1, 1);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_Delay(25);

	SDL_RenderFillRect(renderer, NULL);
	SDL_RenderPresent(renderer);
	printf("Init done. Starting main loop...\n");

	//initial values
	s += si;

	SDL_Rect* rect;
	rect->x = si;
	rect->y = 0;
	rect->w = 16;
	rect->h = 16;
		


	while (1) {


		//draw
		//tail
		if (v >= 0) {
			R = 0; G = 255; B = 0;
			SDL_SetRenderDrawColor(renderer, R, G, B, 255);
			
			for (rect->x; rect->x < s; rect->x++) {	
		
				SDL_RenderFillRect(renderer, rect);
			}
		} else {
			R = 255; G = 0; B = 0;
			SDL_SetRenderDrawColor(renderer, R, G, B, 255);
			
			for (rect->x; rect->x > s; rect->x--) {			
				SDL_RenderFillRect(renderer, rect);
			}
		}
		
		
		//head	
		R = 0; G = 0; B = 255;
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		SDL_RenderFillRect(renderer, rect);
		

		//kinematics
		v = KINEMATICS_VELOCITY_UAT(u, a, t);
		s = KINEMATICS_DISPLACEMENT_UVT(u, v, t) + si;
		
		//newx = s / unitdiv;


		t++;

		//update frame
		SDL_Delay(delay);
		SDL_RenderPresent(renderer);
		

		//print and check for exit
		printf("Velocity: %lf | Displacement: %lf | Time: %ld\n", v, s, t);
		if (rect->x > width || rect->x < 0) {break;}
	}

	printf("Program ended.\n");
	return 0;	
}
