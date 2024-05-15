/**
 * \file Printing.hpp
 * \brief Contains example functions to demonstrate how this codebase will be structured.
 */

#include "Printing.hpp"

#include <iostream>

namespace cw {
/**
 * \brief Adds two integers together.
 * \param x LHS.
 * \param y RHS.
 * \returns x + y.
 */
int add(int x, int y) { return x + y; }

/**
 * \brief Writes a string to std::cout.
 * \param m The message to write.
 */
void print(const std::string& m) { std::cout << m << std::endl; }
} // namespace cw
