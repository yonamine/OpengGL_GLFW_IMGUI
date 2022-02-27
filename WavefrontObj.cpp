#include "WavefrontObj.hpp"

#include <iostream>

/*

Vertex data:
    v    Geometric vertices:            v  x y z
    vt   Texture vertices:              vt u v
    vn   Vertex normals:                vn dx dy dz

Elements:
    p    Point:                         p v1
    l    Line:                          l v1 v2 ... vn
    f    Face:                          f v1 v2 ... vn
    f    Face with texture coords:      f v1/t1 v2/t2 .... vn/tn
    f    Face with vertex normals:      f v1//n1 v2//n2 .... vn//nn
    f    Face with txt and norms:       f v1/t1/n1 v2/t2/n2 .... vn/tn/nn

Grouping:
    g    Group name:                    g groupname

Display/render attributes:
    usemtl     Material name:           usemtl materialname
    mtllib     Material library:        mtllib materiallibname.mtl


 */

WavefrontObject::WavefrontObjectKeyword WavefrontObject::ConvertToKeyword(
    const std::string& keyword) const {
  if (keyword.empty()) {
    return WavefrontObjectKeyword::EmptyLine;
  } else if (keyword == kWavefrontKeywordCommend) {
    return WavefrontObjectKeyword::Comment;
  } else if (keyword == kWavefrontKeywordName) {
    return WavefrontObjectKeyword::Name;
  } else if (keyword == kWavefrontKeywordGeometricVertex) {
    return WavefrontObjectKeyword::GeometricVertex;
  } else if (keyword == kWavefrontKeywordVertexNormal) {
    return WavefrontObjectKeyword::VertexNormal;
  } else if (keyword == kWavefrontKeywordFace) {
    return WavefrontObjectKeyword::Face;
  }

  return WavefrontObjectKeyword::Invalid;
}

glm::vec4 WavefrontObject::ParsePoints(std::stringstream& ss) {
  std::string token{};
  std::array<float, kMaxComponents> points{0.0f, 0.0f, 0.0f, 1.0f};
  size_t index{0};

  while ((std::getline(ss, token, kTokenDelimiter)) &&
         (index < kMaxComponents)) {
    if (token.empty()) {
      continue;
    }

    try {
      points[index] = std::stof(token);
    } catch (const std::invalid_argument& e) {
      printf("Conversion Error: '%s'\n", e.what());
      abort();  // @TODO Change to better mechanism
    } catch (const std::out_of_range& e) {
      printf("Conversion Error: '%s'\n", e.what());
      abort();  // @TODO Change to better mechanism
    }

    index++;
  }

  return glm::vec4(points[0], points[1], points[2], points[3]);
}

bool WavefrontObject::LoadFile(const std::string& filename) {
  std::fstream file{};

  file.open(filename, std::ios::in);
  if (!file.is_open()) {
    // @TODO Add error message: File does not exist
    return false;
  }

  std::string line{};
  while (std::getline(file, line)) {
    std::string token{};
    std::stringstream ss{line};

    std::getline(ss, token, ' ');
    std::cout << "===== " << token << '\n';
    auto keyword = ConvertToKeyword(token);
    switch (keyword) {
      case WavefrontObjectKeyword::EmptyLine:
      case WavefrontObjectKeyword::Comment:
        break;
      case WavefrontObjectKeyword::Name:
        std::getline(ss, token, ' ');
        name_ = token;
        break;
      case WavefrontObjectKeyword::GeometricVertex: {
        // v  0.0  0.0  0.0
        view_matrix_ = ParsePoints(ss);
        std::cout << "view_matrix_: " << glm::to_string(view_matrix_) << '\n';
        break;
      }
      case WavefrontObjectKeyword::VertexNormal: {
        while (std::getline(ss, token, ' '))
          ;
        break;
      }
      case WavefrontObjectKeyword::Face: {
        while (std::getline(ss, token, ' '))
          ;
        break;
      }
      case WavefrontObjectKeyword::Invalid:
      default:
        std::cout << "Invalid token: '" << token << " - " << token.length()
                  << "'\n";
        return false;
    }
  }

  return true;
}
