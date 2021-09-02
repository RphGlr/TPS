#version 450 core

in vec3 _FragPos;
flat in vec3 _Normal;
in vec3 _smoothNormal;

in vec2 o_textCoords;

flat in float lightAmount;

out vec4 color;

uniform sampler2D u_texture;
uniform sampler2D trailTexture;
uniform sampler2D fovTexture;
uniform mat4 ambiantLight; 
uniform vec3 viewPos;
uniform float _time;


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
void main()
{
	color = vec4(0,0,0,0);
	vec3 norm = normalize(_Normal);

    float sandSize = 10;


	Light light = Mat4ToLight(transpose(ambiantLight));

	vec3 viewDir = normalize(viewPos - _FragPos);
	vec3 ambiant = light.values[0] * vec3(light.color);
	vec3 lightDir = normalize(light.position - _FragPos);

	float diff = max(dot(norm, lightDir), 0.0); 
	vec3 diffuse = diff * vec3(light.color);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = light.values[1] * spec * vec3(light.color);  

	vec4 fresnelColor = vec4(138/255.0,176/255.0,234/255.0,1);


	float fresnel = clamp(1 -dot(viewDir, norm), 0,1);
    fresnel = pow(fresnel,2);

    float smoothFresnel =  clamp(dot(viewDir, normalize(_smoothNormal  )), 0,1);
    smoothFresnel = pow(smoothFresnel,2);

    
    float heartbeat =clamp(smoothstep(0.9,1., sin(_time*.004)), 0,1);
     heartbeat += clamp(smoothstep(0.9,1., sin((_time+200)*.004)), 0,1);
     heartbeat = clamp(heartbeat,0,1);


   vec4 gradient = vec4(80, 4, 151, 255)/255.0;


   
	float fov = 0;
	int size = 8;
	float pSize = 0.001*.25;
	for(int x = -size; x < size; x++)
	{
		for(int y = -size; y < size; y++)
		{
				 fov +=	texture(fovTexture, vec2(_FragPos.z / 1000, _FragPos.x/1000) + vec2(x * pSize, y* pSize)).r;
		}
	}


	fov /= (size*2 * size * 2);

	    float s = abs(sin( _time *.005 + _FragPos.y));

		vec4 c1 = vec4(241, 122, 238, 255)/255.0;
		vec4 c2 = vec4(255, 0, 0, 255) / 255.0;


	gradient = mix(vec4(0),vec4(1), s);
	gradient = mix(vec4(0),  vec4(142, 60, 122, 255)/255.0, fresnel);
	
	vec4 sandColor = vec4(222/255.0,188/255.0, 158/255.0, 1);
	fresnel = pow(fresnel,1) *.5;

	color = gradient * vec4(vec3(1,1,1)+ specular  + diffuse *.4  ,1)  ; 

	vec4 skyColor = vec4(0,0,.5,1);

	float fog = clamp(distance(viewPos,_FragPos)/150,0,1);
		color= mix(color, skyColor, fog);









}