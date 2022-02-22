#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace Tema2_object2D
{

    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateHealthBarFrame(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

}
