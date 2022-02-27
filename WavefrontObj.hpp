/**
 *
 *
 * References:
 * 1. http://paulbourke.net/dataformats/obj/
 * 2. https://en.wikipedia.org/wiki/Wavefront_.obj_file
 * 3.
 * https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html
 * 4. https://www.fileformat.info/format/wavefrontobj/egff.htm
 *
 *
 */
#pragma once

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <string>
#include <vector>

class WavefrontObject {
 public:
  enum class WavefrontObjectKeyword {
    // General
    Comment,    // #
    Name,       // g
    EmptyLine,  // ''
    // Vertex Data
    GeometricVertex,         // v
    TextureVertex,           // vt
    VertexNormal,            // vn
    ParameterSpaceVertices,  // vp
    // Elements
    Point,    // p
    Line,     // l
    Face,     // f
    Curve,    // curv
    Curve2d,  // curv2
    Surface,  // surf
    Invalid,  // Invalid Keyword
  };

  bool LoadFile(const std::string &filename);

  // friend std::ostream &operator<<(std::ostream &out, const glm::vec4 &vec);

 private:
  static const std::string kWavefrontKeywordCommend;
  static const std::string kWavefrontKeywordName;
  static const std::string kWavefrontKeywordGeometricVertex;
  static const std::string kWavefrontKeywordVertexNormal;
  static const std::string kWavefrontKeywordFace;
  static constexpr char kTokenDelimiter{' '};
  static constexpr uint64_t kMaxComponents{4};

  std::string name_{};
  glm::vec4 view_matrix_{0.0f};
  glm::mat4 geometric_vertice_{0.0f};
  glm::mat4 texture_vertice_{0.0f};
  glm::mat4 vertex_normals_{0.0f};
  glm::mat4 face_{0.0f};

  WavefrontObjectKeyword ConvertToKeyword(const std::string &keyword) const;
  glm::vec4 ParsePoints(std::stringstream &ss);
};

const std::string WavefrontObject::kWavefrontKeywordCommend{"#"};
const std::string WavefrontObject::kWavefrontKeywordName{"g"};
const std::string WavefrontObject::kWavefrontKeywordGeometricVertex{"v"};
const std::string WavefrontObject::kWavefrontKeywordVertexNormal{"vn"};
const std::string WavefrontObject::kWavefrontKeywordFace{"f"};

// std::ostream &operator<<(std::ostream &out, const glm::vec4 &vec) {
//    out << "{" << vec.x << " " << vec.y << " "<< vec.z  << "}";
// out << "view_matrix_: " << glm::to_string(vec);
//    return out;
//}
