#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "kinematics.h"


//poorly made program used just to make funny lines



//constants (edit these)
const int width = 1260;
const int height = 1080;
const uint16_t delay = 5;

const double c_multix = 1.0;
const double c_multiy = 1.0;

const double c_ux = -20.0;
const double c_uy = -50.0;

const double c_vx = 0.0;
const double c_vy = 0.0;

const uint32_t c_t = 0;

const double c_sx = 0.0;
const double c_sy = 0.0;

//const double c_sxi = width/2.0;
const double c_sxi = width-200;
const double c_syi = height;
//const double c_syi = height/2.0;

bool flag = true;
uint32_t tick = 0;



//variables used during program
double multix = c_multix;
double multiy = c_multiy;


uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

	
double ux = c_ux; //initial horizontal velocity
double uy = c_uy; //initial vertical velocity

double vx = c_vx; //horizontal velocity
double vy = c_vy; //vertical velocity

//must be constant for kinematics
const double ax = -8.0; //horizontal acceleration 
const double ay = -KINEMATICS_GRAVITY_EARTH; //vertical acceleration 

uint32_t t = c_t; //time

double sx = c_sx; //horizontal displacement
double sy = c_sy; //vertical displacement

double sxi = c_sxi; //horizontal initial displacement
double syi = c_syi; //vertical initial displacement




void reset(SDL_Rect* rect) {

	sxi = c_sxi;
	syi = c_syi;
	
	sx = sxi;
	sy = syi;

	t = c_t;

	ux = c_ux;
	uy = c_uy;

	rect->x = sx;
	rect->y = sy;
}




void loop(SDL_Renderer* renderer, SDL_Rect* rect) {

	do {
		//draw

		//tail
		if (vy >= 0) { R = 255; G = 0; B = 0; } 
		else 	     { R = 0; G = 255; B = 0; }
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		SDL_RenderDrawLine(renderer, rect->x, rect->y, sx, sy);
		rect->x = sx;
		rect->y = sy;

	
		//kinematics
		vx = KINEMATICS_VELOCITY_UAT(ux, ax, t);
		vy = KINEMATICS_VELOCITY_UAT(uy, ay, t);
		sx = KINEMATICS_DISPLACEMENT_UAT(ux, ax, t) + sxi;
		sy = KINEMATICS_DISPLACEMENT_UAT(uy, ay, t) + syi;
		

		t++;

		//update frame
		SDL_Delay(delay);
		SDL_RenderPresent(renderer);
		

		printf("VelocityX: %lf | DisplacementX: %lf | Time: %ld\n", vx, sx, t);
		printf("VelocityY: %lf | DisplacementY: %lf\n", -vy, sy);


	} while (rect->x <= width && rect->x && rect->y <= height);
	//} while (rect->x && rect->y <= height);
}



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

	/*
	while (tick < 4999) {

		loop(renderer, rect);
		reset(rect);
		
		if (vx >= 0.0) {
			if (flag) {
			
				//multiy = c_multiy;
			}
			//uy -= multiy;
			uy += multiy;
			flag = false;
			multiy--;
		} else {
			uy += multiy;
			multiy++;
		}

		ux += multix;
		
		multix++;
		//multiy++;
		tick++;
	}
	*/
	
	/*
	while (tick < 4999) {
		
		loop(renderer, rect);
		reset(rect);


		if (tick % 2) {
		
			ux += multix;
		} else {
		
			ux -= multix;
		}
		uy += multiy;

		multix++;
		multiy++;
		tick++;
	}
	*/


	while (tick < 4999) {

		loop(renderer, rect);
		reset(rect);
		
		
		ux += multix;
		uy -= multiy;
		
		multix++;
		multiy++;
		tick++;
	}


	printf("Program ended.\n");	
	SDL_Delay(1000);

	return 0;	
}
