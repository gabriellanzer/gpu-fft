#pragma once

#include "fft-cpu.hpp"

// Função para aplicar o efeito de Bloom no domínio da frequência
inline void bloomPerChannelFrequency(unsigned char* inputImage, unsigned char* outputImage,
            int width, int height, int channels, float bloomIntensity, float bloomThreshold) {
    // Converte os canais de cor para float
    float* real = new float[width * height];
    float* imag = new float[width * height];

    for (int c = 0; c < channels; c++)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                real[i * width + j] = static_cast<float>(inputImage[(i * width + j) * channels + c] / 255.0f);
                imag[i * width + j] = 0.0;
            }
        }

        // Aplica a FFT 2D na componente de cor
        fft2d(real, imag, height, width, 1);

        // Aplica o efeito de Bloom no domínio da frequência
        int size = height * width;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                // Calcule o espectro de magnitude
                double magnitude = sqrt(real[i * width + j] * real[i * width + j] +
                                        imag[i * width + j] * imag[i * width + j]);
                magnitude /= size;

                // Aplica o efeito Bloom no domínio da frequência
                if (magnitude > bloomThreshold)
                {
                    real[i * width + j] *= (1.0 + bloomIntensity);
                    imag[i * width + j] *= (1.0 + bloomIntensity);
                }
            }
        }

        // Aplica a FFT 2D inversa
        ifft2d(real, imag, height, width);

        // Copia os valores resultantes para o canal correspondente
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                const int index = (i * width + j) * channels + c;
                outputImage[index] = static_cast<unsigned char>(std::min(real[i * width + j], 1.0f) * 255);
            }
        }
    }

    // Libera a memória alocada
    delete[] real;
    delete[] imag;
}