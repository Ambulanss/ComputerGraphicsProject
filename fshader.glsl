
#version 330 core

// Interpolowane warto�ci z vertex shader
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Light2Direction_cameraspace;

// Wyj�ciowe dane
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec3 Light2Position_worldspace;

uniform vec3 LightColor = vec3(1,0,0); //0.7 1 1niebieskawo zielone �wiat�o
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
    // W�a�ciwo�ci materia�u
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb; ///Kiedy �wiat�o uderza w obiekt zostaje odbite we wszystkich kierunkach ///jest brany z tekstury
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor * 4; //razy 4 bo troch� podkr�camy ambient, akwarium stoi w pokoju a nie na dnie oceanu
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3); ///Druga cz�� �wiat�a kt�ra jest odbijana odbija si� g��wnie w kierunku, kt�ry jest odbiciem �wiat�a na powierzchni.
	///Specular - symuluje jasny punkt �wiat�a, kt�ry pojawia si� na b�yszcz�cych obiektach (bardziej kolor �wiat�a ni� obiektu)

	// Odleg�o�� do �wiat�a
	float distance = length( LightPosition - Position_worldspace ); //im dalej tym s�absze �wiat�o

	// Normalna obliczanego fragmentu, w przestrzeni kamery (camera space)
	vec3 n = normalize( Normal_cameraspace );
	// Kierunek �wiat�a (od fragmentu do �wiat�a)
	vec3 l = normalize( LightDirection );

	///Aby obliczy� kolor pixela k�t pomi�dzy �wiat�em, a normaln� powierzchni ma znaczenie - je�eli �wiat�o jest prostopad�e do powierzchni
	///jest wtedy skoncentrowane, a je�eli pod k�tem, to ta sama ilo�� �wiat�a rozbija si� na wi�kszej powierzchni
	// Kosinus k�ta pomi�dzy normaln�, a kierunkiem �wiat�a
	// clamped above 0 //�eby �wiat�o kt�re jest za tr�jk�tem, nie dawa�o jakich� ujemnych warto�ci koloru
	//  - �wiat�o znajduje si� w pionie tr�jk�ta -> 1
	//  - �wiat�o jest prostopad�e do tr�jk�ta -> 0
	//  - �wiat�o jest za tr�jk�tem -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Wektor oczu (w kierunku kamery)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Kierunek w kt�rym tr�jk�t odbija �wiat�o
	vec3 R = reflect(-l,n);
	// Kosinus k�ta pomi�dzy wektorem oka (Eye vector) a wektorem odbicia (Reflect vector),
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
