#pragma once

#include "matrix.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

std::uint32_t big_to_little_endian(std::uint32_t value) {
  return (value << 24) | ((value << 8) & 0x00FF0000) |
         ((value >> 8) & 0X0000FF00) | (value >> 24);
}

std::vector<std::uint8_t> read_labels(const char *file_name) {
  std::ifstream stream(file_name, std::ios::binary);

  if (!stream.is_open()) {
    std::cerr << "File not found.\n";
  }

  std::uint32_t magic_number = 0;
  stream.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
  magic_number = big_to_little_endian(magic_number);
  std::cout << "Magic number: " << magic_number << '\n';

  std::uint32_t num_of_labels = 0;
  stream.read(reinterpret_cast<char *>(&num_of_labels), sizeof(num_of_labels));
  num_of_labels = big_to_little_endian(num_of_labels);
  std::cout << "Num of labels: " << num_of_labels << '\n';

  std::vector<std::uint8_t> labels;
  labels.resize(num_of_labels);
  stream.read(reinterpret_cast<char *>(&labels[0]),
              sizeof(std::uint8_t) * num_of_labels);

  return labels;
}

std::vector<Matrix<float>> read_images(const char *file_name) {
  std::ifstream stream(file_name, std::ios::binary);

  if (!stream.is_open()) {
    std::cerr << "File not found.\n";
  }

  std::uint32_t magic_number = 0;
  stream.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
  magic_number = big_to_little_endian(magic_number);
  std::cout << "Magic number: " << magic_number << '\n';

  std::uint32_t num_of_images = 0;
  stream.read(reinterpret_cast<char *>(&num_of_images), sizeof(num_of_images));
  num_of_images = big_to_little_endian(num_of_images);
  std::cout << "Num of images: " << num_of_images << '\n';

  std::uint32_t image_rows = 0;
  stream.read(reinterpret_cast<char *>(&image_rows), sizeof(image_rows));
  image_rows = big_to_little_endian(image_rows);
  std::cout << "Num of rows in image: " << image_rows << '\n';

  std::uint32_t image_cols = 0;
  stream.read(reinterpret_cast<char *>(&image_cols), sizeof(image_cols));
  image_cols = big_to_little_endian(image_cols);
  std::cout << "Num of cols in image: " << image_cols << '\n';

  std::vector<Matrix<std::uint8_t>> images;
  images.reserve(num_of_images);

  for (int i = 0; i < num_of_images; ++i) {
    images.emplace_back(image_rows * image_cols, 1, 0);
  }

  for (int i = 0; i < num_of_images; ++i) {
    stream.read(reinterpret_cast<char *>(&images[i][0][0]),
                sizeof(std::uint8_t) * image_rows * image_cols);
  }

  std::vector<Matrix<float>> images_f;
  images_f.reserve(num_of_images);
  for (int i = 0; i < num_of_images; ++i) {
    images_f.emplace_back(image_rows * image_cols, 1, 0.0f);
  }

  for (int i = 0; i < num_of_images; ++i) {
    for (int row = 0; row < 28 * 28; ++row) {
      float val = static_cast<float>(images[i][row][0]) / 255.0f;
      images_f[i][row][0] = val > 0.8f ? 1.0f : 0.0f;
    }
  }
  return images_f;
}
