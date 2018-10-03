
#version 330 core

// Interpolowane wartoœci z vertex shader
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Light2Direction_cameraspace;

// Wyjœciowe dane
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec3 Light2Position_worldspace;

uniform vec3 LightColor = vec3(1,0,0); //0.7 1 1niebieskawo zielone œwiat³o
uniform vec3 Light2Color = vec3(0,1,0);

uniform float LightPower = 150.0f;
uniform float Light2Power = 150.0f;


// Prototypy funkcji
vec3 CalcPointLight(vec3 LightPosition, vec3 LightDirection,vec3 LColor, float LPower);


void main(){

            color+=CalcPointLight(LightPosition_worldspace,LightDirection_cameraspace,LightColor,LightPower)
            +CalcPointLight(Light2Position_worldspace,Light2Direction_cameraspace,Light2Color,Light2Power);
}

vec3 CalcPointLight(vec3 LightPosition, vec3 LightDirection, vec3 LColor, float LPower)
{
    // W³aœciwoœci materia³u
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb; ///Kiedy œwiat³o uderza w obiekt zostaje odbite we wszystkich kierunkach ///jest brany z tekstury
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor * 4; //razy 4 bo trochê podkrêcamy ambient, akwarium stoi w pokoju a nie na dnie oceanu
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3); ///Druga czêœæ œwiat³a która jest odbijana odbija siê g³ównie w kierunku, który jest odbiciem œwiat³a na powierzchni.
	///Specular - symuluje jasny punkt œwiat³a, który pojawia siê na b³yszcz¹cych obiektach (bardziej kolor œwiat³a ni¿ obiektu)

	// Odleg³oœæ do œwiat³a
	float distance = length( LightPosition - Position_worldspace ); //im dalej tym s³absze œwiat³o

	// Normalna obliczanego fragmentu, w przestrzeni kamery (camera space)
	vec3 n = normalize( Normal_cameraspace );
	// Kierunek œwiat³a (od fragmentu do œwiat³a)
	vec3 l = normalize( LightDirection );

	///Aby obliczyæ kolor pixela k¹t pomiêdzy œwiat³em, a normaln¹ powierzchni ma znaczenie - je¿eli œwiat³o jest prostopad³e do powierzchni
	///jest wtedy skoncentrowane, a je¿eli pod k¹tem, to ta sama iloœæ œwiat³a rozbija siê na wiêkszej powierzchni
	// Kosinus k¹ta pomiêdzy normaln¹, a kierunkiem œwiat³a
	// clamped above 0 //¿eby œwiat³o które jest za trójk¹tem, nie dawa³o jakichœ ujemnych wartoœci koloru
	//  - Œwiat³o znajduje siê w pionie trójk¹ta -> 1
	//  - œwiat³o jest prostopad³e do trójk¹ta -> 0
	//  - œwiat³o jest za trójk¹tem -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Wektor oczu (w kierunku kamery)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Kierunek w którym trójk¹t odbija œwiat³o
	vec3 R = reflect(-l,n);
	// Kosinus k¹ta pomiêdzy wektorem oka (Eye vector) a wektorem odbicia (Reflect vector),
	// clamped to 0
	//  - Patrzenie w odbicie -> 1
	//  - Patrzenie gdzie indziej -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );


	///Blinn
	float spec = 0.0;
	vec3 lightDir   = normalize(LightPosition - Position_worldspace);
    vec3 viewDir    = normalize(EyeDirection_cameraspace - Position_worldspace);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(Normal_cameraspace, halfwayDir), 0.0), 32.0);


	vec3 resault =
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LColor * LPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LColor * LPower * pow(cosAlpha,5) / (distance*distance);//pow(cosAlpha,5)
    return resault;
}
