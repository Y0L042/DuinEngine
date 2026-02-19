#include "TestSceneBuilderCommon.h"

namespace TestSceneBuilder
{
const std::string FULL_ENTITY_JSON = R"({
"uuid": "f5a3e8d1c9b2a7f4",
"name": "PlayerCharacter",
"enabled": true,
"tags": [
  {"type": "player"},
  {"type": "controllable"},
  {"type": "persistent"}
],
"pairs": [
  {
    "relationship": "IsA",
    "target": "CharacterBase"
  }
],
"components": [
    {
      "type": "Transform",
      "position": {"x": 10.5, "y": 0.0, "z": -3.2},
      "rotation": {"x": 0.0, "y": 45.0, "z": 0.0},
      "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
    },
    {
      "type": "RigidBody",
      "mass": 75.0,
      "friction": 0.5,
      "restitution": 0.2,
      "kinematic": false
    },
    {
      "type": "MeshRenderer",
      "meshPath": "assets/models/character.mesh",
      "materialPath": "assets/materials/character_skin.mat",
      "castShadows": true
    }
  ],
  "children": [
    {
      "uuid": "1a2b3c4d5e6f7890",
      "name": "Camera",
      "enabled": true,
      "tags": [{"type": "camera"}],
      "pairs": [],
      "components": [
        {
          "type": "Transform",
          "position": {"x": 0.0, "y": 1.8, "z": 0.5},
          "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
          "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        },
        {
          "type": "Camera",
          "fov": 75.0,
          "nearPlane": 0.1,
          "farPlane": 1000.0,
          "isPrimary": true
        }
      ],
      "children": []
    },
    {
      "uuid": "9876543210abcdef",
      "name": "WeaponSlot",
      "enabled": false,
      "tags": [{"type": "equipment"}],
      "pairs": [],
      "components": [
        {
          "type": "Transform",
          "position": {"x": 0.3, "y": 1.2, "z": 0.4},
          "rotation": {"x": 0.0, "y": 90.0, "z": 0.0},
          "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        }
      ],
      "children": []
    }
  ]
})";

const std::string SIMPLE_ENTITY_JSON = R"(   
{
    "uuid": "1a2b3c4d5e6f7890",
    "name": "Camera",
    "enabled": true,
    "tags": [{"type": "camera"}],
    "pairs": [
        {
            "relationship": "IsA",
            "target": "CameraArchetype"
        }
    ],
    "components": 
    [
        {
            "type": "Transform",
            "position": {"x": 0.0, "y": 1.8, "z": 0.5},
            "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
            "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        },
        {
            "type": "Camera",
            "fov": 75.0,
            "nearPlane": 0.1,
            "farPlane": 1000.0,
            "isPrimary": true
        }
    ],
    "children": []
})";

} // namespace TestSceneBuilder
