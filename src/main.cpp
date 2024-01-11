#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include "stb_image.h"
#include "stb_image_write.h"

// Implementação da FFT 1D
void fft1d(float* data_real, float* data_imag, int size, int sign)
{
    if (size <= 1) {
        return;
    }

    // Divide os dados em pares e ímpares
    int half_size = size / 2;
    float* even_real = new float[half_size];
    float* even_imag = new float[half_size];
    float* odd_real = new float[half_size];
    float* odd_imag = new float[half_size];

    for (int i = 0; i < half_size; i++) {
        even_real[i] = data_real[i * 2];
        even_imag[i] = data_imag[i * 2];
        odd_real[i] = data_real[i * 2 + 1];
        odd_imag[i] = data_imag[i * 2 + 1];
    }

    // Chama recursivamente a FFT para pares e ímpares
    fft1d(even_real, even_imag, half_size, sign);
    fft1d(odd_real, odd_imag, half_size, sign);

    // Combina os resultados
    for (int i = 0; i < half_size; i++) {
        float angle = sign * 2.0 * M_PI * i / size;
        float cos_val = cos(angle);
        float sin_val = sin(angle);

        float temp_real = cos_val * odd_real[i] - sin_val * odd_imag[i];
        float temp_imag = cos_val * odd_imag[i] + sin_val * odd_real[i];

        data_real[i] = even_real[i] + temp_real;
        data_imag[i] = even_imag[i] + temp_imag;
        data_real[i + half_size] = even_real[i] - temp_real;
        data_imag[i + half_size] = even_imag[i] - temp_imag;
    }

    // Libera a memória alocada
    delete[] even_real;
    delete[] even_imag;
    delete[] odd_real;
    delete[] odd_imag;
};

// Função para calcular a FFT 2D
void fft2d(float* real, float* imag, int rows, int cols, int sign)
{
    // Apply FFT 1D on rows
    for (int i = 0; i < rows; ++i)
    {
        fft1d(real + i * cols, imag + i * cols, cols, sign);
    }

    // Apply FFT 1D on cols
    float* tColReal = new float[rows];
    float* tColImag = new float[rows];
    for (int i = 0; i < cols; ++i)
    {
        // Tranpose from vertical to horizontal
        for (int j = 0; j < rows; ++j)
        {
            tColReal[j] = real[j * cols + i];
            tColImag[j] = imag[j * cols + i];
        }

        fft1d(tColReal, tColImag, rows, sign);

        // Transpose from horizontal to vertical
        for (int j = 0; j < rows; ++j)
        {
            real[j * cols + i] = tColReal[j];
            imag[j * cols + i] = tColImag[j];
        }
    }
}

// Função para calcular a FFT 2D inversa
void ifft2d(float* real, float* imag, int rows, int cols) {
    // Aplica a FFT 2D inversa invertendo o sinal
    fft2d(real, imag, rows, cols, -1);

    // Normaliza os resultados
    int size = rows * cols;
    for (int i = 0; i < size; i++) {
        real[i] /= size;
        imag[i] /= size;
    }
}

// Função para aplicar o efeito de Bloom no domínio da frequência
void applyBloomFrequencyDomain(unsigned char* inputImage, unsigned char* outputImage, int width, int height, int channels,
                               float bloomIntensity, float bloomThreshold) {
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

int main()
{
    char c;
    float intensity;
    float threshold;
    do
    {
        fprintf(stdout, "(intensity) (threshold):");
        fscanf(stdin, "%f %f", &intensity, &threshold);
        fflush(stdin);

        // Carrega a imagem de entrada usando stb_image.h
        int width, height, channels;
        unsigned char* inputImage = stbi_load("input.jpeg", &width, &height, &channels, 0);

        if (!inputImage) {
            std::cerr << "Erro ao carregar a imagem de entrada." << std::endl;
            return -1;
        }

        // Aloca memória para a imagem de saída
        unsigned char* outputImage = new unsigned char[width * height * channels];

        // Aplica o efeito de Bloom no domínio da frequência
        applyBloomFrequencyDomain(inputImage, outputImage, width, height, channels, intensity, threshold);

        // Salva a imagem resultante usando stb_image_write.h
        stbi_write_png("output.png", width, height, channels, outputImage, width * channels);

        // Libera a memória alocada
        stbi_image_free(inputImage);
        delete[] outputImage;

    } while((c = getchar()) != 'n');

    return 0;
}
