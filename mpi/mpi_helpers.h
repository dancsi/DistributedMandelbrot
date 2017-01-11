#pragma once

#include <utility>

#include <gsl/gsl>
#include <mpi.h>

#include "common/logging.h"
#include "message_tags.h"

namespace MPI {
	using logging::console;
	
	namespace detail {
		constexpr MPI_Datatype type_array[] = {MPI_INT, MPI_UINT32_T, MPI_FLOAT, MPI_DOUBLE, MPI_BYTE};

		using namespace std;
		template <typename T>
		struct TypeLookupBasic{};

		template<>
		struct TypeLookupBasic<int> : integral_constant<int, 0> {};
		template<>
		struct TypeLookupBasic<uint32_t> : integral_constant<int, 1> {};
		template<>
		struct TypeLookupBasic<float> : integral_constant<int, 2> {};
		template<>
		struct TypeLookupBasic<double> : integral_constant<int, 3> {};
		template<>
		struct TypeLookupBasic<char> : integral_constant<int, 4> {};
		template<>
		struct TypeLookupBasic<unsigned char> : integral_constant<int, 4> {};
		template<>
		struct TypeLookupBasic<void*> : integral_constant<int, 4> {};

		template<typename T>
		struct TypeLookupScalar : TypeLookupBasic<remove_cv_t<T>> {};

		template<typename T>
		struct TypeLookupSpan {};
		template<typename T>
		struct TypeLookupSpan<gsl::span<T>> : TypeLookupScalar<T> {};

		template<typename T, bool is_scalar>
		struct TypeLookupImpl {};
		template<typename T>
		struct TypeLookupImpl<T, true> : TypeLookupScalar<T> {};
		template<typename T>
		struct TypeLookupImpl<T, false> : TypeLookupSpan<T> {};

		template<typename T>
		struct TypeLookupCVRRemoved : TypeLookupImpl<T, is_scalar<T>::value> {};

		template<typename T>
		struct TypeLookup : TypeLookupCVRRemoved<remove_reference_t<T>> {};

		template <typename T> 
		constexpr MPI_Datatype TypeLookup_v = type_array[TypeLookup<T>::value];

		template <typename T>
		struct BufferWrapper;

#define ENABLE_IF_T_SCALAR template <typename T, typename = enable_if_t<is_scalar<T>::value, T> >

		ENABLE_IF_T_SCALAR
		constexpr int buffer_length(T& v) {
			return 1;
		}

		ENABLE_IF_T_SCALAR
		T* buffer_address(T& v) {
			return &v;
		}

		ENABLE_IF_T_SCALAR
		int buffer_length(gsl::span<T> v) {
			return v.size();
		}

		ENABLE_IF_T_SCALAR
		void* buffer_address(gsl::span<T> v) {
			return v.data();
		}

	}

	template<typename T>
	void Send(T&& v, int dest, MPI_Comm comm, int tag = static_cast<int>(Tag::none)) {
		MPI_Send(detail::buffer_address(v), detail::buffer_length(v), detail::TypeLookup_v<T>, dest, tag, comm);
	}

	template<typename T>
	int Recv(T&& v, int source, int tag, MPI_Comm comm, MPI_Status* status) {
		return MPI_Recv(detail::buffer_address(v), detail::buffer_length(v), detail::TypeLookup_v<T>, source, tag, comm, status);
	}

	template<typename T>
	int Recv(T&& v, MPI_Comm comm) {
		return Recv(std::forward<T>(v), MPI_ANY_SOURCE, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
	}

	template<typename T>
	int Bcast(T&& v, int root, MPI_Comm comm) {
		return MPI_Bcast(detail::buffer_address(v), detail::buffer_length(v), detail::TypeLookup_v<T>, root, comm);
	}

	void Barrier(MPI_Comm comm) {
		MPI_Barrier(comm);
	}

	struct Context {
		int rank, world_size;
		MPI_Comm comm;

		Context(int *argc, char*** argv) {
			console->trace("Initializing MPI");
			int provided_threading_level;
			MPI_Init_thread(argc, argv, MPI_THREAD_SERIALIZED, &provided_threading_level);
			if (provided_threading_level < MPI_THREAD_SERIALIZED) {
				logging::log_and_throw("The MPI virtual queue requires the provided threading level to be at least MPI_THREAD_SERIALIZED. Instead, it received {}", provided_threading_level);
			}
			console->trace("Initialized MPI");
			MPI_Comm_dup(MPI_COMM_WORLD, &comm);

			MPI_Comm_size(comm, &world_size);
			MPI_Comm_rank(comm, &rank);
			console->trace("rank = {}, size = {}", rank, world_size);

			logging::set_process_name(std::to_string(rank));
		}

		virtual ~Context() {
			console->info("Closing MPI connection");
			//MPI_Comm_disconnect(&comm);
			MPI_Finalize();
			console->info("MPI connection closed");
		}
	};
}
