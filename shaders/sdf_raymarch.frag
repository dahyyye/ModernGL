#version 330 core
out vec4 outColor;

uniform mat4 uView, uProj, uInvView, uInvProj;
uniform vec2 uResolution;

// SDF params (�̸� uMove �� ����!)
uniform int  uShape;
uniform vec3 uMove;
uniform float uSphereR;
uniform vec3  uBoxB;
uniform vec2  uTorus;

uniform vec3 uLightPos;

float sdSphere(vec3 p, float r){ return length(p) - r; }
float sdBox(vec3 p, vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x, max(q.y,q.z)), 0.0);
}
float sdTorus(vec3 p, vec2 t){
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
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
    vec3 ro = getCamPos();                // ī�޶� ��ġ (����)
    vec3 rd = getRayDir(gl_FragCoord.xy); // ���� ���� (����)

    Hit h = raymarch(ro, rd);
    if (!h.h) { discard; return; }

    gl_FragDepth = depthFromWorld(h.p);

     // ī�޶�(������) ��ġ
    float dist = max(length(ro - h.p), 1e-6);     // 0 division ȸ��
    vec3  L    = (ro - h.p) / dist;               // ǥ������Ʈ(=ī�޶�) ����  (������Ʈ)
    vec3  V    = (ro - h.p) / dist;               // ǥ���ī�޶� ����
    vec3  H    = normalize(L + V);                 // Blinn-Phong half vector

    // ����Ʈ Ȯ��
    float diff = max(dot(h.n, L), 0.0);

    // ��-�� ���巯
    float specPow = 64.0;
    float spec = pow(max(dot(h.n, H), 0.0), specPow);

    // �ں��Ʈ + ��ǻ�� + ���巯
    vec3 base    = vec3(0.90, 0.35, 0.25);
    vec3 ambient = 0.20 * base;                    // �� �ּ� ��� �÷���
    vec3 color   = ambient + base * diff + vec3(1.0) * spec * 0.25;

    outColor = vec4(color, 1.0);

}