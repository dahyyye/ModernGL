#version 330 core
out vec4 outColor;

uniform mat4 uView, uProj, uInvView, uInvProj;
uniform vec2 uResolution;

uniform int  uShape;
uniform vec3 uMove;
uniform float uSphereR;
uniform vec3  uBoxB;
uniform vec2  uTorus;

uniform vec3 uLightPos;

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}
float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b + r;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}
float sdBoxFrame( vec3 p, vec3 b, float e )
{
       p = abs(p  )-b;
  vec3 q = abs(p+e)-e;
  return min(min(
      length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}
float sdCone( vec3 p, vec2 c, float h )
{
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c,h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y,-1.0);
    
  vec2 w = vec2( length(p.xz), p.y );
  vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
  vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
  float k = sign( q.y );
  float d = min(dot( a, a ),dot(b, b));
  float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
  return sqrt(d)*sign(s);
}
float sdLink( vec3 p, float le, float r1, float r2 )
{
  vec3 q = vec3( p.x, max(abs(p.y)-le,0.0), p.z );
  return length(vec2(length(q.xy)-r1,q.z)) - r2;
}
float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}
float sdCone( vec3 p, vec2 c, float h )
{
  float q = length(p.xz);
  return max(dot(c.xy,vec2(q,p.y)),-h-p.y);
}
float sdCappedTorus( vec3 p, vec2 sc, float ra, float rb)
{
  p.x = abs(p.x);
  float k = (sc.y*p.x>sc.x*p.y) ? dot(p.xy,sc) : length(p.xy);
  return sqrt( dot(p,p) + ra*ra - 2.0*ra*k ) - rb;
}
float sdCone( vec3 p, vec2 c )
{
    // c is the sin/cos of the angle
    vec2 q = vec2( length(p.xz), -p.y );
    float d = length(q-c*max(dot(q,c), 0.0));
    return d * ((q.x*c.y-q.y*c.x<0.0)?-1.0:1.0);
}





float mapSDF(vec3 p){
    p -= uMove;                         
    if(uShape==0) return sdSphere(p,uSphereR);
    if(uShape==1) return sdBox(p,uBoxB);
    if(uShape==2) return sdTorus(p,uTorus);
    return 1e9;
}

float depthFromWorld(vec3 pw){
    vec4 clip = uProj * uView * vec4(pw,1.0);
    float ndcZ = clip.z / clip.w;
    return ndcZ * 0.5 + 0.5;
}

vec3 getRayDir(vec2 fragCoord){
    vec2 ndc = (fragCoord / uResolution) * 2.0 - 1.0;
    vec4 clip = vec4(ndc, -1.0, 1.0);
    vec4 eye  = uInvProj * clip;
    eye = vec4(eye.xy, -1.0, 0.0);
    return normalize((uInvView * eye).xyz);
}
vec3 getCamPos(){ return (uInvView * vec4(0,0,0,1)).xyz; }

vec3 calcNormal(vec3 p){
    const float e = 1.5e-3;
    vec2 k = vec2(1,-1);
    return normalize(
        k.xyy * mapSDF(p + k.xyy*e) +
        k.yyx * mapSDF(p + k.yyx*e) +
        k.yxy * mapSDF(p + k.yxy*e) +
        k.xxx * mapSDF(p + k.xxx*e)
    );
}

struct Hit{ bool h; vec3 p; vec3 n; };

Hit raymarch(vec3 ro, vec3 rd){
    float t=0.0;
    const float EPS=1e-3, MIN_STEP=1e-4;
    for(int i=0;i<256;i++){
        vec3 p = ro + rd*t;
        float d = mapSDF(p);
        if(d < EPS) return Hit(true,p,calcNormal(p));
        t += max(d, MIN_STEP);
        if(t>500.0) break;
    }
    return Hit(false,vec3(0),vec3(0));
}

void main() {
    vec3 ro = getCamPos();                // 카메라 위치 (월드)
    vec3 rd = getRayDir(gl_FragCoord.xy); // 레이 방향 (월드)

    Hit h = raymarch(ro, rd);
    if (!h.h) { discard; return; }

    gl_FragDepth = depthFromWorld(h.p);

     // 카메라(점광원) 위치
    float dist = max(length(ro - h.p), 1e-6);     // 0 division 회피
    vec3  L    = (ro - h.p) / dist;               // 표면→라이트(=카메라) 방향  (헤드라이트)
    vec3  V    = (ro - h.p) / dist;               // 표면→카메라 방향
    vec3  H    = normalize(L + V);                 // Blinn-Phong half vector

    // 램버트 확산
    float diff = max(dot(h.n, L), 0.0);

    // 블린-폰 스펙러
    float specPow = 64.0;
    float spec = pow(max(dot(h.n, H), 0.0), specPow);

    // 앰비언트 + 디퓨즈 + 스펙러
    vec3 base    = vec3(0.90, 0.35, 0.25);
    vec3 ambient = 0.20 * base;                    // ← 최소 밝기 올려줌
    vec3 color   = ambient + base * diff + vec3(1.0) * spec * 0.25;

    outColor = vec4(color, 1.0);

}