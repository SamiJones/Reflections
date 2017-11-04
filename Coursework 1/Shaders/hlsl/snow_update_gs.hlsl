
//
// Model the update of each particle.  Each particle is modelled in world coordinate space
//


// Resources

cbuffer GameTime : register (b0) {

	float		gameTimeDelta;
};


cbuffer SnowSystemUpdateConstants : register (b1) {

	float4		gravity;
	float		resetAge;
	float		ageDelta;
	float		generatorAgeThreshold;

	// Pseudo-random numbers used for new particle configuration
	float		init_x_velocity;
	float		init_z_velocity;
	float		init_angular_velocity;
	float		init_age;

	// Pesudo-random number in the range [0, 8) to determine the initial particle type.  Again, only a single new particle likely in every update cycle  (see above)
	uint		randomFlakeType;

	float		init_weight;
	float		init_x;
	float		init_z;
};



// Input / Output structures

// For the update process, the geometry shader (GS) inputs and outputs the same vertex structure
struct ParticleStructure {

	float3				pos				: POSITION;
	float3				velocity		: VELOCITY;
	float				weight : WEIGHT;
	float				age : AGE;
	float				theta : ORIENTATION;
	float				angularVelocity : ANGULARVELOCITY;
	uint				isaGenerator	: GENERATOR;

	// Determine which texture will be used [0, 7]
	uint				flakeType		: FLAKETYPE;
};


// Output a maximum of 2 particles (current particle + possible new particle (if current particle is a generator and time has passed to create a new particle)
[maxvertexcount(2)]
void main(point ParticleStructure inputParticle[1],
	inout PointStream<ParticleStructure> outputParticleStream) {

	ParticleStructure			outputParticle;

	if (inputParticle[0].isaGenerator == 1) {

		//
		// Stream out geneator / emitter with a new age value
		//
		outputParticle = inputParticle[0];

		if (inputParticle[0].age >= generatorAgeThreshold) {

			// Need to reset age...
			outputParticle.age = resetAge;

		}
		else {

			// Countdown generator age to create new particle
			outputParticle.age = inputParticle[0].age + ageDelta;
		}

		outputParticleStream.Append(outputParticle);

		//
		// Create new particle if necessary
		//
		if (inputParticle[0].age >= generatorAgeThreshold) {

			outputParticle.pos = float3(
				inputParticle[0].pos.x + init_x,
				inputParticle[0].pos.y,
				inputParticle[0].pos.z + init_z);

			outputParticle.velocity = float3(
				init_x_velocity * 0.5,
				0.0,
				init_z_velocity * 0.5);

			outputParticle.weight = init_weight;

			// Initialise particle age (will count down normal particle age)
			outputParticle.age = init_age;

			outputParticle.theta = 0.0;
			outputParticle.angularVelocity = init_angular_velocity * (3.142 * 0.5);

			outputParticle.isaGenerator = 0;
			outputParticle.flakeType = randomFlakeType;

			outputParticleStream.Append(outputParticle);
		}

	}
	else {

		// Modify normal particle - discard if age == 0

		if (inputParticle[0].age > 0.0) {

			// Fixed velocity - animation speed variable wrt. hardware capability and workload on GPU - not ideal!
			//outputParticle.pos = inputParticle[0].pos + inputParticle[0].velocity * 0.005;
			//outputParticle.velocity = inputParticle[0].velocity + gravity.xyz;
			//outputParticle.theta = inputParticle[0].theta + inputParticle[0].angularVelocity * 0.25;

			// Time dependent update (basic numerical / Euler integration).  By determining a required speed per unit time we can maintain a consistent rate of animation.
			outputParticle.pos = inputParticle[0].pos + (inputParticle[0].velocity * gameTimeDelta);

			outputParticle.velocity = inputParticle[0].velocity + (gravity.xyz / inputParticle[0].weight) * gameTimeDelta;

			outputParticle.theta = inputParticle[0].theta + inputParticle[0].angularVelocity * gameTimeDelta;


			outputParticle.weight = inputParticle[0].weight;
			outputParticle.age = inputParticle[0].age - ageDelta;

			outputParticle.angularVelocity = inputParticle[0].angularVelocity;
			outputParticle.isaGenerator = inputParticle[0].isaGenerator;
			outputParticle.flakeType = inputParticle[0].flakeType;

			outputParticleStream.Append(outputParticle);
		}
	}
}
