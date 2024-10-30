#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec3 cameraPos;

void main()
{
    // Sample the texture color
    vec4 texColor = texture(texture0, fragTexCoord);
    
    // Normalize the normal vector
    vec3 normal = normalize(fragNormal);
    
    // Calculate the view direction
    vec3 viewDir = normalize(cameraPos - fragPosition);
    
    // Calculate the dot product between normal and view direction
    float dotProduct = dot(normal, viewDir);
    
    // Map the dot product to an opacity value
    // When dotProduct is 1 (viewing from top), opacity is 0 (transparent)
    // When dotProduct is -1 (viewing from bottom), opacity is 1 (opaque)
    float opacity = clamp(1.0 - (dotProduct * 0.5 + 0.5), 0.0, 1.0);
    
    // Set the final color with adjusted alpha
    finalColor = vec4(texColor.rgb, texColor.a * opacity);
}
