#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NUM_LIGHTS 4

uniform vec3 lightPos[NUM_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture1;

uniform bool isSun;

void main()
{
    vec3 result;

    if (isSun)
    {
        // Emissive lighting for the sun
        vec3 emission = texture(texture1, TexCoords).rgb;
        result = emission;
    }
    else
    {
        //moc slonca / swiatla
        vec3 ambient = 0.3 * texture(texture1, TexCoords).rgb;
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);
        
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        float specularStrength = 0.1;
        
        for (int i = 0; i < NUM_LIGHTS; ++i)
        {
            // Diffuse
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse += diff * texture(texture1, TexCoords).rgb;
            
            // Specular
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            specular += specularStrength * spec * vec3(1.0);
        }

        result = ambient + diffuse / NUM_LIGHTS + specular / NUM_LIGHTS;
    }

    FragColor = vec4(result, 1.0);
}