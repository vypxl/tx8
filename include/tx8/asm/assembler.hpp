/**
 * @file assembler.h
 * @brief Assembler and its methods
 * @details Includes methods for parsing assembly and generating debug text or tx8 binary output
 */

#pragma once

#include "lexer.hpp"
#include "tx8/core/log.hpp"
#include "tx8_parser.hpp"

#include <optional>
#include <tx8/core/types.h>
#include <vector>

namespace tx {
    class Assembler {
        std::vector<tx_Label>         labels;
        std::vector<tx_Instruction>   instructions;
        tx_uint32                     position      = 0;
        tx_uint32                     last_label_id = 1;
        bool                          error         = false;
        bool                          ran           = false;
        std::unique_ptr<std::istream> is;

        tx::parser::Lexer  lexer;
        tx::parser::Parser parser;

        /// Get the position associated with the label which has the specified id.
        tx_uint32 convert_label(tx_uint32 id);
        /// Convert all label IDs found in parameters of instructions in the instruction list to their corresponding absolute positions
        void convert_labels();

        /// Register a new label and return its id or the id of the already registered label with the same name
        tx_uint32 handle_label(const std::string& name);
        /// Set the position of a registered label if it has not been set already. Returns the id of the label.
        tx_uint32 set_label_position(const std::string& name);

        /// Add a parsed instruction to the instruction list of the assembler
        void add_instruction(tx_Instruction inst);

        static void      write_parameter(tx_Parameter& p, std::vector<tx_uint8>& output);
        static void      write_instruction(tx_Instruction& inst, std::vector<tx_uint8>& output);
        static tx_uint32 calculate_instruction_length(tx_Instruction& inst);

        friend class tx::parser::Parser;

      public:
        /// @brief Construct the assembler with the given input stream
        /// @param input The input stream to read the assembly code from
        explicit Assembler(std::unique_ptr<std::istream> input);
        /// Construct the assembler with a string containing the assembly code
        explicit Assembler(const std::string& input);
        /// Construct the assembler with a C-string containing the assembly code
        explicit Assembler(const char* input);

        /// @brief Run the assembly process
        /// @details The generate_binary() or write_binary_file() automatically call this.
        ///          Call it manually only if you have legitimate reasons for it.
        void run();

        /// Write the generated binary stream specified by `output`
        /// @returns true if the binary was successfully written, false otherwise
        bool write_binary(std::ostream& output);
        /// Generate the binary into a buffer
        /// @returns The binary buffer if successful
        std::optional<std::vector<tx_uint8>> generate_binary();
        /// Get the size of the binary the assembler would currently generate (only makes sense after run() was called)
        static inline tx_uint32 get_binary_size(Assembler* as) { return as->position; }

        /// Print an error message with the current line number from lex
        template <typename... Args>
        void report_error(const char* format, Args... args) {
            tx::log_err(format, args...);
            error = 1;
        }

        /// Pretty print the instruction list
        void print_instructions();
        /// Pretty print all found labels and their positions
        void print_labels();
    };
} // namespace tx
