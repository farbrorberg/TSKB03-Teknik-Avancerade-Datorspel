#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform Light light;

out vec4 out_Color;

struct Spotlight {

    //Light distances
    vec3 direction, position;
    float cutOff, outerCutOff;

    //Light props
    vec3 specular, diffuse, ambient;

    //Spotlight props
    float constant, linear, quadratic;
};

void main() {

    //Ambient
    vec3 ambient = light.ambient * lightColor; //lightColor = material ambient prop

    //Diffuse
    vec3 norm = normalize(in_Normal);
    vec3 lightDir = normalize(light.position - in_Position);
//    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(norm,lightDir));
    vec3 diffuse = light.diffuse * diff * lightColor; //lightColor = material diffuse prop

    //Specular
    vec3 viewDir = normalize(viewPos - in_Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5); //0.5 = material shinyness
    vec3 specular = light.specular * spec * lightColor; //lightColor = material specular prop

    //Spotlight
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp ((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    //Attenuation
    float distance = length(light.position - in_Position);
    float attenuation = 1.0f / (
        light.constant + light.linear * light.distance +
        light.quadratic * (distance * distance)
    );

    ambient *= attenuation;
    diffuse*= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

	out_Color = vec4(result, 1.0);
}
