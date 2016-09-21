varying vec4 diffuse,ambient;
varying vec3 normal,halfVector;

void main()
{
	normal = normalize(gl_NormalMatrix * gl_Normal);
	halfVector = gl_LightSource[0].halfVector.xyz;

	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	gl_Position = ftransform();
}