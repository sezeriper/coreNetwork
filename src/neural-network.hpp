#pragma once
#include "./matrix.hpp"
#include <chrono>
#include <initializer_list>
#include <random>
#include <vector>

constexpr float sigmoid(float x) { return 1.0f / (1.0f + exp(-x)); }
constexpr float sigmoid_d(float x) { return x * (1.0f - x); }
constexpr float relu(float x) { return x > 0.0f ? x : 0.0f; }
constexpr float relu_d(float x) { return static_cast<float>(x > 0.0f); }
constexpr float fastsigmoid(float x) {
  return 0.5f * (x / (1.0f + std::abs(x)) + 1.0f);
}
constexpr float fastsigmoid_d(float x) { return x * (1.0f - x); }

#define ACTIVATION sigmoid
#define ACTIVATION_D sigmoid_d

class NeuralNetwork {
private:
  std::vector<Matrix<float>> weights;
  std::vector<Matrix<float>> biases;

  std::uniform_real_distribution<float> dist;
  std::default_random_engine gen;

public:
  float learningRate = 0.2f;

  NeuralNetwork(const int numOfInputs,
                std::initializer_list<const int> numOfHiddens,
                const int numOfOutputs)
      : dist(-0.3f, 0.3f), gen(std::random_device()()) {
    weights.reserve(numOfHiddens.size() + 1);
    biases.reserve(numOfHiddens.size() + 1);

    weights.emplace_back(*numOfHiddens.begin(), numOfInputs);
    biases.emplace_back(*numOfHiddens.begin(), 1);

    for (auto it = numOfHiddens.begin() + 1; it != numOfHiddens.end(); it++) {
      weights.emplace_back(*it, *(it - 1));
      biases.emplace_back(*it, 1);
    }

    weights.emplace_back(numOfOutputs, *(numOfHiddens.end() - 1));
    biases.emplace_back(numOfOutputs, 1);

    for (auto &weight : weights) {
      weight.map([this](float x) { return dist(gen); });
    }

    for (auto &bias : biases) {
      bias.map([this](float x) { return dist(gen); });
    }
  }

  Matrix<float> forward(const Matrix<float> &inputs) const {
    Matrix output = dot(weights[0], inputs);
    output += biases[0];
    output.map(ACTIVATION);

    for (int i = 1, max = weights.size() - 1; i != max; i++) {
      output = dot(weights[i], output);
      output += biases[i];
      output.map(ACTIVATION);
    }

    output = dot(weights.back(), output);
    output += biases.back();
    output.map(ACTIVATION);

    return output;
  }

  void backward(const Matrix<float> &input, const Matrix<float> &target) {
    std::vector<Matrix<float>> acts;
    acts.reserve(weights.size() + 1);

    acts.emplace_back(input);

    for (int i = 0, max = weights.size() - 1; i < max; i++) {
      acts.emplace_back(dot(weights[i], acts[i]));
      acts.back() += biases[i];
      acts.back().map(ACTIVATION);
    }

    acts.emplace_back(dot(weights.back(), acts.back()));
    acts.back() += biases.back();
    acts.back().map(ACTIVATION);

    std::vector<Matrix<float>> errors;
    errors.reserve(weights.size());

    errors.emplace_back(target - acts.back());
    errors.back() *= map(acts.back(), ACTIVATION_D);
    errors.back() *= learningRate;

    for (int i = weights.size() - 1; i >= 1; i--) {
      errors.emplace_back(dot(transpose(weights[i]), errors.back()));
      errors.back() *= map(acts[i], ACTIVATION_D);
      errors.back() *= learningRate;
    }

    for (int i = 0, max = weights.size(); i < max; i++) {
      weights[i] += dot(errors[max - i - 1], transpose(acts[i]));
      biases[i] += errors[max - i - 1];
    }
  }
};
