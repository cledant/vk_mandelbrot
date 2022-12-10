#ifndef VK_MANDELBROT_UITEXTS_HPP
#define VK_MANDELBROT_UITEXTS_HPP

namespace UiTexts {

constexpr char const *INPUT_HELP_WIN_NAME = "Help";
constexpr char const *INPUT_HELP_WIN_TEXT =
  "Controls:\n"
  "\tLeft click: reset camera position and zoom\n"
  "\tScroll up: increase zoom on mouse cursor\n"
  "\tScroll down: decrease zoom on mouse cursor\n"
  "\tWASD: move camera position\n"
  "\tEqual/Minus: increase/decrease iteration\n"
  "\tP: reset iteration\n"
  "\tLeft Shift: movement multiplier for WASD and iteration";

constexpr char const *INPUT_ERROR_TEXT = "Error";

constexpr char const *INPUT_ITERATION_WIN_NAME = "Set number of iteration";
constexpr char const *INPUT_ITERATION_WIN_TEXT = "Iterations";

}

#endif // VK_MANDELBROT_UITEXTS_HPP
