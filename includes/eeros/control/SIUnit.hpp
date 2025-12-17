#ifndef ORG_EEROS_CONTROL_SIUNIT_HPP_
#define ORG_EEROS_CONTROL_SIUNIT_HPP_

#include <array>
#include <utility>

namespace eeros {
namespace control {

/**
 * Represents all combined SI base units. Being an integer individually representing the power of that specific base unit.
 * Therefore a length value of 2 and a time value of -1, means metre squared divided by seconds.
 * 
 * To differentiate between radian values the struct also contains a simply boolean being either true if the unit is in radians or false if not.
 * 
 * Internal members have to be public to be used as a template parameter type.
 * See non-closure literal class type https://en.cppreference.com/w/cpp/language/template_parameters.html for more information on the requirements on the class.
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

  /**
   * Defaulted three-way comparsion, handles all comparsion cases in one by simply comparing the base member types. 
   */
  auto operator<=>(const SIUnit&) const -> std::strong_ordering = default;

  /**
   * Defaulted three-way comparsion, handles all comparsion cases in one by simply comparing the base types. 
   */
  auto operator==(const SIUnit&) const -> bool = default;

  /**
   * Creates an instance of the SIUnit, only outside of the class way to construct an instnace, because default constructor is deleted and custom constructor private.
   * Done to ensure if a SIUnit is created that it fufills certain criteria, which can be expanded and otherwise will fail compilation.
   * 
   * To allow for those compile time checks the SI base units have to be passed as template parameters, allowing checks on them using static_assert, evaluated at compile-time.
   * 
   * @return SIUnit
   */
  template <int Length = 0, int Mass = 0, int Time = 0, int Electric_Current = 0, int Thermodynamic_Temperature = 0, int Amount_Of_Substance = 0, int Luminous_Intensity = 0, bool Radian = false>
  constexpr static SIUnit create()
  {
    // Allows calls to static_assert(...) where certain combinbations of the passed template parameters are invalid ensuring only valid SIUnits are constructed
    return SIUnit{Length, Mass, Time, Electric_Current, Thermodynamic_Temperature, Amount_Of_Substance, Luminous_Intensity, Radian};
  }

  /**
   * Generates an array containing an arbitrary amount of dimensionless SIUnit instances.
   * 
   * @tparam N Amount of dimensionless SIUnit instances to generate.
   * @return SIUnits
   */
  template<std::size_t N>
  constexpr static decltype(auto) generateNSizeArray() {
    return createArray<N>(std::make_index_sequence<N>{});
  }

private:
  /**
   * Privated constructor intializing the base units, used to prevent instantiation of an SIUnit besides the usage of SIUnit::create().
   */
  constexpr SIUnit(int length, int mass, int time, int electric_current, int thermodynamic_temperature, int amount_of_substance, int luminous_intensity, bool radian)
    : length(length)
    , mass(mass)
    , time(time)
    , electric_current(electric_current)
    , thermodynamic_temperature(thermodynamic_temperature)
    , amount_of_substance(amount_of_substance)
    , luminous_intensity(luminous_intensity)
    , radian(radian) {}

  /**
   * Generates an array containing an arbitrary amount of dimensionless SIUnit instances.
   * 
   * @tparam N Amount of dimensionless SIUnit instances to generate.
   * @tparam Is Template parameter pack of a sequence from 0 - m, used to fill the array type with values.
   * @return SIUnits
   */
  template<std::size_t N, std::size_t... Is>
  constexpr static decltype(auto) createArray(std::index_sequence<Is...>) {
    return std::array<SIUnit, N>{(static_cast<void>(Is), SIUnit::create())...};
  }
};

}
}
#endif /* ORG_EEROS_CONTROL_SIUNIT_HPP_ */
