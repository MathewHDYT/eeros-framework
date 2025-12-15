#ifndef ORG_EEROS_CONTROL_SIUNIT_HPP_
#define ORG_EEROS_CONTROL_SIUNIT_HPP_

#include <array>
#include <utility>

namespace eeros {
namespace control {

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
 * @since v1.2.1
 */
struct SIUnit {
  const int length = 0;
  const int mass = 0;
  const int time = 0;
  const int electric_current = 0;
  const int thermodynamic_temperature = 0;
  const int amount_of_substance = 0;
  const int luminous_intensity = 0;
  const bool radian = false;

  auto operator<=>(const SIUnit&) const -> std::strong_ordering = default;
  auto operator==(const SIUnit&) const -> bool = default;

  template <int Length = 0, int Mass = 0, int Time = 0, int Electric_Current = 0, int Thermodynamic_Temperature = 0, int Amount_Of_Substance = 0, int Luminous_Intensity = 0, bool Radian = false>
  constexpr static SIUnit create()
  {
      //constexpr bool valid_radian = Radian ? (Length == 0 && Mass == 0 && Time == 0 && Electric_Current == 0 && Thermodynamic_Temperature == 0 && Amount_Of_Substance == 0 && Luminous_Intensity == 0) : false;
      //static_assert(valid_radian, "Invalid SI-Unit: Radian used in combination with other SI-Units.");
      return SIUnit{Length, Mass, Time, Electric_Current, Thermodynamic_Temperature, Amount_Of_Substance, Luminous_Intensity, Radian};
  }

  template<std::size_t N>
  constexpr static decltype(auto) generateNSizeArray() {
    return createArray<N>(std::make_index_sequence<N>{});
  }

private:
  constexpr SIUnit(int length, int mass, int time, int electric_current, int thermodynamic_temperature, int amount_of_substance, int luminous_intensity, bool radian)
    : length(length)
    , mass(mass)
    , time(time)
    , electric_current(electric_current)
    , thermodynamic_temperature(thermodynamic_temperature)
    , amount_of_substance(amount_of_substance)
    , luminous_intensity(luminous_intensity)
    , radian(radian) {}

  template<std::size_t N, std::size_t... Is>
  constexpr static decltype(auto) createArray(std::index_sequence<Is...>) {
    return std::array<SIUnit, N>{(static_cast<void>(Is), SIUnit::create())...};
  }
};

}
}
#endif /* ORG_EEROS_CONTROL_SIUNIT_HPP_ */
