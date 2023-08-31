#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <math.h>

//gcc wave.c -lasound -lm -lSDL2 -o wave


const int width = 1920;
const int height = 1080;
const uint16_t delay = 15;
const float pi = 3.14159265359;


//loud ass beeps, turn volume down
void beep(int freq) {

	int ms = 50;

	if (freq < 1) {
	
		freq = 1;
	}

	static snd_pcm_t *pcm = NULL;
	
	if (pcm == NULL) {
	
		if (snd_pcm_open(&pcm, "default", 0, 0)) {
		
		}
		snd_pcm_set_params(pcm, 1, 3, 1, 44100, 1, 20);
	}
	

	unsigned char buf[2400];
	long frames, phase;

	for (int i = 0; i < (ms / 50); i++) {
	
		snd_pcm_prepare(pcm);

		for (int j = 0; j < sizeof(buf); j++) {
		
			buf[j] = freq > 0 ? (255 * j * freq / 8000) : 0;
		}

		int r = snd_pcm_writei(pcm, buf, sizeof(buf));

		if (r < 0) {

			snd_pcm_recover(pcm, r, 0);
		}
	}	
}






void loop(uint8_t R, uint8_t G, uint8_t B,
	double xmulti, double ymulti,
	SDL_Renderer* renderer,
	double func(double)) {
	
	int16_t n = 0;
	int16_t wave = 0;

	int16_t pn = 0;
	int16_t pwave = 0;
	

	while (n < width) {
		
		wave = (int16_t)(func((double)n) * ymulti);
		wave %= height;

		beep((int)wave + (height/2));

		//draw
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		SDL_RenderDrawLine(renderer, pn, (pwave+(height/2)), 
					     n , (wave+(height/2)));
		pn = n;
		pwave = wave;
		n += 1 * xmulti;

		SDL_RenderPresent(renderer);
		SDL_Delay(delay);
	}
}



void circle(uint8_t R, uint8_t G, uint8_t B,
	double a, double b, double radius, double detail, 
	bool lines,
	SDL_Renderer* renderer) {

	int color = 0xffffff;
	SDL_SetRenderDrawColor(renderer, R, G, B, 255);


	double t = (pi * 2.0) * detail;
	double x, y = 0;
	int oldx = a;
	int oldy = b;
	
	bool preventMiddleLine = false;


	while (t > 0.0) {
	
		x = (a + radius * cos(t)) * 1.0;
		y = (b + radius * sin(t)) * 1.0;


		if (lines) {
			if (preventMiddleLine) {
			
				SDL_RenderDrawLine(renderer, oldx, oldy, 
						(int)x, (int)y);
				beep((int)y);
			}
			oldx = (int)x;
			oldy = (int)y;
		} else {
			printf("%lf %lf\n", x, y);
			SDL_RenderDrawPoint(renderer, (int)x, (int)y);
		}
			
		SDL_SetRenderDrawColor(renderer, color >> 16, 
				(color >> 8) & 0xff, color & 0xff, 255);


		t--;
		color -= 0xfff;
		SDL_RenderPresent(renderer);
		SDL_Delay(delay);
		preventMiddleLine = true;
	}
}


//fucked up :(
void semicircle(uint8_t R, uint8_t G, uint8_t B,
	double a, double b, double radius, double detail, 
	SDL_Renderer* renderer) {

	
	SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	int color = 0xffffff;
	
	double x = a;
	double y = b;

	int i = 0;
	
	//haha
	while (i < radius * 2.0) {

		y = b + sqrt((radius * radius) - pow(x - a, 2.0));
	
		SDL_SetRenderDrawColor(renderer, color >> 16, 
				(color >> 8) & 0xff, color & 0xff, 255);


		beep((int)y);
		SDL_RenderDrawPoint(renderer, (int)x, (int)y);


		x++;
		i++;
		color -= 0xfff;
		
		SDL_RenderPresent(renderer);
		SDL_Delay(delay);
	}
}



void animate(SDL_Renderer* renderer) {
	
	uint8_t R, G, B = 0;

	double xmulti = 10;
	double ymulti = 50;

	//trig	
	loop(255, 0, 0, xmulti, ymulti, renderer, cos);
	loop(0, 255, 0, xmulti, ymulti, renderer, sin);
	loop(0, 0, 255, xmulti, ymulti, renderer, tan);
	
	loop(0, 0, 128, xmulti, ymulti, renderer, acos);
	loop(0, 128, 0, xmulti, ymulti, renderer, asin);
	loop(128, 0, 0, xmulti, ymulti, renderer, atan);


	loop(64, 0, 64, xmulti, ymulti, renderer, cosh);
	loop(0, 64, 64, xmulti, ymulti, renderer, sinh);
	loop(255, 255, 0, xmulti, ymulti, renderer, tanh);



	//others
	loop(0, 255, 255, xmulti, ymulti, renderer, exp);
	loop(255, 0, 255, xmulti, ymulti, renderer, log10);
	loop(0, 128, 128, xmulti, ymulti, renderer, log);
	
	loop(64, 64, 0, xmulti, ymulti, renderer, fabs);
	loop(0, 64, 64, xmulti, ymulti, renderer, ceil);
	loop(64, 0, 64, xmulti, ymulti, renderer, floor);
	
	loop(128, 128, 128, xmulti, ymulti, renderer, sqrt);


	double a = (double)(width/2);
	double b = (double)(height/2);
	double radius = 300.0;
	

	circle(255, 255, 255, a, b, radius, 60, 1, renderer);
	
	//eyes	
	circle(255, 255, 255, a+(radius/2), b-(radius/2), 30, 15, 1, renderer);
	circle(255, 255, 255, a-(radius/2), b-(radius/2), 30, 15, 1, renderer);

	//mouth		
	semicircle(255, 255, 255, a, b, 100, 40, renderer);
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
	//SDL_RenderSetLogicalSize(renderer, width, height);
	printf("Init done. Starting main loop...\n");


	animate(renderer);



	printf("Program ended.\n");	
	SDL_Delay(3000);

	return 0;	
}
