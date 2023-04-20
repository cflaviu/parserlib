#ifndef PARSERLIB_CHOICEPARSER_HPP
#define PARSERLIB_CHOICEPARSER_HPP


#include "ParserNode.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"


namespace parserlib {


    /**
     * Choice of parsers.
     * At least one parser must parse successfully in order to parse the choice successfully.
     * @param Children children parser nodes.
     */
    template <class ...Children> class ChoiceParser : public ParserNode<ChoiceParser<Children...>> {
    public:
        /**
         * Constructor.
         * @param children children nodes.
         */
        ChoiceParser(const std::tuple<Children...>& children) 
            : m_children(children) {
        }

        /**
         * Returns the children nodes.
         * @return the children nodes.
         */
        const std::tuple<Children...>& children() const {
            return m_children;
        }

        /**
         * Invokes all child parsers, one by one, until one returns true.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return parse<0>(pc, [&](const auto& child) { return child(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionBase(ParseContextType& pc) const {
            return parse<0>(pc, [&](const auto& child) { return child.parseLeftRecursionBase(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parseLRC<0>(pc, lrc);
        }

    private:
        std::tuple<Children...> m_children;

        template <size_t Index, class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            if constexpr (Index < sizeof...(Children)) {
                if (pf(std::get<Index>(m_children))) {
                    return true;
                }
                return parse<Index + 1>(pc, pf);
            }
            else {
                return false;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRC(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            if constexpr (Index < sizeof...(Children)) {
                lrc.setContinuationResolved(false);
                if (std::get<Index>(m_children).parseLeftRecursionContinuation(pc, lrc)) {
                    return true;
                }
                return parseLRC<Index + 1>(pc, lrc);
            }
            else {
                return false;
            }
        }
    };


    /**
     * Creates a choice of parsers out of two parsers.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType1, class ParserNodeType2>
    ChoiceParser<ParserNodeType1, ParserNodeType2>
    operator | (const ParserNode<ParserNodeType1>& node1, const ParserNode<ParserNodeType2>& node2) {
        return ChoiceParser<ParserNodeType1, ParserNodeType2>(
            std::make_tuple(
                static_cast<const ParserNodeType1&>(node1), 
                static_cast<const ParserNodeType2&>(node2)));
    }


    /**
     * Creates a choice of parsers out of two choice parsers.
     * It flattens the structures into one choice.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ...Children1, class ...Children2>
    ChoiceParser<Children1..., Children2...>
    operator | (const ParserNode<ChoiceParser<Children1...>>& node1, const ParserNode<ChoiceParser<Children2...>>& node2) {
        return ChoiceParser<Children1..., Children2...>(
            std::tuple_cat(
                static_cast<const ChoiceParser<Children1...>&>(node1).children(), 
                static_cast<const ChoiceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a choice of parsers out of a choice parser and a non-choice parser.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ...Children1, class ParserNodeType2>
    ChoiceParser<Children1..., ParserNodeType2>
    operator | (const ParserNode<ChoiceParser<Children1...>>& node1, const ParserNode<ParserNodeType2>& node2) {
        return ChoiceParser<Children1..., ParserNodeType2>(
            std::tuple_cat(
                static_cast<const ChoiceParser<Children1...>&>(node1).children(), 
                std::make_tuple(static_cast<const ParserNodeType2&>(node2))));
    }


    /**
     * Creates a choice of parsers out of a non-choice parser and a choice parser.
     * It flattens the structures into one choice.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType1, class ...Children2>
    ChoiceParser<ParserNodeType1, Children2...>
        operator | (const ParserNode<ParserNodeType1>& node1, const ParserNode<ChoiceParser<Children2...>>& node2) {
        return ChoiceParser<ParserNodeType1, Children2...>(
            std::tuple_cat(
                std::make_tuple(static_cast<const ParserNodeType1&>(node1)), 
                static_cast<const ChoiceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a choice of parsers out of a parser and a char.
     * @param node parser node.
     * @param ch character.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, char ch) {
        return node | TerminalParser<char>(ch);
    }


    /**
     * Creates a choice of parsers out of a parser and a char string.
     * @param node parser node.
     * @param str character string.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, const char* str) {
        return node | TerminalStringParser<char>(str);
    }


    /**
     * Creates a choice of parsers out of a parser and a wchar_t.
     * @param node parser node.
     * @param ch character.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, wchar_t ch) {
        return node | TerminalParser<wchar_t>(ch);
    }


    /**
     * Creates a choice of parsers out of a parser and a wchar_t string.
     * @param node parser node.
     * @param str character string.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, const wchar_t* str) {
        return node | TerminalStringParser<wchar_t>(str);
    }


    /**
     * Creates a choice of parsers out of a parser and a char16_t.
     * @param node parser node.
     * @param ch character.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, char16_t ch) {
        return node | TerminalParser<char16_t>(ch);
    }


    /**
     * Creates a choice of parsers out of a parser and a char16_t string.
     * @param node parser node.
     * @param str character string.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, const char16_t* str) {
        return node | TerminalStringParser<char16_t>(str);
    }


    /**
     * Creates a choice of parsers out of a parser and a char32_t.
     * @param node parser node.
     * @param ch character.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, char32_t ch) {
        return node | TerminalParser<char32_t>(ch);
    }


    /**
     * Creates a choice of parsers out of a parser and a char32_t string.
     * @param node parser node.
     * @param str character string.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const ParserNode<ParserNodeType>& node, const char32_t* str) {
        return node | TerminalStringParser<char32_t>(str);
    }


    /**
     * Creates a choice of parsers out of a char and a parser.
     * @param ch character.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (char ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char>(ch) | node;
    }


    /**
     * Creates a choice of parsers out of a char string and a parser.
     * @param str string.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const char* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char>(str) | node;
    }


    /**
     * Creates a choice of parsers out of a wchar_t and a parser.
     * @param ch character.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (wchar_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<wchar_t>(ch) | node;
    }


    /**
     * Creates a choice of parsers out of a wchar_t string and a parser.
     * @param str string.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const wchar_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<wchar_t>(str) | node;
    }


    /**
     * Creates a choice of parsers out of a char16_t and a parser.
     * @param ch character.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (char16_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char16_t>(ch) | node;
    }


    /**
     * Creates a choice of parsers out of a char16_t string and a parser.
     * @param str string.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const char16_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char16_t>(str) | node;
    }


    /**
     * Creates a choice of parsers out of a char32_t and a parser.
     * @param ch character.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (char32_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char32_t>(ch) | node;
    }


    /**
     * Creates a choice of parsers out of a char32_t string and a parser.
     * @param str string.
     * @param node parser node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType>
    auto operator | (const char32_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char32_t>(str) | node;
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICEPARSER_HPP
