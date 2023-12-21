#include "tui.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "matrix.hpp"
#include "neural-network.hpp"
#include "read_mnist.hpp"

int main() {
  /* NeuralNetwork nn{2, {2}, 1}; */
  /**/
  /* for (int i = 0; i < 2000; ++i) { */
  /*   nn.backward({{0}, {0}}, {{0}}); */
  /*   nn.backward({{0}, {1}}, {{1}}); */
  /*   nn.backward({{1}, {0}}, {{1}}); */
  /*   nn.backward({{1}, {1}}, {{0}}); */
  /* } */
  /**/
  /* print(nn.forward({{0}, {0}})); */
  /* print(nn.forward({{0}, {1}})); */
  /* print(nn.forward({{1}, {0}})); */
  /* print(nn.forward({{1}, {1}})); */

  std::vector<Matrix<float>> targets;
  targets.reserve(10);

  for (int i = 0; i < 10; ++i) {
    targets.push_back(Matrix<float>{{0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f},
                                    {0.0f}});
  }

  targets[0][0][0] = 1.0f;
  targets[1][1][0] = 1.0f;
  targets[2][2][0] = 1.0f;
  targets[3][3][0] = 1.0f;
  targets[4][4][0] = 1.0f;
  targets[5][5][0] = 1.0f;
  targets[6][6][0] = 1.0f;
  targets[7][7][0] = 1.0f;
  targets[8][8][0] = 1.0f;
  targets[9][9][0] = 1.0f;

  auto labels =
      read_labels("dataset/train-labels-idx1-ubyte/train-labels.idx1-ubyte");

  auto images =
      read_images("dataset/train-images-idx3-ubyte/train-images.idx3-ubyte");

  auto test_images =
      read_images("dataset/t10k-images-idx3-ubyte/t10k-images.idx3-ubyte");

  NeuralNetwork nn{28 * 28, {128}, 10};

  std::cout << "Training...\n";
  for (int i = 0; i < images.size(); ++i) {
    nn.backward(images[i], targets[labels[i]]);
  }
  std::cout << "Training finished.\n";

  display_ui(nn, test_images);

  return 0;
}
