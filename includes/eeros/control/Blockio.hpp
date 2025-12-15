#ifndef ORG_EEROS_CONTROL_BLOCKIO_HPP_
#define ORG_EEROS_CONTROL_BLOCKIO_HPP_

#include <algorithm>
#include <eeros/control/Block.hpp>
#include <eeros/control/Input.hpp>
#include <eeros/control/Output.hpp>
#include <eeros/control/IndexOutOfBoundsFault.hpp>
#include <functional>

namespace eeros {
namespace control {

template<uint8_t N>
concept None = (N == 0);

template<uint8_t N>
concept One = (N == 1);

template<uint8_t N>
concept Multiple = (N > 1);

struct Empty {};

/**
 * Base class for all blocks with inputs and outputs.
 *
 * Extend this class and override the run method to implement any
 * given algorithm. 
 * 
 * Alternatively, an algorithm can be set directly 
 * when creating such a block. Choose this method when the algorithm 
 * is simple and one wants to avoid using several other blocks doing 
 * a simple algorithm, e.g. adding a offset and scale to a signal.
 * 
 * Define such a block with an example algorithm as follows:
 * Blockio<2,1,Vector2,Vector2> block([&]() {
 *   auto val = (block.getIn(0).getSignal().getValue() + 0.5) * 2;
 *   val[0] *= -1.0;
 *   val += block.getIn(1).getSignal().getValue() + 1.0;
 *   block.getOut().getSignal().setValue(val);
 *   block.getOut().getSignal().setTimestamp(gen.getIn(0).getSignal().getTimestamp());
 * });
* 
 * @tparam N - number of inputs
 * @tparam M - number of outputs
 * @tparam Tin - input type (double - default type)
 * @tparam Tout - output type (double - default type)
 * @tparam Uin - input type (double - default type)
 * @tparam Uout - output type (double - default type)
 * @since v1.2.1
 */

template < uint8_t N, uint8_t M, typename Tin = double, typename Tout = Tin, std::array<SIUnit, N> Uin = SIUnit::generateNSizeArray<N>(), std::array<SIUnit, M> Uout = SIUnit::generateNSizeArray<M>() >
class Blockio : public Block {
 public:
  /**
   * Construct a block with inputs and outputs. 
   * Clears the output signal.
   */
  Blockio() : Blockio([](){}) { }

  /**
   * Construct a block with inputs and outputs.
   * Clears the output signal.
   * The block will run a given algorithm defined by the parameter function.
   *
   * @param f - function defining the algorithm
   */
  Blockio(std::function<void()> const &f) : func(f), in(generateNInputs()), out(generateMOutputs()) {
    initalizeInputsAndOutputs();
  }

  /**
   * Disabling use of copy constructor because the block should never be copied unintentionally.
   */
  Blockio(const Blockio& s) = delete; 

  /**
   * Runs the generic algorithm.
   *
   */
  virtual void run() {
    func();
  }

  /**
   * Get an output of the multiple instances of the block.
   * With additional compile time safety if the passed index is out of scope.
   * 
   * @tparam I - compile time constant index of the input
   * @return input
   */
  template<size_t I>
  decltype(auto) getIn() requires Multiple<N> {
    return std::get<I>(in);
  }

  /**
   * Get an output of the multiple instances of the block.
   * 
   * @param index - runtime index of the input
   * @return input
   */
  Input<Tin, SIUnit::create()>& getIn(uint8_t index) requires Multiple<N> {
    if (index >= N) throw IndexOutOfBoundsFault("Trying to get inexistent element of input vector in block '" + this->getName() + "'"); 
    return in[index];
  }

  /**
   * Get the single input of the block.
   * 
   * @return output
   */
  auto& getIn() requires One<N> {
    return in;
  }

  /**
   * Get an output of the multiple instances of the block.
   * With additional compile time safety if the passed index is out of scope.
   * 
   * @tparam I - compile time constant index of the output
   * @return output
   */
  template<size_t I>
  decltype(auto) getOut() requires Multiple<M> {
    return std::get<I>(out);
  }

  /**
   * Get an output of the multiple instances of the block.
   * 
   * @param index - runtime index of the output
   * @return output
   */
  Output<Tout, SIUnit::create()>& getOut(uint8_t index) requires Multiple<M> {
    if (index >= M) throw IndexOutOfBoundsFault("Trying to get inexistent element of output vector in block '" + this->getName() + "'"); 
    return out[index];
  }

  /**
   * Get the single output of the block.
   * 
   * @return output
   */
  auto& getOut() requires One<M> {
    return out;
  }

 private:
  std::function<void()> func;

  constexpr void initalizeInputsAndOutputs() {
    if constexpr (One<M>) {
      out.setOwner(this);
      out.getSignal().clear();
    }
    else if constexpr (Multiple<M>) {
      std::apply([this](auto&&... out) {
        ((out.setOwner(this)), ...);
        ((out.getSignal().clear()), ...);
      }, out);
    }

    if constexpr (One<N>) {
      in.setOwner(this);
    }
    else if constexpr (Multiple<N>) {
      std::apply([this](auto&&... in) {
        ((in.setOwner(this)), ...);
      }, in);
    }
  }

  template<std::size_t... Is>
  constexpr static decltype(auto) createInputs(std::index_sequence<Is...>) {
    constexpr bool allDimensionLess = std::ranges::all_of(Uin, [](auto e) { return e == SIUnit::create(); });   
    if constexpr (allDimensionLess) {
      return std::array<Input<Tin, SIUnit::create()>, N>{};
    }
    else {
      return std::tuple<Input<Tin, Uin[Is]>...>{};
    }
  }

  constexpr static decltype(auto) generateNInputs() {
    if constexpr (One<N>) {
      return Input<Tin, Uin[0U]>{};
    }
    else if constexpr (None<N>) {
      return Empty{};
    }
    else {
      return createInputs(std::make_index_sequence<N>{});
    }
  }

  template<std::size_t... Is>
  constexpr static decltype(auto) createOutputs(std::index_sequence<Is...>) {
    constexpr bool allDimensionLess = std::ranges::all_of(Uin, [](auto e) { return e == SIUnit::create(); });   
    if constexpr (allDimensionLess) {
      return std::array<Output<Tin, SIUnit::create()>, N>{};
    }
    else {
      return std::tuple<Output<Tin, Uout[Is]>...>{};
    }
  }

  constexpr static decltype(auto) generateMOutputs() {
    if constexpr (One<M>) {
      return Output<Tout, Uout[0U]>{};
    }
    else if constexpr (None<M>) {
      return Empty{};
    }
    else {
      return createOutputs(std::make_index_sequence<M>{});
    }
  }

 protected:
  decltype(generateNInputs()) in;
  decltype(generateMOutputs()) out;
};

/**
 * Operator overload (<<) to enable an easy way to print the state of a
 * block instance to an output stream.\n
 * Does not print a newline control character.
 */
template < uint8_t N, uint8_t M, typename Tin = double, typename Tout = Tin >
std::ostream& operator<<(std::ostream& os, Blockio<N,M,Tin,Tout>& b) {
  os << "Generic block: '" << b.getName() << "'"; 
  return os;
}

}
}
#endif /* ORG_EEROS_CONTROL_BLOCKIO_HPP_ */
