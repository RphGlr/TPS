#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


uniform mat4 model;
uniform mat4 viewProj;
uniform vec3 playerDir;

in VS_OUT {
 vec3 FragPos;  
 vec3 Normal;
 vec3 smoothNormal;
 vec2 o_textCoords;
} gs_in[];


out vec3 _FragPos;  
out vec3 _Normal;
out vec3 _smoothNormal;
out vec2 _o_textCoords;

uniform float _time;
uniform vec3 _direction;


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

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 worldPos, vec3 normal, vec4 Center)
{
    float magnitude = 0.3;
    float n = snoise(worldPos*.2 + vec3(0,_time*.001,0));
    float s = abs(sin( _time *.001 + n));
    s = smoothstep(0.8,1., s);
    vec3 direction = normal * s  * magnitude; 
    vec4 newPos = mix(position,  position - vec4(direction,0) ,s);
    return vec4(viewProj * model * newPos);
} 

void main() {    
   vec3 normal = GetNormal();

   vec4 center = gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position;
   center /= 3;

   gl_Position = explode(gl_in[0].gl_Position, gs_in[0].FragPos, normal, center);
   _FragPos = gs_in[0].FragPos;
    _Normal = gs_in[0].Normal;
    _smoothNormal = gs_in[0].smoothNormal;
    _o_textCoords= gs_in[0].o_textCoords;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, gs_in[1].FragPos, normal, center);
    _FragPos = gs_in[1].FragPos;
    _Normal = gs_in[1].Normal;
    _smoothNormal = gs_in[1].smoothNormal;
    _o_textCoords= gs_in[1].o_textCoords;
     EmitVertex();

     gl_Position = explode(gl_in[2].gl_Position, gs_in[2].FragPos, normal, center);
    _FragPos = gs_in[2].FragPos;
    _Normal = gs_in[2].Normal;
    _smoothNormal = gs_in[2].smoothNormal;
    _o_textCoords= gs_in[2].o_textCoords;
    EmitVertex();

    EndPrimitive();
}  