#ifndef PARTICLE_LIST
#define PARTICLE_LIST

typedef struct{
	Particle* particles; // Particle list
	int N; // Number of particles
	int size; // Number of coordinates (2 positions, 2 velocities for each particle)
	float* y; // Set of coordinates for each particle
	int index; // Whewn adding particles knows where to start
}Particle_list;

Particle_list gen_particle_list(int N, float* y, Particle* p){
	Particle_list output;
	output.N = N;
	output.size = 4*N;
	output.y = y;
	output.particles = p;
	output.index = 0;
	return output;
}

void add_particle(Particle_list *particle_list, float x, float y, float vx, float vy, float m, float r)
{
	int index = particle_list->index;
	if (particle_list->index < particle_list->N){
		// Adds the information to the coordinate vector
		particle_list->y[4*index] = x;
		particle_list->y[4*index+1] = y;
		particle_list->y[4*index+2] = vx;
		particle_list->y[4*index+3] = vy;
		// Adds the information to the Particle struct
		particle_list->particles[index] = gen_particle(
				&(particle_list->y[4*index]),
				&(particle_list->y[4*index+1]),
				&(particle_list->y[4*index+2]),
				&(particle_list->y[4*index+3]),
				m, r);
		// We have added a particle, we have to increase the index for the next one
		particle_list->index++; 
	}
	else{
		// If we try to add too many particles we raise an error
		printf("WARNING: Trying to add to many particles (increase N)\n");
	}
}

#endif
