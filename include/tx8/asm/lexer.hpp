#pragma once

#include <istream>
#include <memory>

namespace tx {
    struct LexerToken { };
    class Lexer {
      public:
        explicit Lexer(std::unique_ptr<std::istream> input) : is(std::move(input)) {};

        tx::LexerToken next_token();

      private:
        std::unique_ptr<std::istream> is;
    };
} // namespace tx
