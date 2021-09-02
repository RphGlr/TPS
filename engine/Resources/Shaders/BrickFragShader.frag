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

vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}


float snoise(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

vec2 rotateUV(vec2 uv, float rotation)
{
    float mid = 0.5;
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}


float brickSDF(vec2 uv, float noiseOffset)
{
		float brickSDF = smoothstep(0.0, .15, uv.y);
	   brickSDF *= smoothstep(0.0, .15, 1-uv.y);
	   brickSDF *= smoothstep(0.0, .15, 1-uv.x);
	   brickSDF *= smoothstep(0.0, .15, uv.x);

	   return mix(brickSDF , 0, noiseOffset);
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 ComputeNormal(vec2 uv, vec3 norm, float noiseOffset)
{
    vec4 h;
    float size = 0.05;
    h[0] = brickSDF(uv + vec2(0, -1 * size), noiseOffset).r;
	h[1] = brickSDF(uv + vec2(-1 * size,0), noiseOffset).r;
    h[2] = brickSDF(uv + vec2( 1 * size, 0), noiseOffset).r;
    h[3] = brickSDF(uv + vec2(0, 1 * size), noiseOffset).r;

    vec3 n;
    n.z = h[3] - h[0];
    n.x = h[2] - h[1];
    n.y = 1;

	float maskX = 1 - step(norm.x, 0); // X Mask
	float maskZ = 1 - step(norm.z, 0); // Z Mask

	float maskAbsoluteX = step(abs(norm.x),0);
	float maskAbsoluteZ = step(abs(norm.z),0);

	mat4 rMatrixX = rotationMatrix(vec3(1,0,0), 90 * 0.0174533  );
	mat4 rMatrixZ = rotationMatrix(vec3(0,1,0), 90 * 0.0174533  * maskAbsoluteZ);
	vec4 n1 = vec4(n,1) * rMatrixX * rMatrixZ;
    n1.b = mix(mix(n1.b, -n1.b, maskZ), -n1.b, 1 - step(0, norm.x)) ;
    n1.r = mix(mix(n1.r, -n1.r, maskX), -n1.r, 1 - step(0, norm.z)) ;
    return normalize( vec3(-n1.r, n1.g, -n1.b));
}

void main()
{


	float fov = 0;
	int size = 8;
	float pSize = 0.001*.25;
	for(int x = -size; x < size; x++)
	{
		for(int y = -size; y < size; y++)
		{
				 fov +=	texture(fovTexture, vec2(FragPos.z / 1000, FragPos.x/1000) + vec2(x * pSize, y* pSize)).r;
		}
	}


	fov /= (size*2 * size * 2);

	float fog = clamp(distance(viewPos, FragPos)/150,0,1);
	float fog2 = clamp(distance(viewPos, FragPos)/50,0,1);
	 // Triplanar Mapping

	vec3 norm = normalize(Normal);

	float maskX = step(abs(norm.x), .1); // X Mask
	float maskZ = step(abs(norm.z), .1); // Z Mask
	vec3 wPos = FragPos ;

	float brickOffset = step(0,(sin(wPos.y *3.141592))) *.5; 

	float resultX = fract(wPos.x *.5 + brickOffset);
	float resultZ = fract(wPos.z *.5 + brickOffset);

	// Compute UV
	vec2 uv = vec2(resultX * maskX  + resultZ * maskZ,  fract(wPos.y));
	uv = mix(uv, vec2(1-uv.x,uv.y), maskZ);
	float mask = step(normalize(Normal).x,0) * step(normalize(Normal).z,0);
	uv = mix( uv, vec2(1-uv.x,uv.y), mask);

	float noise = snoise( round( vec3(  (wPos.x  *.5+ brickOffset+.5) * maskX  , wPos.y   +.5, (wPos.z *.5  + brickOffset +.5)* maskZ ) ));
	float noiseOffset = abs(sin( _time*.001 + round(wPos.y +.5))) * abs(noise);

	vec3 brickUV = ComputeNormal(uv, norm, noiseOffset);
	
 	norm = brickUV ;
	norm = normalize(norm);

	float bSDF = brickSDF(uv, noiseOffset);

	Light light = Mat4ToLight(transpose(ambiantLight));

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 ambiant = light.values[0] * vec3(light.color);
	vec3 lightDir = normalize(light.position - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(light.color);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = light.values[1] * spec * vec3(light.color);  

	color = vec4(0,0,0,0);


	vec4 fresnelColor = vec4(138/255.0,176/255.0,234/255.0,1);
	
	vec4 sandColor = vec4(222/255.0,188/255.0, 158/255.0, 1);
	float fr = clamp(1 -dot(viewDir, norm), 0,1);
	fr = pow(fr,3) *.5;
	color = mix(sandColor*.5,sandColor, bSDF) * vec4(ambiant+ specular * bSDF + diffuse + specular * bSDF ,1) +fresnelColor * fr  ; 

	vec4 skyColor = vec4(0,0,.5,1);
	vec4 skyColor2 = vec4(.1,.7,.8,1);


	float s= step( bSDF,.1);
	color= mix(color + vec4(241, 122, 238, 255)/255.0 * s, color, fog2);

	float n = clamp(abs(snoise(FragPos *.2 + vec3(0,-_time*0.001, 0))) *.5,0,1);
	float yGradient = clamp(1-(FragPos.y/20) , 0,1) ;
	yGradient -= n * FragPos.y /20;
	vec4 lightColor =vec4(241, 122, 238, 255)/255.0;
	color += fov * step(0.5, yGradient) * lightColor*.5  ;
	color += fov * clamp(yGradient,0,1)* lightColor  ;

	float aO = 1-clamp(smoothstep(0,1,FragPos.y),0,1);
	color -= aO *.1;
	
	color= mix(color, 	skyColor, fog);



}