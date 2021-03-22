#pragma once
#include <glm/glm.hpp>
struct CubeUV {
    int id;
    glm::vec2 uv;
};


inline CubeUV XYZ2CubeUV(const glm::vec3& p) {
	float ax = std::abs(p.x);
	float ay = std::abs(p.y);
	float az = std::abs(p.z);
	CubeUV c;
	if (ax >= ay && ax >= az)	// x face
	{
		c = { p.x >= 0 ? 0 : 1, glm::vec2(-p.z / p.x, p.y / ax) };
	}
	else if (ay >= az)	// y face
	{
		c = { p.y >= 0 ? 2 : 3, glm::vec2(p.x / ay, -p.z / p.y) };
	}
	else // z face
	{
		c = { p.z >= 0 ? 4 : 5, glm::vec2(p.x / p.z, p.y / az) };
	}
	c.uv.x = c.uv.x * 0.5f + 0.5f;
	c.uv.y = c.uv.y * 0.5f + 0.5f;
	return c;
}
class SHSampler {
public:
    SHSampler(float* data, int width);
    ~SHSampler();

    glm::vec3 Sample(glm::vec2 uv) const;

private:
    int _width;
    glm::vec3* _data;
};