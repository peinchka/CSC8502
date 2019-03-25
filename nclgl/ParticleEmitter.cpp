#include "ParticleEmitter.h"
#include <string>
#include <io.h>
#include <stdio.h>

#define RAND() ((rand()%101)/100.0f)

ParticleEmitter::ParticleEmitter(void)	{
	particleRate = 100.0f;
	particleLifetime = 5000.0f;
	particleSize = 24.0f;
	particleVariance = 0.2f;
	nextParticleTime = 0.0f;
	particleSpeed = 2.2f;
	numLaunchParticles = 100;
	largestSize = 0;

	texture = SOIL_load_OGL_texture(TEXTUREDIR"snowflake2.png", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
}

ParticleEmitter::~ParticleEmitter(void) {
	for (std::vector<Particle *>::iterator i = particles.begin(); i != particles.end(); ++i) {
		delete (*i);
	}
	for (std::vector<Particle *>::iterator i = freeList.begin(); i != freeList.end(); ++i) {
		delete (*i);
	}
}

void ParticleEmitter::Update(float msec)	{
	nextParticleTime -= msec;

	while(nextParticleTime <= 0) {
		nextParticleTime += particleRate;
		for(int i = 0; i < numLaunchParticles; ++i) {
			particles.push_back(GetFreeParticle());
		}
	}

	for(std::vector<Particle *>::iterator i = particles.begin(); i != particles.end(); ) {
		Particle *p = (*i);

		p->colour.w -= (msec / particleLifetime);

		if(p->colour.w <= 0.0f) {
			freeList.push_back(p);
			i = particles.erase(i);
		}
		else{
			p->position += p->direction*(msec*particleSpeed);
			++i;
		}
	}

	if(particles.size() > largestSize) {
		ResizeArrays();	
	}
}

Particle* ParticleEmitter::GetFreeParticle()	{
	Particle * p = NULL;

	if(freeList.size() > 0) {
		p = freeList.back();
		freeList.pop_back();
	}
	else{ 
		p = new Particle();
	}

	p->colour = Vector4(1, 1, 1, 1.0);
	p->direction	= initialDirection;
	
	p->direction.x += ((RAND() - RAND()) * particleVariance);
//	p->direction.y += ( - RAND() * particleVariance);
	p->direction.y += (RAND() - RAND() * particleVariance);
	p->direction.z += ((RAND() - RAND()) * particleVariance);

	p->direction.Normalise();
	p->position.ToZero();

	return p;	
}

/*
If we end up with more particles than space in graphics memory, we must
allocate some new graphics memory!
*/
void	ParticleEmitter::ResizeArrays() {
	//Delete the system memory, as it is of the wrong size now...
	//yes, this does mean we're keeping particle info in THREE places...
	delete[] vertices;
	delete[] colours;

	//Tell OpenGL that we can delete our old VBOs
	glDeleteBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glDeleteBuffers(1, &bufferObject[COLOUR_BUFFER]);

	//Make some new system memory
	vertices = new Vector3[particles.size()];
	colours  = new Vector4[particles.size()];

	//Make some new VBO space in memory. Note how we don't actually upload any
	//data to the graphics card here! We just 'new' the memory for now.
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size()*sizeof(Vector3), 0, GL_DYNAMIC_DRAW);


	glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size()*sizeof(Vector4), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//Done with our buffers...

	largestSize = particles.size();		//Update our largest particles size
}

/*
Now for the overloaded Draw function. It's a little bit messy, as we have to copy all of the
Particle data structs into one contiguous lump of memory to send using the OpenGL command
glBufferSubData - which you should remember from the skeletal animation tutorial. 

Why don't we use glMapBuffer? Just uploading a whole load of new data is probably easier
and quicker than faffing with mapping and unmapping buffers!
*/
void ParticleEmitter::Draw()	{
	//Get 2 contiguous sections of memory full of our particle info
	for(unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i]  = particles.at(i)->colour;
	}

	glBindVertexArray(arrayObject);

	//Bind our vertex data, and update its data with that of the vertices array
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size()*sizeof(Vector3), (void*)vertices);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);	//Tell the VAO we have positions...
	glEnableVertexAttribArray(VERTEX_BUFFER);	//Binds this buffer to the VAO
	
	//And now do the same for colours...
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size()*sizeof(Vector4), (void*)colours);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
	glEnableVertexAttribArray(COLOUR_BUFFER);
	
	//We're going to use a type of alpha blending known as additive blending here. Overlapping 
	//particles will have their colours added together on screen, giving us a cool effect when
	//particles are near each other. Fire a lot of particles at once with slow speed to get a
	//'plasma ball' type effect!
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	//And now do our usual Drawing stuff...
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_POINTS,  0, particles.size());	// draw ordered list of vertices
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0); //Remember to turn off our VAO ;)
};