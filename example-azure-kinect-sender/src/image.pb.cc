// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: image.proto

#include "image.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace kinect {
class ImageDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<Image>
      _instance;
} _Image_default_instance_;
}  // namespace kinect
namespace protobuf_image_2eproto {
static void InitDefaultsImage() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::kinect::_Image_default_instance_;
    new (ptr) ::kinect::Image();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::kinect::Image::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_Image =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsImage}, {}};

void InitDefaults() {
  ::google::protobuf::internal::InitSCC(&scc_info_Image.base);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::kinect::Image, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::kinect::Image, width_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::kinect::Image, height_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::kinect::Image, stride_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::kinect::Image, image_data_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::kinect::Image)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::kinect::_Image_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "image.proto", schemas, file_default_instances, TableStruct::offsets,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\013image.proto\022\006kinect\"J\n\005Image\022\r\n\005width\030"
      "\001 \001(\005\022\016\n\006height\030\002 \001(\005\022\016\n\006stride\030\003 \001(\005\022\022\n"
      "\nimage_data\030\004 \001(\014b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 105);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "image.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_image_2eproto
namespace kinect {

// ===================================================================

void Image::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Image::kWidthFieldNumber;
const int Image::kHeightFieldNumber;
const int Image::kStrideFieldNumber;
const int Image::kImageDataFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Image::Image()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_image_2eproto::scc_info_Image.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:kinect.Image)
}
Image::Image(const Image& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  image_data_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.image_data().size() > 0) {
    image_data_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.image_data_);
  }
  ::memcpy(&width_, &from.width_,
    static_cast<size_t>(reinterpret_cast<char*>(&stride_) -
    reinterpret_cast<char*>(&width_)) + sizeof(stride_));
  // @@protoc_insertion_point(copy_constructor:kinect.Image)
}

void Image::SharedCtor() {
  image_data_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&width_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&stride_) -
      reinterpret_cast<char*>(&width_)) + sizeof(stride_));
}

Image::~Image() {
  // @@protoc_insertion_point(destructor:kinect.Image)
  SharedDtor();
}

void Image::SharedDtor() {
  image_data_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void Image::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* Image::descriptor() {
  ::protobuf_image_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_image_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const Image& Image::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_image_2eproto::scc_info_Image.base);
  return *internal_default_instance();
}


void Image::Clear() {
// @@protoc_insertion_point(message_clear_start:kinect.Image)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  image_data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&width_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&stride_) -
      reinterpret_cast<char*>(&width_)) + sizeof(stride_));
  _internal_metadata_.Clear();
}

bool Image::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:kinect.Image)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 width = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &width_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 height = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(16u /* 16 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &height_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 stride = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(24u /* 24 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &stride_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bytes image_data = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(34u /* 34 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_image_data()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:kinect.Image)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:kinect.Image)
  return false;
#undef DO_
}

void Image::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:kinect.Image)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 width = 1;
  if (this->width() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->width(), output);
  }

  // int32 height = 2;
  if (this->height() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->height(), output);
  }

  // int32 stride = 3;
  if (this->stride() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->stride(), output);
  }

  // bytes image_data = 4;
  if (this->image_data().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      4, this->image_data(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:kinect.Image)
}

::google::protobuf::uint8* Image::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:kinect.Image)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 width = 1;
  if (this->width() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->width(), target);
  }

  // int32 height = 2;
  if (this->height() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->height(), target);
  }

  // int32 stride = 3;
  if (this->stride() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->stride(), target);
  }

  // bytes image_data = 4;
  if (this->image_data().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        4, this->image_data(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:kinect.Image)
  return target;
}

size_t Image::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:kinect.Image)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // bytes image_data = 4;
  if (this->image_data().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->image_data());
  }

  // int32 width = 1;
  if (this->width() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->width());
  }

  // int32 height = 2;
  if (this->height() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->height());
  }

  // int32 stride = 3;
  if (this->stride() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->stride());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Image::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:kinect.Image)
  GOOGLE_DCHECK_NE(&from, this);
  const Image* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Image>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:kinect.Image)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:kinect.Image)
    MergeFrom(*source);
  }
}

void Image::MergeFrom(const Image& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:kinect.Image)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.image_data().size() > 0) {

    image_data_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.image_data_);
  }
  if (from.width() != 0) {
    set_width(from.width());
  }
  if (from.height() != 0) {
    set_height(from.height());
  }
  if (from.stride() != 0) {
    set_stride(from.stride());
  }
}

void Image::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:kinect.Image)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Image::CopyFrom(const Image& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:kinect.Image)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Image::IsInitialized() const {
  return true;
}

void Image::Swap(Image* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Image::InternalSwap(Image* other) {
  using std::swap;
  image_data_.Swap(&other->image_data_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(width_, other->width_);
  swap(height_, other->height_);
  swap(stride_, other->stride_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata Image::GetMetadata() const {
  protobuf_image_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_image_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace kinect
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::kinect::Image* Arena::CreateMaybeMessage< ::kinect::Image >(Arena* arena) {
  return Arena::CreateInternal< ::kinect::Image >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
