#include "raylib.h"
#include <stdio.h>
#include "math.h"
#include "../lib/particle.h"
#include "../lib/particle_list.h"
#include "../lib/rungekutta4.h"


#define screenWidth 1200
#define screenHeight 1200

// Haky way to calculate inverse square root very fast
float InvSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;            // store floating-point bits in integer
	i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
	x = *(float*)&i;              // convert new bits into float
	x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
	return x;
}

int Springs(float x, Particle_list *p, float f[]);

// Renders the nodes as balls
void drawParticles(Particle particles[], int n){
	Color colors[]= {
		DARKPURPLE,
	};
	int n_colors=sizeof(colors)/sizeof(RED);
	for (int i =0; i<n; i++){
		DrawCircle(*particles[i].x, *particles[i].y, particles[i].r, colors[i%n_colors]);
		//DrawText(TextFormat("%i", particles[i].id), particles[i].x, particles[i].y, 10, LIGHTGRAY);
	}
}

// Calculates the force between two particles joined by a string
Vector2 spring(Particle p1, Particle p2, float K0, float l0)
{
	Vector2 force;
	Vector2 radialVec = (Vector2){*p2.x-*p1.x, *p2.y-*p1.y}; //
	float dist = radialVec.x*radialVec.x + radialVec.y*radialVec.y; // Distance squared

	force.x = K0*(1-l0*InvSqrt(dist))*radialVec.x;
	force.y = K0*(1-l0*InvSqrt(dist))*radialVec.y;

	//force.x = K0*radialVec.x;
	//force.y = K0*radialVec.y;
	return force;
}

int FirstNeighbors();

void genParticleGrid(Particle_list *p, int nx, int ny, float deltax, float deltay){
	float x0 = 180.0f;
	float y0 = 50.0f;
	for (int i=0; i < nx; i++){
		for (int j=0; j < ny; j++) {
			add_particle(p, x0+deltax*i, y0 +deltay*j, 0.0f, 0.0f, 1.0f, 10.0f);
		}
	}
	FirstNeighbors();
}
#define Nx 40
#define Ny 40
#define DeltaX 20.f
#define DeltaY 20.f

bool lattice[Nx*Ny][Nx*Ny];

// Creates a list of neighbours
int FirstNeighbors(){
	// Loop in the grid: i,j are the coordinates in the grid (x->i, y->j)
	for (int i=0; i<Nx-1; i++) {
		for (int j=0; j<Ny-1; j++) {
			// particle index = j + Nx*i
			lattice[j+Nx*i][j+1+Nx*i] = true;
			lattice[j+Nx*i][j+Nx*(i+1)] = true;
		}
		lattice[Ny-1 +Nx*i][Ny-1 + Nx*(i+1)] = true;
		lattice[i +Nx*(Nx-1)][i+1 + Nx*(Nx-1)] = true;
	}
	return 0;

}

// Calculates the total force including:
// One body: gravity and drag
// Two body: Springs between nodes
int Springs(float x, Particle_list *p, float f[]){
	const float K = 60.0f;
	const float dump = 1e-1f;
	const float d0 = 1.0f*DeltaX;
	const float g0 = -5.0f; // Gravetat
	const float size = p->size;
	Vector2 force;
	int i,j;

	// Set the velocities
	for (i=0; i<size; i+=4) {
		f[i] = *p->particles[i/4].vx;
		f[i+1] = *p->particles[i/4].vy;
		f[i+2] = 0.0f;
		f[i+3] = 0.0f;
	}
	// Set the accelerations
	// One body interactions:
	for (i = 0; i<size; i+=4) {
		f[i+2] += -dump*(*p->particles[i/4].vx); // horizontal drag
		f[i+3] += -g0 -dump*(*p->particles[i/4].vy); // vertical drag + gravity
	}
	// Two body interaction
	for (i=0; i<size; i+=4) {
		for (j=i; j<size; j+=4) {
			if (lattice[i/4][j/4]) { // latice returns true if two particles are joined by a spring
				force = spring(p->particles[i/4], p->particles[j/4], K, d0);
				f[i+2] += force.x;
				f[i+3] += force.y;
				f[j+2] -= force.x;
				f[j+3] -= force.y;
			}
		}
	}
	// Fix in place the top corners (no force)
	f[0+2] = 0;
	f[0+3] = 0;
	f[4*(0+Nx*(Nx-1))+2] = 0;
	f[4*(0+Nx*(Nx-1))+3] = 0;
	return 0;
}

int main(void){
	InitWindow(screenWidth, screenHeight, "Cloth");
	int N = Nx*Ny; // Number of particles

	// Allocate memory for the particle coordinates
	float y[4*N]; // Particle's coordinates
	float y1[4*N]; 
	Particle particles[N]; //List of particles
	Particle particles1[N]; 

	// Instanciate two particle list (one for the current step and another for the next)
	Particle_list particle_list = gen_particle_list(N, y, particles);
	Particle_list particle_list1 = gen_particle_list(N, y1, particles1);

	// Create a grid of particles which create the cloth
	genParticleGrid(&particle_list, Nx, Ny, DeltaX, DeltaY);
	genParticleGrid(&particle_list1, Nx, Ny, DeltaX, DeltaY);

	int pause = 0;
	SetTargetFPS(60);
	int selectedBall = 0;
	int isBallSelected = 0;
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_SPACE)) {
			if (pause) pause = 0;
			else pause = 1;
		}
		if (IsKeyPressed(KEY_Q)) { break; } // Q exit button
		//Drag BALL with the mouse
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			for (int i=0; i<N; i++){
				if (touch_particle(particle_list.particles[i], 
							GetMousePosition().x, 
							GetMousePosition().y))
				{
					selectedBall = i;
					isBallSelected = 1;
					break;
				}
				isBallSelected = 0;
			}

		}

		if (!pause) {
			// Calculate next positions
			rungekutta4(0.0f, &particle_list, &particle_list1, Springs);
			for (int j=0; j<4*N; j++){
				particle_list.y[j] = particle_list1.y[j];
			}
		}
		// Drag ball with mouse
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && isBallSelected){
			*particle_list.particles[selectedBall].x = GetMousePosition().x;
			*particle_list.particles[selectedBall].y = GetMousePosition().y;
		}

		// RENDERING
		BeginDrawing();
		ClearBackground(RAYWHITE);
		drawParticles(particle_list.particles, particle_list.N);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
