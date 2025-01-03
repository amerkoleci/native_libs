set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(KTX_SOURCES
    ${KTX_DIR}/external/dfdutils/colourspaces.c
    ${KTX_DIR}/external/dfdutils/createdfd.c
    ${KTX_DIR}/external/dfdutils/dfd.h
    ${KTX_DIR}/external/dfdutils/dfd2vk.inl
    ${KTX_DIR}/external/dfdutils/interpretdfd.c
    ${KTX_DIR}/external/dfdutils/printdfd.c
    ${KTX_DIR}/external/dfdutils/queries.c
    ${KTX_DIR}/external/dfdutils/vk2dfd.c
    ${KTX_DIR}/external/etcdec/etcdec.cxx
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_averages_and_directions.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_block_sizes.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_color_quantize.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_color_unquantize.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_compress_symbolic.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_compute_variance.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_decompress_symbolic.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_diagnostic_trace.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_entry.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_find_best_partitioning.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_ideal_endpoints_and_weights.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_image.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_integer_sequence.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_mathlib.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_mathlib_softfloat.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_partition_tables.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_percentile_tables.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_pick_best_endpoint_format.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_quantization.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_symbolic_physical.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_weight_align.cpp
    ${KTX_DIR}/external/astc-encoder/Source/astcenc_weight_quant_xfer_tables.cpp
    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/etcunpack.cxx
    ${KTX_DIR}/lib/filestream.c
    ${KTX_DIR}/lib/filestream.h
    ${KTX_DIR}/lib/formatsize.h
    ${KTX_DIR}/lib/hashlist.c
    ${KTX_DIR}/lib/info.c
    ${KTX_DIR}/lib/ktxint.h
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/memstream.h
    ${KTX_DIR}/lib/strings.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/uthash.h
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/texture.h
    ${KTX_DIR}/utils/unused.h
    
    # Basis Universal
    ${KTX_DIR}/lib/astc_codec.cpp
    ${KTX_DIR}/lib/basis_sgd.h
    ${KTX_DIR}/lib/basis_transcode.cpp
    ${KTX_DIR}/lib/miniz_wrapper.cpp
    ${KTX_DIR}/external/basisu/transcoder/basisu_containers.h
    ${KTX_DIR}/external/basisu/transcoder/basisu_containers_impl.h
    ${KTX_DIR}/external/basisu/transcoder/basisu_file_headers.h
    ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder_internal.h
    ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder_uastc.h
    ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder.cpp
    ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder.h
    ${KTX_DIR}/external/basisu/transcoder/basisu.h
    ${KTX_DIR}/external/basisu/zstd/zstd.c

    # KT1
    ${KTX_DIR}/lib/texture1.c
    ${KTX_DIR}/lib/texture1.h

    # KTX2
    ${KTX_DIR}/lib/texture2.c
    ${KTX_DIR}/lib/texture2.h

    # Vulkan support
    ${KTX_DIR}/lib/vk_format.h
    ${KTX_DIR}/lib/vkformat_check.c
    ${KTX_DIR}/lib/vkformat_enum.h
    ${KTX_DIR}/lib/vkformat_str.c
    ${KTX_DIR}/lib/vkformat_typesize.c
    #${KTX_DIR}/lib/vk_funcs.c
    #${KTX_DIR}/lib/vk_funcs.h
    #${KTX_DIR}/lib/vkloader.c
)

set(KTX_INCLUDE_DIRS
    ${KTX_DIR}/include
    ${KTX_DIR}/lib
    ${KTX_DIR}/utils
    ${KTX_DIR}/external    
    ${KTX_DIR}/external/basisu/zstd
    ${KTX_DIR}/external/basisu/transcoder
    ${KTX_DIR}/other_include
)

add_library(ktx ${KTX_SOURCES})

target_compile_definitions(ktx PUBLIC LIBKTX)
if (WIN32)
target_compile_definitions(ktx PUBLIC "KTX_API=__declspec(dllexport)")
endif()
target_compile_definitions(ktx PUBLIC KTX_FEATURE_WRITE=0)
target_compile_definitions(ktx PUBLIC BASISD_SUPPORT_KTX2_ZSTD=0)
target_compile_definitions(ktx PUBLIC BASISU_NO_ITERATOR_DEBUG_LEVEL)

target_include_directories(ktx SYSTEM PUBLIC ${KTX_INCLUDE_DIRS})

#target_link_libraries(ktx PUBLIC vulkan)
