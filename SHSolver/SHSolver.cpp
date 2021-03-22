#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>
#include <cstdio>
#include <vector>
#include <memory>

#include "SHSampler.h"
#include "SHUtils.h"

constexpr int WIDTH = 1000;
glm::vec3 hdr[WIDTH][WIDTH];
std::shared_ptr<SHSampler> sampler[6];
glm::vec3 coeffs[9];
glm::mat4 matrices[3];

void genMatrix() {

    /* Form the quadratic form matrix (see equations 11 and 12 in paper) */

    int col;
    float c1, c2, c3, c4, c5;
    c1 = 0.429043; c2 = 0.511664;
    c3 = 0.743125; c4 = 0.886227; c5 = 0.247708;

    for (col = 0; col < 3; col++) { /* Equation 12 */
        matrices[col][0][0] = c1 * coeffs[8][col]; /* c1 L_{22}  */
        matrices[col][0][1] = c1 * coeffs[4][col]; /* c1 L_{2-2} */
        matrices[col][0][2] = c1 * coeffs[7][col]; /* c1 L_{21}  */
        matrices[col][0][3] = c2 * coeffs[3][col]; /* c2 L_{11}  */

        matrices[col][1][0] = c1 * coeffs[4][col]; /* c1 L_{2-2} */
        matrices[col][1][1] = -c1 * coeffs[8][col]; /*-c1 L_{22}  */
        matrices[col][1][2] = c1 * coeffs[5][col]; /* c1 L_{2-1} */
        matrices[col][1][3] = c2 * coeffs[1][col]; /* c2 L_{1-1} */

        matrices[col][2][0] = c1 * coeffs[7][col]; /* c1 L_{21}  */
        matrices[col][2][1] = c1 * coeffs[5][col]; /* c1 L_{2-1} */
        matrices[col][2][2] = c3 * coeffs[6][col]; /* c3 L_{20}  */
        matrices[col][2][3] = c2 * coeffs[2][col]; /* c2 L_{10}  */

        matrices[col][3][0] = c2 * coeffs[3][col]; /* c2 L_{11}  */
        matrices[col][3][1] = c2 * coeffs[1][col]; /* c2 L_{1-1} */
        matrices[col][3][2] = c2 * coeffs[2][col]; /* c2 L_{10}  */
        matrices[col][3][3] = c4 * coeffs[0][col] - c5 * coeffs[6][col];
        /* c4 L_{00} - c5 L_{20} */
    }
}

void updateCoeffs(glm::vec3 v, float dOmega, glm::vec3 pos) {
    for (int i = 0; i < 3; i++) {
        float c; /* A different constant for each coefficient */

        /* L_{00}.  Note that Y_{00} = 0.282095 */
        c = 0.282095;
        coeffs[0][i] += v[i] * c * dOmega;

        /* L_{1m}. -1 <= m <= 1.  The linear terms */
        c = 0.488603;
        coeffs[1][i] += v[i] * (c * pos.y) * dOmega;   /* Y_{1-1} = 0.488603 y  */
        coeffs[2][i] += v[i] * (c * pos.z) * dOmega;   /* Y_{10}  = 0.488603 z  */
        coeffs[3][i] += v[i] * (c * pos.x) * dOmega;   /* Y_{11}  = 0.488603 x  */

        /* The Quadratic terms, L_{2m} -2 <= m <= 2 */

        /* First, L_{2-2}, L_{2-1}, L_{21} corresponding to xy,yz,xz */
        c = 1.092548;
        coeffs[4][i] += v[i] * (c * pos.x * pos.y) * dOmega; /* Y_{2-2} = 1.092548 xy */
        coeffs[5][i] += v[i] * (c * pos.y * pos.z) * dOmega; /* Y_{2-1} = 1.092548 yz */
        coeffs[7][i] += v[i] * (c * pos.x * pos.z) * dOmega; /* Y_{21}  = 1.092548 xz */

        /* L_{20}.  Note that Y_{20} = 0.315392 (3z^2 - 1) */
        c = 0.315392;
        coeffs[6][i] += v[i] * (c * (3 * pos.z * pos.z - 1)) * dOmega;

        /* L_{22}.  Note that Y_{22} = 0.546274 (x^2 - y^2) */
        c = 0.546274;
        coeffs[8][i] += v[i] * (c * (pos.x * pos.x - pos.y * pos.y)) * dOmega;

    }
}

float sinc(float x) {               /* Supporting sinc function */
    if (fabs(x) < 1.0e-4) return 1.0;
    else return(sin(x) / x);
}

void computeHDR() {
    const char* file = "./Resources/HDR/test.png";

    int w, h, channel;
    auto data = stbi_loadf(file, &w, &h, &channel, 0);
    if (!data) {
        printf("Cannot open image file\n");
        return;
    }
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < 3; k++) {
                hdr[i][j][k] = data[(i * w + j) * channel + k];
                // fscanf(fp, "%f", &data[i][j][k]);
            }
        }
    }
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < w; j++) {
            float v = (w / 2.f - i) / (w / 2);
            float u = (j - w / 2.f) / (w / 2);
            float r = std::sqrt(u * u + v * v);
            if (r > 1.0f) continue;

            float theta = glm::pi<float>() * r;
            float phi = std::atan2(v, u);


            float x = sin(theta) * cos(phi);
            float y = sin(theta) * sin(phi);
            float z = cos(theta);

            float dOmega = glm::two_pi<float>() / w * (glm::two_pi<float>() / w) * sinc(theta);
            updateCoeffs(hdr[i][j], dOmega, glm::vec3(x, y, z));
        }
    }
}

void computeCube() {
    const char* files[6] = { 
        "./Resources/Textures/right.jpg",
        "./Resources/Textures/left.jpg",
        "./Resources/Textures/top.jpg",
        "./Resources/Textures/bottom.jpg",
        "./Resources/Textures/front.jpg",
        "./Resources/Textures/back.jpg"
    };

    for (int i = 0; i < 6; i++) {
        int w, h, channel;
        auto data = stbi_loadf(files[i], &w, &h, &channel, 0);
        if (!data) {
            printf("Cannot open image file\n");
            return;
        }
        sampler[i] = std::make_shared<SHSampler>(data, w);
    }

    for (int i = 0; i < 2048 * 2048; i++) {
        auto vec = GetUniformRandSphere();
        auto uv = XYZ2CubeUV(vec);
        updateCoeffs(sampler[uv.id]->Sample(uv.uv), 1.0f / (2048 * 2048), vec);
    }

}

int main(int argc, char** argv) {
    computeCube();
    genMatrix();
    printf("glm::mat4 RMat = glm::mat4(\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%lf, ", matrices[0][i][j]);
        }
        printf("\n");
    }
    printf(");\n");
    printf("glm::mat4 GMat = glm::mat4(\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%lf, ", matrices[1][i][j]);
        }
        printf("\n");
    }
    printf(");\n");
    printf("glm::mat4 BMat = glm::mat4(\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%lf, ", matrices[2][i][j]);
        }
        printf("\n");
    }
    printf(");\n");
    for (int i = 0; i < 9; i++) {
        printf("Coefficient %d: %lf, %lf, %lf\n", i, coeffs[i][0], coeffs[i][1], coeffs[i][2]);
    }
    return 0;
}