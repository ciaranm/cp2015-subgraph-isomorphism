/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef GUARD_BIT_GRAPH_HH
#define GUARD_BIT_GRAPH_HH 1

#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>

/// We'll use an array of unsigned long longs to represent our bits.
using BitWord = unsigned long long;

/// Number of bits per word.
static const constexpr int bits_per_word = sizeof(BitWord) * 8;

/**
 * A bitset with a fixed maximum size. This only provides the operations
 * we actually use in the bitset algorithms: it's more readable this way
 * than doing all the bit voodoo inline.
 *
 * Indices start at 0.
 */
template <unsigned words_>
class FixedBitSet
{
    private:
        using Bits = std::array<BitWord, words_>;

        Bits _bits = {{ }};

    public:
        /**
         * Set a given bit 'on'.
         */
        auto set(int a) -> void
        {
            // The 1 does have to be of type BitWord. If we just specify a
            // literal, it ends up being an int, and it isn't converted
            // upwards until after the shift is done.
            _bits[a / bits_per_word] |= (BitWord{ 1 } << (a % bits_per_word));
        }

        /**
         * Set a given bit 'on'.
         */
        auto set_atomic(int a) -> void
        {
            // we don't have std::atomic_concurrent_view yet...
            __sync_or_and_fetch(&_bits[a / bits_per_word], (BitWord{ 1 } << (a % bits_per_word)));
        }

        /**
         * Set a given bit 'off'.
         */
        auto unset(int a) -> void
        {
            _bits[a / bits_per_word] &= ~(BitWord{ 1 } << (a % bits_per_word));
        }

        /**
         * Set all bits on.
         */
        auto set_up_to(int size) -> void
        {
            unset_all();
            for (int i = 0 ; i < size ; ++i)
                set(i);
        }

        /**
         * Set all bits off.
         */
        auto unset_all() -> void
        {
            for (unsigned i = 0 ; i < words_ ; ++i)
                _bits[i] = 0;
        }

        /**
         * Is a given bit on?
         */
        auto test(int a) const -> bool
        {
            return _bits[a / bits_per_word] & (BitWord{ 1 } << (a % bits_per_word));
        }

        /**
         * How many bits are on?
         */
        auto popcount() const -> unsigned
        {
            unsigned result = 0;
            for (auto & p : _bits)
                result += __builtin_popcountll(p);
            return result;
        }

        /**
         * Are any bits on?
         */
        auto empty() const -> bool
        {
            for (auto & p : _bits)
                if (0 != p)
                    return false;
            return true;
        }

        /**
         * Intersect (bitwise-and) with another set.
         */
        auto intersect_with(const FixedBitSet<words_> & other) -> void
        {
            for (typename Bits::size_type i = 0 ; i < words_ ; ++i)
                _bits[i] = _bits[i] & other._bits[i];
        }

        /**
         * Union (bitwise-or) with another set.
         */
        auto union_with(const FixedBitSet<words_> & other) -> void
        {
            for (typename Bits::size_type i = 0 ; i < words_ ; ++i)
                _bits[i] = _bits[i] | other._bits[i];
        }

        /**
         * Intersect with the complement of another set.
         */
        auto intersect_with_complement(const FixedBitSet<words_> & other) -> void
        {
            for (typename Bits::size_type i = 0 ; i < words_ ; ++i)
                _bits[i] = _bits[i] & ~other._bits[i];
        }

        /**
         * Return the index of the first set ('on') bit, or -1 if we are
         * empty.
         */
        auto first_set_bit() const -> int
        {
            for (typename Bits::size_type i = 0 ; i < _bits.size() ; ++i) {
                int b = __builtin_ffsll(_bits[i]);
                if (0 != b)
                    return i * bits_per_word + b - 1;
            }
            return -1;
        }

        /**
         * Return the index of the last set ('on') bit, or -1 if we are
         * empty.
         */
        auto last_set_bit() const -> int
        {
            for (int i = _bits.size() - 1 ; i >= 0 ; --i) {
                if (0 == _bits[i])
                    continue;

                int b = __builtin_clzll(_bits[i]);
                return (i + 1) * bits_per_word - b - 1;
            }
            return -1;
        }

        auto operator== (const FixedBitSet<words_> & other) const -> bool
        {
            if (_bits.size() != other._bits.size())
                return false;

            for (typename Bits::size_type i = 0 ; i < _bits.size() ; ++i)
                if (_bits[i] != other._bits[i])
                    return false;

            return true;
        }
};

/**
 * A bitgraph with a fixed maximum size. In effect this is an adjacency
 * matrix representation. This only provides the operations we actually use
 * in the bitset algorithms.
 *
 * Indices start at 0.
 */
template <unsigned size_>
class FixedBitGraph
{
    private:
        using Rows = std::vector<FixedBitSet<size_> >;

        int _size = 0;
        Rows _adjacency;

    public:
        /**
         * Return the actual size (not the maximum).
         */
        auto size() const -> int
        {
            return _size;
        }

        /**
         * Change our actual size. Must be below the maximum.
         */
        auto resize(int size) -> void
        {
            _size = size;
            _adjacency.resize(size);
        }

        /**
         * Add an edge from a to b (and from b to a).
         */
        auto add_edge(int a, int b) -> void
        {
            _adjacency[a].set(b);
            _adjacency[b].set(a);
        }

        /**
         * Add an edge from a to b (and from b to a).
         */
        auto add_edge_atomic(int a, int b) -> void
        {
            _adjacency[a].set_atomic(b);
            _adjacency[b].set_atomic(a);
        }

        /**
         * Are vertices a and b adjacent?
         */
        auto adjacent(int a, int b) const -> bool
        {
            return _adjacency[a].test(b);
        }

        /**
         * What is the degree of a given vertex?
         */
        auto degree(int a) const -> int
        {
            return _adjacency[a].popcount();
        }

        /**
         * Intersect the supplied bitset with a particular row.
         */
        auto intersect_with_row(int row, FixedBitSet<size_> & p) const -> void
        {
            p.intersect_with(_adjacency[row]);
        }

        /**
         * Intersect the supplied bitset with the complement of a
         * particular row.
         */
        auto intersect_with_row_complement(int row, FixedBitSet<size_> & p) const -> void
        {
            p.intersect_with_complement(_adjacency[row]);
        }

        /**
         * Fetch the neighbourhood of a particular vertex.
         */
        auto neighbourhood(int vertex) const -> FixedBitSet<size_>
        {
            return _adjacency[vertex];
        }
};

/**
 * We have to decide at compile time what the largest graph we'll support
 * is.
 */
constexpr auto max_graph_words __attribute__((unused)) = 1024;

/**
 * Thrown if we exceed max_graph_words.
 */
class GraphTooBig :
    public std::exception
{
    public:
        GraphTooBig() throw ();
};

#endif
