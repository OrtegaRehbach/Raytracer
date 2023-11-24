#pragma once

#include "color.h"
#include "texture.h"

struct Material {
    Color diffuse;
    float albedo;
    float specularAlbedo;
    float specularCoefficient;
    float reflectivity;
    float transparency;
    float refractionIndex;
    Texture* texture;
};