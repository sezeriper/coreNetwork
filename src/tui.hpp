#include "neural-network.hpp"
#include "read_mnist.hpp"
#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

static constexpr int cell_size_x = 2;
static constexpr int cell_size_y = 4;
static constexpr int mat_w = 28;
static constexpr int mat_h = 28;
static constexpr int img_w = cell_size_x * mat_w;
static constexpr int img_h = cell_size_y * mat_h;

int get_rand_int() { return rand() % 10'000; }

void display_ui(const NeuralNetwork &nn,
                const std::vector<Matrix<float>> &imgs) {
  using namespace ftxui;

  srand(time(NULL));

  Canvas c(img_w, img_h);
  /* Matrix<float> img(28 * 28, 1, 0.0f); */
  std::size_t img_index = get_rand_int();
  Matrix<float> img = imgs[img_index];

  auto draw_pixel = [&c](int x, int y) {
    x -= 1;
    y -= 2;
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 4; ++j) {
        c.DrawBlock(x + i, y + j, true, Color::White);
      }
    }
  };

  auto draw_square = [&c, &draw_pixel](int x, int y) {
    x -= 1;
    y -= 2;
    for (int i = 0; i < 4; i += 2) {
      for (int j = 0; j < 8; j += 4) {
        draw_pixel(x + i, y + j);
      }
    }
  };

  auto copy_img_to_canvas = [&]() {
    c = Canvas(img_w, img_h);
    for (int x = 0; x < mat_w; ++x) {
      for (int y = 0; y < mat_h; ++y) {
        const auto val = static_cast<uint8_t>(img[y * mat_w + x][0] * 255.0f);

        if (val != 0) {
          draw_pixel(x * 2, y * 4);
        }
      }
    }
  };

  auto copy_canvas_to_img = [&]() {
    img.map([](float x) { return 0.0f; });
    for (int x = 0; x < mat_w; ++x) {
      for (int y = 0; y < mat_h; ++y) {
        auto pixel = c.GetPixel(x, y);
        img[y * mat_w + x][0] = static_cast<float>(pixel.character != " ");
      }
    }
  };

  copy_img_to_canvas();

  auto renderer = Renderer([&c] { return canvas(c); });

  auto renderer_with_mouse = CatchEvent(renderer, [&](Event e) {
    if (e.is_mouse()) {
      int mouse_x = e.mouse().x * 2;
      int mouse_y = e.mouse().y * 4;

      if (e.mouse().IsHeld()) {
        draw_square(mouse_x, mouse_y);
        copy_canvas_to_img();
        return true;
      }
    }

    if (e.character() == "c") {
      img.map([](float) { return 0.0f; });
      c = Canvas(img_w, img_h);
      return true;
    }

    if (e.character() == " ") {
      img_index = get_rand_int();
      img = imgs[img_index];
      copy_img_to_canvas();
      return true;
    }

    return false;
  });

  auto result_gauge = [&](int number) {
    auto result_mat = nn.forward(img);
    auto result = result_mat[number][0];
    return hbox({text(std::to_string(number)), separator(),
                 gaugeRight(result) | color(Color::Interpolate(
                                          result, Color::Red, Color::Green))});
  };

  FlexboxConfig fconf;
  fconf.align_items = FlexboxConfig::AlignItems::FlexStart;

  // Add some separator to decorate the whole component:
  auto component_renderer = Renderer(renderer_with_mouse, [&] {
    return flexbox(
        {
            renderer_with_mouse->Render() | border,
            vbox({
                text("Neural Network Predictions:"),
                separator(),
                result_gauge(0),
                separator(),
                result_gauge(1),
                separator(),
                result_gauge(2),
                separator(),
                result_gauge(3),
                separator(),
                result_gauge(4),
                separator(),
                result_gauge(5),
                separator(),
                result_gauge(6),
                separator(),
                result_gauge(7),
                separator(),
                result_gauge(8),
                separator(),
                result_gauge(9),
                separator(),
            }) | flex,
        },
        fconf);
  });

  auto screen = ScreenInteractive::FixedSize(140, 30);
  screen.Loop(component_renderer);
}
