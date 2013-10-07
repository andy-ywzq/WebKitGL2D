include(FindEXPAT)
find_package(EXPAT REQUIRED)

# Try to find webrtc code based on PKG_CONFIG_PATH
# This hack is needed because there's no way to install libWebRTC.
string(REPLACE ":" "/../../../Source/webrtc/trunk;" _hints "$ENV{PKG_CONFIG_PATH}")
find_path(WEBRTCLIB_INCLUDE_DIRS
    NAMES talk/app/webrtc/mediaconstraintsinterface.h
    HINTS ${_hints}
)

set(_libraries
out/Release/obj/talk/libjingle_peerconnection.a
out/Release/obj/talk/libjingle_p2p.a
out/Release/obj/talk/libjingle_media.a
out/Release/obj/webrtc/modules/libvideo_capture_module.a
out/Release/obj/webrtc/modules/libvideo_render_module.a
out/Release/obj/webrtc/video_engine/libvideo_engine_core.a
out/Release/obj/webrtc/voice_engine/libvoice_engine.a
out/Release/obj/webrtc/modules/libacm2.a
out/Release/obj/third_party/libsrtp/libsrtp.a
out/Release/obj/webrtc/modules/libwebrtc_video_coding.a
out/Release/obj/webrtc/modules/libwebrtc_i420.a
out/Release/obj/webrtc/modules/libwebrtc_utility.a
out/Release/obj/webrtc/common_video/libcommon_video.a
out/Release/libyuv.a
out/Release/obj/webrtc/modules/libvideo_processing.a
out/Release/obj/webrtc/system_wrappers/source/libsystem_wrappers.a
out/Release/obj/third_party/libjpeg_turbo/libjpeg_turbo.a
out/Release/obj/webrtc/modules/librtp_rtcp.a
out/Release/obj/webrtc/modules/libpaced_sender.a
out/Release/obj/webrtc/modules/remote_bitrate_estimator/librbe_components.a
out/Release/obj/webrtc/modules/libremote_bitrate_estimator.a
out/Release/obj/webrtc/modules/libbitrate_controller.a
out/Release/obj/webrtc/modules/libaudio_device.a
out/Release/obj/webrtc/modules/libaudio_processing.a
out/Release/obj/webrtc/modules/libaudio_coding_module.a
out/Release/obj/webrtc/modules/libmedia_file.a
out/Release/obj/webrtc/modules/libaudio_conference_mixer.a
out/Release/obj/webrtc/modules/video_coding/utility/libvideo_coding_utility.a
out/Release/obj/talk/libjingle_sound.a
out/Release/obj/webrtc/modules/libG711.a
out/Release/obj/webrtc/modules/libaudioproc_debug_proto.a
out/Release/obj/webrtc/modules/libG722.a
out/Release/obj/webrtc/modules/libaudio_processing_sse2.a
out/Release/obj/webrtc/modules/libiSAC.a
out/Release/obj/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a
out/Release/obj/third_party/libvpx/libvpx.a
out/Release/obj/third_party/libvpx/libvpx_intrinsics_mmx.a
out/Release/obj/third_party/libvpx/libvpx_intrinsics_sse2.a
out/Release/obj/third_party/libvpx/libvpx_intrinsics_ssse3.a
out/Release/obj/third_party/libvpx/libvpx_asm_offsets_vp8.a
out/Release/obj/third_party/libvpx/libvpx.a # libvpx must be here twice due to a circular dependency
out/Release/obj/webrtc/modules/libNetEq.a
out/Release/obj/webrtc/modules/libNetEq4.a
out/Release/obj/webrtc/modules/libiLBC.a
out/Release/obj/webrtc/modules/libCNG.a
out/Release/obj/webrtc/common_audio/libcommon_audio.a
out/Release/obj/webrtc/modules/libvideo_processing_sse2.a
out/Release/obj/webrtc/modules/libPCM16B.a
out/Release/obj/third_party/protobuf/libprotobuf_lite.a
out/Release/obj/webrtc/common_audio/libcommon_audio_sse2.a
out/Release/obj/webrtc/modules/libwebrtc_opus.a
out/Release/obj/third_party/opus/libopus.a
out/Release/obj/talk/libjingle.a
out/Release/obj/third_party/openssl/libopenssl.a
)

set(WEBRTCLIB_LIBRARIES ${EXPAT_LIBRARIES})
foreach (_lib ${_libraries})
    get_filename_component(_hint ${_lib} PATH)
    get_filename_component(_libname ${_lib} NAME)
    get_filename_component(_libVarName ${_lib} NAME_WE)
    string(TOUPPER "WEBRTCLIB_${_libVarName}" _libVarName)
    if (NOT ${_libVarName})
        find_library(${_libVarName}
            NAME ${_libname}
            HINTS "${WEBRTCLIB_INCLUDE_DIRS}/${_hint}")
        list(APPEND _libvars ${_libVarName})
    endif ()
    list(APPEND WEBRTCLIB_LIBRARIES ${${_libVarName}})
endforeach ()

find_package_handle_standard_args(WebRTC
    REQUIRED_VARS
    WEBRTCLIB_INCLUDE_DIRS
    ${_libvars})
