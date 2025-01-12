/*******************************************************************************
//
//  SYCL 2020 Conformance Test Suite
//
//  Provides tests that the sycl::local_accessor can access the memory shared
//  among work-items.
//
*******************************************************************************/
#ifndef SYCL_CTS_LOCAL_ACCESSOR_ACCESS_AMONG_WORK_ITEMS_H
#define SYCL_CTS_LOCAL_ACCESSOR_ACCESS_AMONG_WORK_ITEMS_H
#include "accessor_common.h"

namespace local_accessor_access_among_work_items {
using namespace sycl_cts;
using namespace accessor_tests_common;

/**
 * @brief Provides a functor that provides verification that local_accessor can
 *        access the memory shared among work-items
 * @tparam T Current data type
 * @tparam DimensionTypeT Current current dimension size
 */
template <typename T, typename DimensionTypeT>
class run_test {
  static constexpr int Dimension = DimensionT::value;

 public:
  /**
   * @brief Functor that provides verification that local_accessor can access
   *        the memory shared among work-items
   * @param type_name Current data type string representation
   */
  void operator()(const std::string& type_name) {
    auto queue = util::get_cts_object::queue();

    auto section_name = get_section_name<Dimension>(
        type_name,
        "Verify possibility to access the memory shared among work-items. "
        "[local_accessor]");
    SECTION(section_name) {
      T values_arr[2] = {1, 2};
      const sycl::range range(2);
      constexpr T valid_value = 5;
      constexpr T invalid_value = 6;

      bool is_acc_val_equal_to_expected = false;
      {
        sycl::buffer<bool> val_is_equal_to_expected_buffer(
            &is_acc_val_equal_to_expected, sycl::range(1));
        queue.submit([&](sycl::handler& cgh) {
          auto val_is_equal_to_expected_acc =
              val_is_equal_to_expected_buffer
                  .template get_access<sycl::access_mode::write>(cgh);
          sycl::local_accessor<T, Dimension> acc(range, cgh);
          cgh.parallel_for(
              sycl::nd_range(range, range), [=](sycl::nd_item item) {
                auto lid = item.get_local_id(0);
                // Initialize local memory with invalid value
                acc[lid] = invalid_value;
                // Wait for work-items to finish initialization
                sycl::group_barrier(item.get_group());

                // Work-items with index greater than 0 writes to valid data to
                // the first element of the local data
                if (lid != 0) {
                  acc[0] = valid_value;
                }

                // Wait for data store to finish
                sycl::group_barrier(item.get_group());
                // 0th work-item reports the result
                if (lid == 0) {
                  val_is_equal_to_expected_acc[0] = acc[0] == valid_value;
                }
              });
        });
      }
      CHECK(is_acc_val_equal_to_expected);
    }
  }
};

template <typename T>
class run_local_accessor_access_among_work_items_tests {
 public:
  void operator()(const std::string& type_name) {
    // Type packs instances have to be const, otherwise for_all_combination will
    // not compile
    const auto dimensions = get_dimensions();

    for_all_combinations<run_test, T>(dimensions, type_name);
  }
};
}  // namespace local_accessor_access_among_work_items

#endif  // SYCL_CTS_LOCAL_ACCESSOR_ACCESS_AMONG_WORK_ITEMS_H
