#version 450 core 

#pragma compile(off)

layout(binding = 0) uniform sampler2D tHeightMap;


uniform vec3 dimensions;
smooth in vec2 vUv;
smooth in vec2 vStep;
uniform float dz = 0;
uniform float invertR = 1;
uniform float invertG = 1;
uniform float invertH = 1;
uniform int type;

void main(void) {
	vec2 tlv = vec2(vUv.x - vStep.x, vUv.y + vStep.y );
	vec2 lv  = vec2(vUv.x - vStep.x, vUv.y 		   );
	vec2 blv = vec2(vUv.x - vStep.x, vUv.y - vStep.y);
	vec2 tv  = vec2(vUv.x 		  , vUv.y + vStep.y );
	vec2 bv  = vec2(vUv.x 		  , vUv.y - vStep.y);
	vec2 trv = vec2(vUv.x + vStep.x, vUv.y + vStep.y );
	vec2 rv  = vec2(vUv.x + vStep.x, vUv.y 		   );
	vec2 brv = vec2(vUv.x + vStep.x, vUv.y - vStep.y);
	tlv = vec2(tlv.x >= 0.0 ? tlv.x : (1.0 + tlv.x), 	tlv.y >= 0.0	? tlv.y : (1.0  + tlv.y));
	tlv = vec2(tlv.x < 1.0  ? tlv.x : (tlv.x - 1.0 ), 	tlv.y < 1.0   	? tlv.y : (tlv.y - 1.0 ));
	lv  = vec2( lv.x >= 0.0 ?  lv.x : (1.0 + lv.x),  	lv.y  >= 0.0 	?  lv.y : (1.0  +  lv.y));
	lv  = vec2( lv.x < 1.0  ?  lv.x : ( lv.x - 1.0 ),   lv.y  < 1.0  	?  lv.y : ( lv.y - 1.0 ));
	blv = vec2(blv.x >= 0.0 ? blv.x : (1.0 + blv.x), 	blv.y >= 0.0 	? blv.y : (1.0  + blv.y));
	blv = vec2(blv.x < 1.0  ? blv.x : (blv.x - 1.0 ), 	blv.y < 1.0 	? blv.y : (blv.y - 1.0 ));
	tv  = vec2( tv.x >= 0.0 ?  tv.x : (1.0 + tv.x),  	tv.y  >= 0.0 	?  tv.y : (1.0  +  tv.y));
	tv  = vec2( tv.x < 1.0  ?  tv.x : ( tv.x - 1.0 ),   tv.y  < 1.0 	?  tv.y : ( tv.y - 1.0 ));
	bv  = vec2( bv.x >= 0.0 ?  bv.x : (1.0 + bv.x),  	bv.y  >= 0.0 	?  bv.y : (1.0  +  bv.y));
	bv  = vec2( bv.x < 1.0  ?  bv.x : ( bv.x - 1.0 ),   bv.y  < 1.0 	?  bv.y : ( bv.y - 1.0 ));
	trv = vec2(trv.x >= 0.0 ? trv.x : (1.0 + trv.x), 	trv.y >= 0.0 	? trv.y : (1.0  + trv.y));
	trv = vec2(trv.x < 1.0  ? trv.x : (trv.x - 1.0 ), 	trv.y < 1.0   	? trv.y : (trv.y - 1.0 ));
	rv  = vec2( rv.x >= 0.0 ?  rv.x : (1.0 + rv.x),  	rv.y  >= 0.0 	?  rv.y : (1.0  +  rv.y));
	rv  = vec2( rv.x < 1.0  ?  rv.x : ( rv.x - 1.0 ),   rv.y  < 1.0   	?  rv.y : ( rv.y - 1.0 ));
	brv = vec2(brv.x >= 0.0 ? brv.x : (1.0 + brv.x), 	brv.y >= 0.0 	? brv.y : (1.0  + brv.y));
	brv = vec2(brv.x < 1.0  ? brv.x : (brv.x - 1.0 ), 	brv.y < 1.0   	? brv.y : (brv.y - 1.0 ));
	float tl = abs(texture2D(tHeightMap, tlv).r);
	float l  = abs(texture2D(tHeightMap, lv ).r);
	float bl = abs(texture2D(tHeightMap, blv).r);
	float t  = abs(texture2D(tHeightMap, tv ).r);
	float b  = abs(texture2D(tHeightMap, bv ).r);
	float tr = abs(texture2D(tHeightMap, trv).r);
	float r  = abs(texture2D(tHeightMap, rv ).r);
	float br = abs(texture2D(tHeightMap, brv).r);
   float dx = 0.0, dy = 0.0;
   if(type == 0){
		dx = tl + l*2.0 + bl - tr - r*2.0 - br;
		dy = tl + t*2.0 + tr - bl - b*2.0 - br;
   }
   else{
		dx = tl*3.0 + l*10.0 + bl*3.0 - tr*3.0 - r*10.0 - br*3.0;
		dy = tl*3.0 + t*10.0 + tr*3.0 - bl*3.0 - b*10.0 - br*3.0;
   }
	vec4 normal = vec4(normalize(vec3(dx * invertR * invertH * 255.0, dy * invertG * invertH * 255.0, dz)), texture2D(tHeightMap, vUv).a);
	gl_FragColor = vec4(normal.xy * 0.5 + 0.5, normal.zw);
}