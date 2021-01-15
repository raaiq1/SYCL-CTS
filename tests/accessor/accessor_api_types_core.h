/*******************************************************************************
//
//  SYCL 1.2.1 Conformance Test Suite
//
//  Provide common code for accessor API verification with core types
//  with or without atomic64 extension
//
*******************************************************************************/

#ifndef SYCL_1_2_1_TESTS_ACCESSOR_ACCESSOR_API_TYPES_CORE_H
#define SYCL_1_2_1_TESTS_ACCESSOR_ACCESSOR_API_TYPES_CORE_H

#include "../common/common.h"
#include "../common/type_coverage.h"
#include "./../../util/extensions.h"

#ifndef TEST_NAME
#error Invalid test namespace
#endif

namespace TEST_NAMESPACE {

/**
 *  @brief Run specific accessors' tests for core type set and
 *         for generic or atomic64 code path
 */
template <template <typename, typename> class action,
          typename extensionTagT>
class check_all_types_core {

  template <typename T>
  using check_type = action<T, extensionTagT>;

public:
  static void run(cl::sycl::queue& queue, sycl_cts::util::logger &log) {

    // Skip tests in case extension not available; can fire for atomic64
    using availability =
        sycl_cts::util::extensions::availability<extensionTagT>;
    if (!availability::check(queue, log))
      return;

#ifndef SYCL_CTS_FULL_CONFORMANCE
    // Specific set of types to cover during ordinary compilation

    const auto vector_types = named_type_pack<int>({"int"});
    const auto scalar_types =
        named_type_pack<float,
                        std::size_t,
                        user_struct>({
                        "float",
                        "std::size_t",
                        "user_struct"});
#else
    // Extended type coverage

    const auto vector_types =
        named_type_pack<bool,
                        char, signed char, unsigned char,
                        short, unsigned short,
                        int, unsigned int,
                        long, unsigned long,
                        long long, unsigned long long,
                        float, cl::sycl::cl_float,
                        cl::sycl::byte,
                        cl::sycl::cl_bool,
                        cl::sycl::cl_char, cl::sycl::cl_uchar,
                        cl::sycl::cl_short, cl::sycl::cl_ushort,
                        cl::sycl::cl_int, cl::sycl::cl_uint,
                        cl::sycl::cl_long, cl::sycl::cl_ulong>({
                        "bool",
                        "char", "signed char", "unsigned char",
                        "short", "unsigned short",
                        "int", "unsigned int",
                        "long", "unsigned long",
                        "long long", "unsigned long long",
                        "float", "cl::sycl::cl_float",
                        "cl::sycl::byte",
                        "cl::sycl::cl_bool",
                        "cl::sycl::cl_char", "cl::sycl::cl_uchar",
                        "cl::sycl::cl_short", "cl::sycl::cl_ushort",
                        "cl::sycl::cl_int", "cl::sycl::cl_uint",
                        "cl::sycl::cl_long", "cl::sycl::cl_ulong"});
    const auto scalar_types =
        named_type_pack<std::size_t, user_struct>({
                        "std::size_t", "user_struct"});

#ifdef INT8_MAX
    if (!std::is_same<std::int8_t, cl::sycl::cl_char>::value) {
      for_type_and_vectors<check_type, std::int8_t>(
          log, queue, "std::int8_t");
    }
#endif
#ifdef UINT8_MAX
    if (!std::is_same<std::uint8_t, cl::sycl::cl_uchar>::value) {
      for_type_and_vectors<check_type, std::uint8_t>(
          log, queue, "std::uint8_t");
    }
#endif
#ifdef INT16_MAX
    if (!std::is_same<std::int16_t, cl::sycl::cl_short>::value) {
      for_type_and_vectors<check_type, std::int16_t>(
          log, queue, "std::int16_t");
    }
#endif
#ifdef UINT16_MAX
    if (!std::is_same<std::uint16_t, cl::sycl::cl_ushort>::value) {
      for_type_and_vectors<check_type, std::uint16_t>(
          log, queue, "std::uint16_t");
    }
#endif
#ifdef INT32_MAX
    if (!std::is_same<std::int32_t, cl::sycl::cl_int>::value) {
      for_type_and_vectors<check_type, std::int32_t>(
          log, queue, "std::int32_t");
    }
#endif
#ifdef UINT32_MAX
    if (!std::is_same<std::uint32_t, cl::sycl::cl_uint>::value) {
      for_type_and_vectors<check_type, std::uint32_t>(
          log, queue, "std::uint32_t");
    }
#endif
#ifdef INT64_MAX
    if (!std::is_same<std::int64_t, cl::sycl::cl_long>::value) {
      for_type_and_vectors<check_type, std::int64_t>(
          log, queue, "std::int64_t");
    }
#endif
#ifdef UINT64_MAX
    if (!std::is_same<std::uint64_t, cl::sycl::cl_ulong>::value) {
      for_type_and_vectors<check_type, std::uint64_t>(
          log, queue, "std::uint64_t");
    }
#endif

#endif // SYCL_CTS_FULL_CONFORMANCE

    for_all_types_and_vectors<check_type>(vector_types, log, queue);

    for_all_types<check_type>(scalar_types, log, queue);

    queue.wait_and_throw();
  }
};

}  // namespace TEST_NAMESPACE

#endif // SYCL_1_2_1_TESTS_ACCESSOR_ACCESSOR_API_TYPES_CORE_H
