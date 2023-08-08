#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "kinematics.h"


const int width = 1920;
const int height = 1080;
const uint16_t delay = 25;

const double unitdiv = 1.0;

uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

	
double ux = 75.0; //initial horizontal velocity
double uy = -120.0; //initial vertical velocity

double vx = 0.0; //horizontal velocity
double vy = 0.0; //vertical velocity

//must be constant for kinematics
const double ax = 0.0; //horizontal acceleration 
const double ay = -KINEMATICS_GRAVITY_EARTH; //vertical acceleration 

uint32_t t = 0; //time

double sx = 0.0; //horizontal displacement
double sy = 0.0; //vertical displacement

double sxi = 0.0; //horizontal initial displacement
double syi = height; //vertical initial displacement





int main() {
	
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, 1, 1);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_Delay(10);

	SDL_RenderFillRect(renderer, NULL);
	SDL_RenderPresent(renderer);
	SDL_RenderSetLogicalSize(renderer, width, height);
	printf("Init done. Starting main loop...\n");

	//initial values
	sx = sxi;
	sy = syi;
	
	SDL_Rect* rect;
	rect->x = sx;
	rect->y = sy;
	rect->w = 8;
	rect->h = 8;

	
	do {
		//draw

		//tail
		if (vy >= 0) { R = 255; G = 0; B = 0; } 
		else 	     { R = 0; G = 255; B = 0; }
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		SDL_RenderDrawLine(renderer, rect->x, rect->y, sx, sy);
		rect->x = sx;
		rect->y = sy;

	
		//head	
		R = 0; G = 0; B = 255;
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		SDL_RenderFillRect(renderer, rect);

		
		//kinematics
		vx = (KINEMATICS_VELOCITY_UAT(ux, ax, t) / unitdiv);
		vy = (KINEMATICS_VELOCITY_UAT(uy, ay, t) / unitdiv);
		sx = (KINEMATICS_DISPLACEMENT_UAT(ux, ax, t) + sxi) / unitdiv;
		sy = (KINEMATICS_DISPLACEMENT_UAT(uy, ay, t) + syi) / unitdiv;
		

		t++;

		//update frame
		SDL_Delay(delay);
		SDL_RenderPresent(renderer);
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, rect);
		

		printf("VelocityX: %lf | DisplacementX: %lf | Time: %ld\n", vx, sx, t);
		printf("VelocityY: %lf | DisplacementY: %lf\n", -vy, sy);


	} while (rect->x <= width && rect->x >= 0 
		&& rect->y <= height && rect->y >= 0);


	printf("Program ended.\n");	
	SDL_Delay(1000);

	return 0;	
}
