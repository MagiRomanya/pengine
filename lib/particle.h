#ifndef particle
#define particle

// Particle type struct
typedef struct{
	float *x, *y;
	float *vx, *vy;
	float m;
	float r;
}Particle;

Particle gen_particle(float* x, float* y, float* vx, float* vy, float m, float r)
{
	Particle output;
	output.x = x;
	output.y = y;
	output.vx = vx;
	output.vy = vy;
	output.m = m;
	output.r = r;
	return output;
}

int touch_particle(Particle p, float x, float y){
	int output = 0;
	float dist = (x-*p.x)*(x-*p.x) + (y-*p.y)*(y-*p.y);
	if (dist < p.r*p.r) output = 1;
	return output;
}
#endif
