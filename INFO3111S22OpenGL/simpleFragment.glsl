#version 420

//in vec3 color;
//in vec4 vertexWorldPosition;

in vec4 fColour;
in vec4 fNormal;
in vec4 fUV_x2;
in vec4 vertexWorldPosition;

out vec4 outputColour;		// RGBA (A = alpha from 0.0 to 1.0)

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};


const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

//const int NUMBEROFLIGHTS = 10;
const int NUMBEROFLIGHTS = 20;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uni

uniform vec3 eyeLocation;

// If true, skip lighting calculation
// 0 = false, not 0 is true
uniform bool bDisableLighting;	
// Default to zero (i.e. if we DON'T set this, it'll use the texture as colour)
uniform bool bDontUseTextureColour;

uniform sampler2D texture01;
uniform sampler2D texture02; 
uniform sampler2D texture03; 
uniform sampler2D texture04; 
// These aren't completely set up, yet... (that's for you to figure out)
uniform sampler2D texture05;
uniform sampler2D texture06; 
uniform sampler2D texture07; 
uniform sampler2D texture08; // Note you can't have an array of texture because that's The Way It Is
//uniform sampler2D mytextures[4];
// There is a "sampler array" but it's not what you think... 
//uniform sampler2DArray myTextures;

uniform bool bUseStencil;
uniform sampler2D stencilTexture01;


//uniform float texture01Ratio;
//uniform float texture02Ratio;
//uniform float texture03Ratio;
//uniform float texture04Ratio;
uniform vec4 texture01to04Ratio;  // x is #0, y is #1, and so on
uniform vec4 texture05to08Ratio;  // x is #0, y is #1, and so on


// function signature, just like C or C++
vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

void main()
{
	// FragColor is depricated
	//gl_FragColor = vec4(color, 1.0);


//	outputColour.rgb = fNormal.xyz;
//	outputColour = vec4( 0.0f, 1.0f, 0.0f, 1.0f );

	if (bDisableLighting)
	{
		outputColour = fColour;
		// Early exit
		return;
	}
	
	// Discard transparency example
	if ( bUseStencil )
	{
//		vec3 stencil01Colour = texture( stencilTexture01, fUV_x2.xy ).rgb;  
		// This will be black or white. 
		// So we could average the samples colour, or we could only sample part of it
		float stencil01Colour = texture( stencilTexture01, fUV_x2.xy ).r; 
		
		// is it black or white
		if ( stencil01Colour < 0.5f) 
		{	// Let's say that's black
			discard;
		}
		else
		{	// else it's white (> 0.5f)
		}			
		
	}//if ( bUseStencil )

	
	vec3 vertexColour = vec3(0.0f, 0.0f, 0.0f);
	
	if (bDontUseTextureColour)
	{
		// Redundant, but stresses that we AREN'T using the texture colour
		vertexColour.rgb = fColour.rgb;
	}
	else
	{
	
		// Texture stuff (sampling)
		vec3 texture01Colour = texture( texture01, fUV_x2.xy ).rgb;  
		vec3 texture02Colour = texture( texture02, fUV_x2.xy ).rgb;	
		vec3 texture03Colour = texture( texture03, fUV_x2.xy ).rgb;	
		vec3 texture04Colour = texture( texture04, fUV_x2.xy ).rgb;	
		
//		vertexColour.rgb = texture01Colour.rgb;

//		vertexColour.rgb =   (texture01Colour.rgb * texture01Ratio) 
//						   + (texture02Colour.rgb * texture02Ratio) 
//						   + (texture03Colour.rgb * texture03Ratio) 
//						   + (texture04Colour.rgb * texture04Ratio) 

		vertexColour.rgb =   (texture01Colour.rgb * texture01to04Ratio.x) 
						   + (texture02Colour.rgb * texture01to04Ratio.y) 
						   + (texture03Colour.rgb * texture01to04Ratio.z) 
						   + (texture04Colour.rgb * texture01to04Ratio.w); 
	}
	
//	outputColour.rgb = texture01SampledPixelColour.rgb;
//	outputColour.a = fColour.a;
//	return;
	
	
	// Else light the object
	vec4 tempSpecularVertexColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	outputColour = calcualteLightContrib( vertexColour.rgb, fNormal.xyz, 
	                                      vertexWorldPosition.xyz, tempSpecularVertexColour );
										  
	// Copy over the alpha transparency value
	outputColour.a = fColour.a;
}



vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
			
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
					   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb * lightSpecularContrib.rgb );
	}//for(intindex=0...
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}
