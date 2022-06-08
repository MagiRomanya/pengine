#include <stdio.h>
#include "raylib.h"
#include "../lib/particle.h"
#include "../lib/particle_list.h"
#include "../lib/rungekutta4.h"

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

int main(void)
{
	int N = Nx*Ny; // Number of particles

	// Allocate memory for the particle coordinates
	float y[4*N]; // Particle's coordinates
	float y1[4*N]; 
	Particle particles[N]; //List of particles
	Particle particles1[N]; 

	Particle_list particle_list = gen_particle_list(N, y, particles);
	Particle_list particle_list1 = gen_particle_list(N, y1, particles1);

	genParticleGrid(&particle_list, Nx, Nx, DeltaX, DeltaY);
	genParticleGrid(&particle_list1, Nx, Nx, DeltaX, DeltaY);

	// RAYLIB:
	InitWindow(800, 450, "test");
	SetTargetFPS(60);
	while (!WindowShouldClose()){
		// COMPUTING NEXT FRAME
		rungekutta4(0.0f, &particle_list, &particle_list1, test);
		for (int j=0; j<4*N; j++){
			particle_list.y[j] = particle_list1.y[j];
		}
		// DRAWING
		BeginDrawing();
		ClearBackground(RAYWHITE);
		for (int i=0; i<N; i++){
			DrawCircle(*particle_list.particles[i].x,
				   *particle_list.particles[i].y,
				   particle_list.particles[i].r,
				   MAROON);
		}
		EndDrawing();
	}
	CloseWindow();
}
