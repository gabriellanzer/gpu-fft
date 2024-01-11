#pragma once
#include <cmath>

// Implementação da FFT 1D
inline void fft1d(float* data_real, float* data_imag, int size, int sign)
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
}

// Função para calcular a FFT 2D
inline void fft2d(float* real, float* imag, int rows, int cols, int sign)
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
inline void ifft2d(float* real, float* imag, int rows, int cols)
{
    // Aplica a FFT 2D inversa invertendo o sinal
    fft2d(real, imag, rows, cols, -1);

    // Normaliza os resultados
    int size = rows * cols;
    for (int i = 0; i < size; i++)
    {
        real[i] /= size;
        imag[i] /= size;
    }
}
