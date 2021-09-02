#version 450 core

in vec3 FragPos;
in vec3 Normal;

in vec2 o_textCoords;
out vec4 color;

uniform sampler2D u_texture;
uniform sampler2D trailTexture;
uniform sampler2D fovTexture;
uniform mat4 ambiantLight; 
uniform vec3 viewPos;

struct Light
{
	vec3 position;
	vec4 color;
	vec4 values;
};

Light Mat4ToLight(mat4 mat)
{
	Light l;

	l.position = vec3(mat[0]);
	l.color = mat[1];
	l.values = mat[2];
		
	return l;
}


vec2 rotateUV(vec2 uv, float rotation)
{
    float mid = 0.5;
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}


uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)); }

float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

vec4 computeTrail(vec2 uv)
{
	vec4 color = vec4(0);
	int size=7;
	for(int x = -size; x< size; x++)
	for(int y = -size; y< size; y++)
	{
		float d = clamp(1- distance(vec2(x,y), vec2(0,0)) /size, 0,1);
		color += texture(trailTexture, uv + vec2(x*0.00033*.25,y*0.00033*.25) ) *  d;
	}
	color /= size*size;

	return color;
}

float computeAo(vec2 uv)
{
	float aO= 0;
	int size=10;
	for(int x = -size; x< size; x++)
	for(int y = -size; y< size; y++)
	{
		float d = clamp(1- distance(vec2(x,y), vec2(0,0)) /size, 0,1);
		aO += texture(u_texture, uv + vec2(x*0.001 *.25,y*0.001*.25) ).r *  d;
	}
	aO /= size*size;

	return aO;
}

vec3 ComputeNormal(vec2 uv)
{
    vec4 h;
    float size = 0.00033;
    h[0] = computeTrail(uv + vec2(0, -1 * size)).r;
	h[1] = computeTrail(uv + vec2(-1 * size,0)).r;
    h[2] = computeTrail(uv + vec2( 1 * size, 0)).r;
    h[3] = computeTrail(uv + vec2(0, 1 * size)).r;

    vec3 n;
    n.z = h[2] - h[1];
	n.x =h[3] - h[0];
    n.y = 1;
    return normalize(n);
}

float shadow(vec2 uv, vec3 lightDir)
{
	float size = 0.0001;
	vec2 lDir = vec2(lightDir.z, lightDir.x);
	float color = 0;
	for(int i = 0; i < 500; i++)
	{
		 color += texture(u_texture, uv +lDir * size * i).r;
	}

	return step(color.r, 0);
}

void main()
{

	float fog = clamp(distance(viewPos, FragPos)/150,0,1);
	vec3 norm = normalize(Normal);

	// Grain
	int sandSize= 35000;
	float rx= random( vec2(round(o_textCoords.y*sandSize)/sandSize , round(o_textCoords.x*sandSize)/sandSize));
	float ry= random( vec2( round(o_textCoords.x *sandSize)/sandSize ,round(o_textCoords.y*sandSize)/sandSize ));
	norm += vec3(rx, 0, ry) *.2 * (1-fog);

	// Sand Normal
	norm += ComputeNormal(o_textCoords) ;
	norm =  normalize(norm);
	norm -= vec3(0, sin(rotateUV(o_textCoords,-.15).x*5000 ) *.1,0);
	norm -= vec3(0, sin(rotateUV(o_textCoords,.1).x*2500) *.2,0);
	norm -= vec3(0, sin(o_textCoords.x*1000) *.3,0);
								 

	// Basic Light Setup
		Light light = Mat4ToLight(transpose(ambiantLight));
	vec3 ambiant = light.values[0] * vec3(light.color);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(light.position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(light.color);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	float fresnel = clamp(pow(1-dot(viewDir, norm),10),0, 1) *.2;
	vec3 specular = vec3(light.values[1] * spec) *.6; 

	// Color
	color = vec4(0,0,0,0);
	vec3 result = vec3(color) * (ambiant + diffuse + specular);
	vec4 sandColor = vec4(222/255.0,188/255.0, 158/255.0, 1);

	// Shadows
	float shdw =shadow(o_textCoords, lightDir);

	specular *= shdw;
	diffuse *= shdw;
	color = sandColor * vec4(ambiant + diffuse + specular ,1)  + fresnel; 

	color -= (1-shdw)*.1;

	// FoV & Light
	float fov = 0;
	int size = 8;
	float pSize = 0.001*.25;
	for(int x = -size; x < size; x++)
	{
		for(int y = -size; y < size; y++)
		{
				 fov +=	texture(fovTexture, o_textCoords + vec2(x * pSize, y* pSize)).r;
		}
	}
	fov /= (size*2 * size * 2);
	vec4 lightColor =vec4(241, 122, 238, 255)/255.0;
	color += vec4(fov) * lightColor *.5;

	// Ambiant Occlusion
	float aO = computeAo(o_textCoords);
	color -= aO *.2;


	//Fog
	color= mix(color, vec4(0,0,.5,1), fog);


}