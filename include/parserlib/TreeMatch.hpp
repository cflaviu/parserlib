#ifndef PARSERLIB_TREEMATCH_HPP
#define PARSERLIB_TREEMATCH_HPP


#include <string>
#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that adds a match to the current parse context if another parser parses the input succecssfully.
     * @param ParserNodeType the parser to invoke.
     * @param MatchIdType type of match id.
     */
    template <class ParserNodeType, class MatchIdType> class TreeMatch 
        : public ParserNode<TreeMatch<ParserNodeType, MatchIdType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         * @param matchId the match id.
         */
        TreeMatch(const ParserNodeType& child, const MatchIdType& matchId) 
            : m_child(child), m_matchId(matchId) {
        }

        /**
         * Returns the parser to invoke.
         * @return the parser to invoke.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Returns the match id.
         * @return the match id.
         */
        const MatchIdType& matchId() const {
            return m_matchId;
        }

        /**
         * If the child parser succeeds, then a match is added to the context.
         * @param pc parse context.
         * @return true if the 1st parsing succeeded, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return parse(pc, [&]() { return m_child(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionBase(ParseContextType& pc) const {
            return parse(pc, [&]() { return m_child.parseLeftRecursionBase(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            if (m_child.parseLeftRecursionContinuation(pc, lrc)) {
                const size_t endMatchCount = pc.matches().size();
                const size_t childMatchCount = endMatchCount - lrc.startMatchCount();
                pc.addMatch(m_matchId, lrc.startPosition(), pc.sourcePosition(), childMatchCount);
                return true;
            }
            return false;
        }

    private:
        const ParserNodeType m_child;
        const MatchIdType m_matchId;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            const auto begin = pc.sourcePosition();
            const size_t beginMatchCount = pc.matches().size();
            if (pf()) {
                const size_t endMatchCount = pc.matches().size();
                const size_t childMatchCount = endMatchCount - beginMatchCount;
                pc.addMatch(m_matchId, begin, pc.sourcePosition(), childMatchCount);
                return true;
            }
            return false;
        }
    };


    /**
     * Operator that allows a match to be inserted into the grammar.
     * @param node node to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParserNodeType, class MatchIdType>
    TreeMatch<ParserNodeType, MatchIdType>
        operator >= (const ParserNode<ParserNodeType>& node, const MatchIdType& matchId) {
        return TreeMatch<ParserNodeType, MatchIdType>(static_cast<const ParserNodeType&>(node), matchId);
    }


} //namespace parserlib


#endif //PARSERLIB_TREEMATCH_HPP
