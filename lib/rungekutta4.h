#ifndef RUNGEKUTA4
#define RUNGEKUTA4

int test(float t, Particle_list *p, float K[]){
	int N = p->N;
	int size = p->size;
	float k = -1.0f;
	float xorigin = 500.0f;
	
	for (int i=0; i < N; i++){
		K[4*i] = *p->particles[i].vx;
		K[4*i+1] = *p->particles[i].vy;
		K[4*i+2] = k*(*p->particles[i].x - xorigin);
		K[4*i+3] = 0.0f;
	}
	return 0;
}

int rungekutta4(float t, Particle_list *p0, Particle_list *p1, int (*func)(float, Particle_list*, float*)){
	int N = p0->N;
	int i = 0;
	float h = 0.1f;
	int size = p0->size;
	float K1[size], K2[size], K3[size], K4[size];

	// K1 = func(t,y0)
	func(t, p0, K1);
	// K2 = func(t +h/2, y0 + h/2*K1)
	for (i=0; i<size; i++){
		p1->y[i] = p0->y[i] + h/2*K1[i];
	}
	func(t +h/2, p1, K2);

	// K3 = func(t +h/2, y0 + h/2*K2)
	for (i=0; i<size; i++){
		p1->y[i] = p0->y[i] + h/2*K2[i];
	}
	func(t +h/2, p1, K3);

	// K4 = func(t +h, y0 + h/2*K2)
	for (i=0; i<size; i++){
		p1->y[i] = p0->y[i] + h*K3[i];
	}
	func(t + h, p1, K4);

	
	for (i=0; i<size; i++){
		p1->y[i] = p0->y[i] + h/6*(K1[i] + 2*K2[i] + 2*K3[i] + K4[i]);
	}
	return 0;
}
#endif
